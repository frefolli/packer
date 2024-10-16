#!/bin/bash

clear
valgrind ./builddir/packer.exe $(find repository -type f)
