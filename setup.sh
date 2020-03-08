#!/bin/bash
if [ -d build ]
then rm -rf build;
fi

cmake -E make_directory build;
cd build;
conan install .. --build missing;
cmake ..;
cmake --build .;
cd ..;
conan export -k . collection/latest@gundoll/stable;
conan export-pkg -f . collection/latest@gundoll/stable;
conan upload -r gundoll collection/latest@gundoll/stable --all --force;
