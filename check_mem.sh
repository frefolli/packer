#!/bin/bash

clear
valgrind ./builddir/packer.exe $(find repository/ -type f | grep "[a-zA-Z-]\+/[-a-zA-Z]\+$" -o)
