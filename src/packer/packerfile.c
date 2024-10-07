#include <packer/packerfile.h>
#include <string.h>
#include <ts-yaml.h>

char* strdup(const char* src) {
  size_t length = strlen(src);
  char* dst = (char*) malloc(sizeof(char) * (length + 1));
  strncpy(dst, src, length);
  dst[length] = '\0';
  return dst;
}

Packerfile* Packerfile__new() {
  Packerfile* packerfile = (Packerfile*) malloc(sizeof(Packerfile));
  Packerfile__init(packerfile);
  return packerfile;
}

void Packerfile__delete(Packerfile* packerfile) {
  if (packerfile != NULL) {
    Packerfile__clean(packerfile);
    free(packerfile);
  }
}

void Packerfile__init(Packerfile* packerfile) {
  packerfile->version = NULL;
  packerfile->license = NULL;
  packerfile->summary = NULL;
  Map_string_Vector_string__init(&packerfile->makedepends);
  Map_string_Vector_string__init(&packerfile->depends);
}

void Packerfile__clean(Packerfile* packerfile) {
  if (packerfile->version != NULL) {
    free(packerfile->version);
    packerfile->version = NULL;
  }
  if (packerfile->license != NULL) {
    free(packerfile->license);
    packerfile->license = NULL;
  }
  if (packerfile->summary != NULL) {
    free(packerfile->summary);
    packerfile->summary = NULL;
  }
  Map_string_Vector_string__clean(&packerfile->makedepends);
  Map_string_Vector_string__clean(&packerfile->depends);
}

void Packerfile__fprint(FILE* fd, const Packerfile* packerfile) {
  fprintf(fd, "version: %s\n", packerfile->version);
  fprintf(fd, "license: %s\n", packerfile->license);
  fprintf(fd, "summary: %s\n", packerfile->summary);
  fprintf(fd, "makedepends:\n");
  for (__auto_type distro = Map_string_Vector_string__cbegin(&packerfile->makedepends);
       distro != Map_string_Vector_string__cend(&packerfile->makedepends);
       ++distro) {
    fprintf(fd, "  %s:\n", distro->first);
    for (__auto_type package = Vector_string__cbegin(&distro->second);
        package != Vector_string__cend(&distro->second);
        ++package) {
      fprintf(fd, "  - %s\n", *package);
    }
  }
  fprintf(fd, "depends:\n");
  for (__auto_type distro = Map_string_Vector_string__cbegin(&packerfile->depends);
       distro != Map_string_Vector_string__cend(&packerfile->depends);
       ++distro) {
    fprintf(fd, "  %s:\n", distro->first);
    for (__auto_type package = Vector_string__cbegin(&distro->second);
        package != Vector_string__cend(&distro->second);
        ++package) {
      fprintf(fd, "  - %s\n", *package);
    }
  }
}

static inline bool parse_string_field(struct YamlObject* map, const char* field, bool optional, char** recipient) {
  if (!YamlObject__contains(map, field)) {
    if (optional)
      return true;
    printf("field '%s' is not optional\n", field);
    return false;
  }
  struct YamlObject* value = YamlObject__get(map, field);
  if (!YamlObject__is_string(value)) {
    printf("field '%s' should be a string\n", field);
    return false;
  }
  *recipient = strdup(value->string);
  return true;
}

static inline bool parse_dependency_field(struct YamlObject* map, const char* field, bool optional, Map_string_Vector_string* recipient) {
  if (!YamlObject__contains(map, field)) {
    if (optional)
      return true;
    printf("field '%s' is not optional\n", field);
    return false;
  }
  
  struct YamlObject* dependencies = YamlObject__get(map, field);
  if (!YamlObject__is_map(dependencies)) {
    printf("field '%s' should be a map\n", field);
    return false;
  }

  for (__auto_type it = Map_string_YamlObjectp__begin(&dependencies->map);
       it != Map_string_YamlObjectp__end(&dependencies->map);
       ++it) {
    if (!YamlObject__is_string(it->second)) {
      printf("field '%s':'%s' should be a string\n", field, it->first);
      return false;
    }
    char* line = strdup(it->second->string);
    char* distro = strdup(it->first);
    Vector_string packages;
    Vector_string__init(&packages);

    const char* token = strtok(line, " ");
    while (token != NULL) {
      if (strlen(token) > 0) {
        Vector_string__push_back(&packages, strdup(token));
      }
      token = strtok(NULL, " ");
    }

    free(line);
    Map_string_Vector_string__put(recipient, distro, packages);
  }

  return true;
}

bool Packerfile__parse_into(Packerfile* packerfile, const char* filepath) {
  struct YamlObject* value = parse_yaml_file(filepath);
  if (value == NULL)
    return false;

  if (!YamlObject__is_map(value)) {
    return false;
  }

  if (!(parse_string_field(value, "version", false,&packerfile->version)
        && parse_string_field(value, "license", false,&packerfile->license)
        && parse_string_field(value, "summary", false,&packerfile->summary)
        && parse_dependency_field(value, "makedepends", true, &packerfile->makedepends)
        && parse_dependency_field(value, "depends", true, &packerfile->depends))) {
    Packerfile__delete(packerfile);
    return false;
  }

  YamlObject__delete(value);
  return true;
}

Packerfile* Packerfile__parse(const char* filepath) {
  Packerfile* packerfile = Packerfile__new();
  if (!Packerfile__parse_into(packerfile, filepath)) {
    Packerfile__delete(packerfile);
    packerfile = NULL;
  }
  return packerfile;
}
