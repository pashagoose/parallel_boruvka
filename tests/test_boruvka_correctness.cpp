#include <gtest/gtest.h>
#include <boruvka_lib/boruvka.h>

#include <random>

using namespace BoruvkaMSTAlgorithm;

const int MAX_RAND_NUM = 2e9;

std::mt19937 rnd(42);

int RandRange(int l = 0, int r = MAX_RAND_NUM) {
	return l + (rnd() % (r - l + 1));
}

bool CheckTree(vector<Edge> edges, size_t n) {
	// assuming that my parallel dsu definetely works for case of 1 thread :D
	// if not - please fix my brain
	ParallelDsu dsu(n);
	for (const auto& edge: edges) {
		dsu.Unite(edge.From, edge.To);
	}
	return dsu.GetComponentsQuantity() == 1;
}

vector<Edge> GenerateTree(size_t n) {
	vector<Edge> result;
	for (size_t i = 1; i < n; ++i) {
		result.push_back(Edge(i, RandRange(0, i - 1), RandRange()));
	}
	return result;
}


int64_t TotalCost(const vector<Edge>& spanningTree) {
	int64_t result = 0;
	for (const auto& edge : spanningTree) {
		result += edge.Cost;
	}
	return result;
}

Edge GenerateRandomEdge(size_t n) {
	for (;;) {
		size_t u = RandRange(0, n - 1);
		size_t v = RandRange(0, n - 1);
		int64_t cost = RandRange();
		if (u == v) continue;
		return Edge(u, v, cost);
	}
} 

vector<Edge> GenerateLinkedGraph(size_t n, bool sparse = true) {
	vector<Edge> result = GenerateTree(n);

	constexpr size_t MAX_ITERATIONS = 1e7;
	size_t counter = 1;

	while (counter++ && counter < MAX_ITERATIONS) {		
		result.push_back(GenerateRandomEdge(n));
		if (sparse) {
			if (RandRange(1, 2 * n) == 1) {
				break;
			}
		} else {
			if (RandRange(1, 100 * n) == 1) { 
				// yep, the best way to generate dense graph
				break;
			} 
		}
	}
	return result;
}


TEST(BoruvkaCorrectness, TreeOneWorker) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 1;
	
	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = RandRange(1, 10000);
		auto edges = GenerateTree(vertices);
		Boruvka solution(edges, vertices, Workers);
		auto cost = solution.CalcMST();
		ASSERT_EQ(cost, TotalCost(edges));
		auto MST = solution.GetBuiltMST();
		ASSERT_TRUE(CheckTree(MST, vertices));
	}
}


TEST(BoruvkaCorrectness, GraphOneWorker) {
	constexpr size_t Repetitions = 10;
	constexpr size_t Workers = 1;

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = RandRange(1, 100000);
		auto edges = GenerateLinkedGraph(vertices, true);
		Boruvka solution(edges, vertices, Workers);
		solution.CalcMST();
		auto MST = solution.GetBuiltMST();
		ASSERT_TRUE(CheckTree(MST, vertices));
	}
}

void CheckGoodAnswerForMultipleWorkers(const vector<Edge>& edges, size_t vertices) {
	constexpr size_t Workers = 8;

	Boruvka solutionCorrect(edges, vertices, 1);
	Boruvka solution(edges, vertices, Workers);
	// assuming that version for 1 thread works properly
	ASSERT_EQ(solutionCorrect.CalcMST(), solution.CalcMST());
	ASSERT_TRUE(CheckTree(solution.GetBuiltMST(), vertices));
}

TEST(BoruvkaCorrectness, TreeMultipleWorkers) {
	constexpr size_t Repetitions = 10;

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = RandRange(1, 10000);
		auto edges = GenerateTree(vertices);
		CheckGoodAnswerForMultipleWorkers(edges, vertices);
	}
}


TEST(BoruvkaCorrectness, SparseGraphMultipleWorkers) {
	constexpr size_t Repetitions = 10;

	for (size_t i = 0; i < Repetitions; ++i) {
		size_t vertices = RandRange(1, 100000);
		auto edges = GenerateLinkedGraph(vertices, true);
		CheckGoodAnswerForMultipleWorkers(edges, vertices);
	}
}

