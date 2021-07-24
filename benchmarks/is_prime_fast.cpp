#include <benchmark/benchmark.h>
#include <is_prime_lib/is_prime.h>

static void BM_IsPrimeFast(benchmark::State& state) {
  uint64_t value = state.range(0);
  for (auto _ : state) {
    benchmark::DoNotOptimize(IsPrime(value));
  }
}
// Register the function as a benchmark
BENCHMARK(BM_IsPrimeFast)
  ->Args({2})
  ->Args({10})
  ->Args({65537 * 65537})
  ->Args({1ull << 30})
  ->Args({1000000009});

// Run the benchmark
BENCHMARK_MAIN();

