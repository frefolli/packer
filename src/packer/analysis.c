#include <packer/analysis.h>
#include <packer/repository.h>
#include <assert.h>
#include <stdlib.h>

Package* get_package_from(Host* host, Map_string_Package* packages, Vector_string* queue, const char* package_id) {
  __auto_type it = Map_string_Package__find(packages, package_id);
  if (it == Map_string_Package__end(packages)) {
    Package package;
    Package__init(&package);
    if (!retrieve_package_into(host, &package, package_id)) {
      fprintf(stderr, "unable to read info for package '%s'\n", package_id);
      fprintf(stderr, "Packerfile may not be present in remote repository or the file in the cache could be corrupted.\n");
      return NULL;
    }
    Map_string_Package__put(packages, strdup(package_id), package);
    it = Map_string_Package__find(packages, package_id);
    assert((it != Map_string_Package__end(packages)));
    Vector_string__push_back(queue, (char*)package_id);
  }
  return &it->second;
}

bool analyze_package(Host* host, Map_string_Package* packages, Vector_string* queue, const char* package_id) {
  Package* package = get_package_from(host, packages, queue, package_id);
  if (package == NULL) {
    return false;
  }
  for (__auto_type it = Vector_string__begin(package->makedepends);
      it != Vector_string__end(package->makedepends);
      ++it) {
    const char* dependency_id = *it;
    if (is_package_id(dependency_id)) {
      Package* dependency = get_package_from(host, packages, queue, dependency_id);
      if (dependency == NULL) {
       return false;
      }
      package = get_package_from(host, packages, queue, package_id);
      package->depends_on += 1;
      dependency->depended_on += 1;
    }
  }
  return true;
}

int compare_Pair_string_Package(const void* a, const void* b) {
  Pair_string_Package* A = (Pair_string_Package*) a;
  Pair_string_Package* B = (Pair_string_Package*) b;
  if (A->second.depends_on > B->second.depends_on) {
    return 1;
  }
  if (A->second.depends_on < B->second.depends_on) {
    return -1;
  }
  if (A->second.depended_on < B->second.depended_on) {
    return 1;
  }
  if (A->second.depended_on > B->second.depended_on) {
    return -1;
  }
  return 0;
}

bool analyze_packages(Host* host, Map_string_Package* packages) {
  bool success = true;
  Vector_string queue;
  Vector_string__init(&queue);
  for (__auto_type it = Map_string_Package__begin(packages);
      it != Map_string_Package__end(packages);
      ++it) {
    Vector_string__push_back(&queue, it->first);
  }

  const char* package_id;
  while (!Vector_string__empty(&queue) && success) {
    package_id = Vector_string__back(&queue);
    Vector_string__pop_back(&queue);
    printf("package_id: %s\n", package_id);
    success &= analyze_package(host, packages, &queue, package_id);
  }

  Vector_string__clean(&queue);

  qsort(Map_string_Package__begin(packages), Map_string_Package__size(packages), sizeof(Pair_string_Package), compare_Pair_string_Package);
  return success;
}
