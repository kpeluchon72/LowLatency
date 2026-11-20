# LowLatency

A C++17 benchmark laboratory for European Black-Scholes option pricing.

## Build and run

Run these commands from the repository root in PowerShell.

### Configure the build

This is required the first time you build the project:

```powershell
cmake -S src -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

### Build

```powershell
cmake --build build --parallel
```

### Run the tests

Build first, then run CTest:

```powershell
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Run the benchmark executable

Build first, then run:

```powershell
cmake --build build --parallel
.\build\benchmark.exe
```

### Build, test, and run

```powershell
cmake --build build --parallel
ctest --test-dir build --output-on-failure
.\build\benchmark.exe
```
