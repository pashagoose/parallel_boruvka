#include <gtest/gtest.h>
#include <boruvka_lib/boruvka.h>
#include <random_graphs_lib/random_graphs.h>
#include "KruskalCheck.h"

using namespace BoruvkaMSTAlgorithm;
using namespace RandomGraphs;

bool CheckTree(vector<Edge> edges, size_t n) {
	// assuming that my parallel dsu definetely works for case of 1 thread :D
	// if not - please fix my brain
	ParallelDsu dsu(n);
	for (const auto& edge: edges) {
		dsu.Unite(edge.From, edge.To);
	}
	return dsu.GetComponentsQuantity() == 1;
}

void CheckGoodAnswer(const vector<Edge>& edges, size_t vertices, size_t Workers) {
	Boruvka solution(edges, vertices, Workers);
	Kruskal solutionCorrect(edges, vertices);
	// assuming that version for 1 thread works properly
	ASSERT_EQ(solutionCorrect.CalcMST(), solution.CalcMST());
	ASSERT_TRUE(CheckTree(solution.GetBuiltMST(), vertices));
}


TEST(BoruvkaCorrectness, TreeOneWorker) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 1;
	Generator gen(42);
	
	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = gen.RandRange(1, 10000);
		auto edges = gen.GenerateTree(vertices);
		CheckGoodAnswer(edges, vertices, Workers);
	}
}


TEST(BoruvkaCorrectness, GraphOneWorker) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 1;
	Generator gen(42);

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = gen.RandRange(1, 100000);
		auto edges = gen.GenerateLinkedGraph(vertices, true);
		CheckGoodAnswer(edges, vertices, Workers);
	}
}

TEST(BoruvkaCorrectness, TreeMultipleWorkers) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 8;
	Generator gen(42);

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = gen.RandRange(1, 10000);
		auto edges = gen.GenerateTree(vertices);
		CheckGoodAnswer(edges, vertices, Workers);
	}
}


TEST(BoruvkaCorrectness, SparseGraphMultipleWorkers) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 8;
	Generator gen(42);

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = gen.RandRange(1, 100000);
		auto edges = gen.GenerateLinkedGraph(vertices, true);
		CheckGoodAnswer(edges, vertices, Workers);
	}
}


TEST(BoruvkaCorrectness, DenseGraphMultipleWorkers) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 8;
	Generator gen(42);

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = gen.RandRange(1, 100000);
		auto edges = gen.GenerateLinkedGraph(vertices, false);
		CheckGoodAnswer(edges, vertices, Workers);
	}
}

