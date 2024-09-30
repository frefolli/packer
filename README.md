# Packer

Yet another makepkg/rpmbuild wrapper

# Usage

In orger to properly work, Packer needs a Packerfile for each repo for which you need to craft a package. Those Packerfiles are essentially yaml files which describe properties about that repo/package. As an example:

```yaml
version: 1.0.0
license: MIT
summary: Tree Sitter Parser for C
depends:
  arch: tree-sitter tree-sitter-cli
  fedora: tree-sitter tree-sitter-cli
```
Packer will attempt to download a `Packerfile` from the repo if it exists, archive it and use it.

`depends` and `makedepends` fields list packages which are required prior to the installation or prior to the building of that package. Since these are references to specific linux distro packages, it's required to fill it for you distro.
At runtime you select (-b) the build system  and (-d) the distro you are using. Using (-r) flag will cause Packer to discard already cached config/sources and packages already crafted.

```bash
python -m main frefolli/lartc -d fedora -b rpmbuild -r
```

# As for now

For now, `makedepends` isn't used and only `rpmbuild` and `makepkg` are supported.
