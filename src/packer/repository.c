#include <assert.h>
#include <packer/repository.h>
#include <curl/curl.h>
#include <stdio.h>
#include <sys/stat.h>
#define REPOSITORY_PREFIX "./repository/"

bool is_package_id(const char* package_id) {
  size_t length = strlen(package_id);
  bool found_slash = false;
  for (size_t i = 0; i < length; ++i) {
    if (package_id[i] == '/') {
      if (!found_slash) {
        found_slash = true;
      } else {
        return false;
      }
    }
  }
  return found_slash;
}

char* packerfile_of(const char* package_id) {
  const size_t package_id_length = strlen(package_id);
  const size_t prefix_length = strlen(REPOSITORY_PREFIX);
  const size_t path_size = prefix_length + package_id_length;
  char* output = malloc(prefix_length + package_id_length+ 1);
  snprintf(output, path_size + 1, "%s%s", REPOSITORY_PREFIX, package_id);
  output[path_size] = '\0';
  return output;
}

char* packerdir_of(const char* group) {
  const size_t group_length = strlen(group);
  const size_t prefix_length = strlen(REPOSITORY_PREFIX);
  const size_t path_size = prefix_length + group_length;
  char* output = malloc(prefix_length + group_length+ 1);
  snprintf(output, path_size + 1, "%s%s", REPOSITORY_PREFIX, group);
  output[path_size] = '\0';
  return output;
}

bool split_package_id(const char* package_id, char** recipient_group, char** recipient_name) {
  char* buffer = strdup(package_id);

  const char* c_group = strtok(buffer, "/");
  if (c_group == NULL)
    return false;
  *recipient_group = strdup(c_group);

  const char* c_name = strtok(NULL, "/");
  if (c_name == NULL) {
    free(*recipient_group);
    *recipient_group = NULL;
    return false;
  }

  *recipient_name = strdup(c_name);
  free(buffer);
  return true;
}

bool create_packerdir_if_needed(const char* group) {
  char* packerdir = packerdir_of(group);
  bool esit = (mkdir(packerdir, S_IRWXU) == 0);
  free(packerdir);
  return esit;
}

bool download_dockerfile(const char* packerfile, const char* group, const char* name) {
  if (!create_packerdir_if_needed(group)) {
    return false;
  }

  const char* template_url = "https://raw.githubusercontent.com/%s/%s/refs/heads/master/Packerfile";
  const size_t nargs = 2;
  const size_t url_size = strlen(template_url) + strlen(group) + strlen(name) - nargs*2;
  char* url = malloc(url_size + 1);
  snprintf(url, url_size + 1, template_url, group, name);
  url[url_size] = '\0';

  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  assert (curl);

  FILE *fp = fopen(packerfile, "wb");
  printf("packerfile: %s\n", packerfile);
  assert(fp != NULL);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  res = curl_easy_perform(curl);

  int32_t http_code = 0;

  if (res == CURLE_OK) {
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
  }

  curl_easy_cleanup(curl);
  free(url);

  fclose(fp);
  if (http_code != 200) {
    assert(remove(packerfile) == 0);
    return false;
  }
  return true;
}

bool retrieve_package_into(Package* package, const char* package_id) {
  char* packerfile_path = packerfile_of(package_id);
  if (!split_package_id(package_id, &package->group, &package->name)) {
    return false;
  }

  if (!file_exists(packerfile_path)) {
    if (!download_dockerfile(packerfile_path, package->group, package->name)) {
      free(package->group);
      package->group = NULL;
      free(package->name);
      package->name = NULL;
      free(packerfile_path);
      packerfile_path = NULL;
      return false;
    }
  }
  if (!Packerfile__parse_into(&package->packerfile, packerfile_path)) {
    free(package->group);
    package->group = NULL;
    free(package->name);
    package->name = NULL;
    free(packerfile_path);
    packerfile_path = NULL;
    return false;
  }
  return true;
}

Package* retrive_package(const char* package_id) {
  Package* package = Package__new();
  if (!retrieve_package_into(package, package_id)) {
    Package__delete(package);
    package = NULL;
  }
  return package;
}
