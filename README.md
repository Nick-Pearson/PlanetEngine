# Planet Engine
[![MSBuild](https://github.com/Nick-Pearson/PlanetEngine/actions/workflows/build.yml/badge.svg)](https://github.com/Nick-Pearson/PlanetEngine/actions/workflows/build.yml)
## Compiling a development build

Run these commands in the project root
```bash
conan install . --install-folder build -s build_type=Debug --build=missing
conan build . --build-folder build
```

## Compiling a release build
```bash
conan install . --install-folder build --build=missing
conan build . --build-folder build
```