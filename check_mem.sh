#!/bin/bash

clear
valgrind ./builddir/main.exe \
  frefolli/tree-sitter-lart \
  frefolli/sexpresso \
  frefolli/c-templates \
  frefolli/ts-yaml \
  frefolli/stardock \
  frefolli/lartc \
  tree-sitter/tree-sitter-c \
  tree-sitter-grammars/tree-sitter-yaml
