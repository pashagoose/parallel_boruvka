# Parallel Boruvka

Parallel version of Boruvka's algorithm for finding MST. Spead up original algorithm by 10% using 4 physical threads. Benchmarks were measured on 8 sparse graphs and 2 tense ones.

# Build project and run tests.
## Build Debug (Release)
```
$ cd parallel_boruvka
$ mkdir build
$ cd build
$ cmake [-DCMAKE_BUILD_TYPE=Release] ..
$ make
```

## Run tests
Из директории `build`:
```
$ ./test/bin/all_tests
```

## Run benchmarks
Из директории `build`:
```
$ ./benchmarks/bin/boruvka_benchmark
```
