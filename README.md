# daisy_q

daisy_q is a CMake project incorporating [libDaisy](https://github.com/electro-smith/libDaisy) Cycfi [Q library](https://github.com/cycfi/q), allowing the Q library to be used for audio DSP processing on Electro-smith Daisy hardware.

### Requirements

* The Daisy [development environment](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment)
* [CMake](https://cmake.org/download/) >= v3.20

### Setup

Clone the project and all of its submodules:

```
git clone --recurse-submodules git@github.com:recursinging/daisy_q.git
```

Switch into the project directory:

```
cd daisy_q
```

Set up the project using CMake

```
cmake -D TOOLCHAIN_PREFIX=<your-toolchain-location> -DCMAKE_BUILD_TYPE=Debug -D CMAKE_TOOLCHAIN_FILE="lib/libDaisy/cmake/toolchains/stm32h750xx.cmake" -S . -B build_debug -G "Unix Makefiles"
```

When the project is successfully set up, switch into the `build_debug` directory:

```
cd build_debug
```

Now the project can be compiled using make:

```
make
```

If your debugger is already attached, the program can be flashed with:

```
make program
```
