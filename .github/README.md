`build.manifest` file is used to make sure that the default pipeline produces a consistent build tree. Generated with:
```sh
mkdir <build-dir>; cd <build-dir>
cmake .. -G "Unix Makefiles" -DENABLE_SWIG=ON -DPYTHON_VERSION=3.7
cmake --build . -j
cmake --install . --prefix ./install
cd install
tree -if --noreport --sort=name  > ../../.github/install.manifest
```
