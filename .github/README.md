`build.manifest` file is used to make sure that the default pipeline produces a consistent build tree. Generated with:
```sh
mkdir <build-dir>; cd <build-dir>
cmake .. -G "Unix Makefiles" -DENABLE_SWIG=ON -DPYTHON_VERSION=3.7 -DCMAKE_INSTALL_PREFIX=install
cmake --build . -j
tree -if --noreport | sort > ../.github/build.manifest
```
