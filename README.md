# Parallel Boruvka

Параллельная версия алгоритма Борувки. На 10% быстрее на 4 ядрах :D В бенчмарках замерялось время сразу на 10 графах: 8 разреженных и 2 плотных (можно глянуть в коде).

# Сборка проекта и запуск тестов
## Сборка Debug (Release)
```
$ cd parallel_boruvka
$ mkdir build
$ cd build
$ cmake [-DCMAKE_BUILD_TYPE=Release] ..
$ make
```

## Запуск тестов
Из директории `build`:
```
$ ./test/bin/all_tests
```

## Запуск бенчмарков
Из директории `build`:
```
$ ./benchmarks/bin/boruvka_benchmark
```
