# Parallel Boruvka

Параллельная версия алгоритма Борувки. По результатам моего тестирования примерно в 2 раза быстрее на 4 ядрах.

# Сборка проекта и запуск тестов
## Сборка
```
$ cd parallel_boruvka
$ mkdir build
$ cd build
$ cmake ..
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
