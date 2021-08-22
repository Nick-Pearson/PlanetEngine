# Planet Engine
[![MSBuild](https://github.com/Nick-Pearson/PlanetEngine/actions/workflows/build.yml/badge.svg)](https://github.com/Nick-Pearson/PlanetEngine/actions/workflows/build.yml)
## Compiling a development build

Run these commands in the project root
```bash
conan install . --install-folder build -s build_type=Debug
conan build . --build-folder build
```

## Compiling a release build
```bash
conan install . --install-folder build
conan build . --build-folder build
```