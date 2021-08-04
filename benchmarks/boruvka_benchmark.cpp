#include <benchmark/benchmark.h>
#include <boruvka_lib/boruvka.h>
#include <random_graphs_lib/random_graphs.h>

using namespace BoruvkaMSTAlgorithm;
using namespace RandomGraphs;

constexpr size_t GRAPHS_NUMBER = 10;

vector<Edge> Graphs[GRAPHS_NUMBER];
size_t Vertices[GRAPHS_NUMBER];

void FillGraphs() {
	Generator gen(42);
	for (size_t i = 0; i < GRAPHS_NUMBER; ++i) {
		Vertices[i] = gen.RandRange(1, 10000);
		bool sparse = (i < 8);
		
		Graphs[i] = gen.GenerateLinkedGraph(Vertices[i], sparse);
	}
}

bool Run(size_t n_threads) {
    for (size_t i = 0; i < GRAPHS_NUMBER; ++i) {
    	Boruvka solution(Graphs[i], Vertices[i], n_threads);
    	solution.CalcMST();
    }
    return true;
}

static void BM_ParallelBoruvka(benchmark::State& state) {
    uint64_t value = state.range(0);
    FillGraphs();
    for (auto _ : state) {
        benchmark::DoNotOptimize(Run(value));
    }
}

BENCHMARK(BM_ParallelBoruvka)
        ->Args({1})
        ->Args({2})
        ->Args({4})
        ->Args({8})
        ->Args({16})
        ->Args({32});

BENCHMARK_MAIN();