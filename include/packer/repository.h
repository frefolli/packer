#ifndef PACKER_REPOSITORY
#define PACKER_REPOSITORY
#include <packer/package.h>
bool is_package_id(const char* package_id);
char* packerfile_of(const char* package_id);
char* packerdir_of(const char* group);
bool split_package_id(const char* package_id, char** recipient_group, char** recipient_name);
bool download_dockerfile(const char* packerfile, const char* group, const char* name);
bool retrieve_package_into(Package* package, const char* package_id);
Package* retrive_package(const char* package_id);
#endif//PACKER_REPOSITORY
