#include "random_graphs.h"

namespace RandomGraphs {

void Generator::SetSeed(size_t number) {
	rnd.seed(number);
}

vector<Edge> Generator::GenerateTree(size_t n) {
	vector<Edge> result;
	for (size_t i = 1; i < n; ++i) {
		result.push_back(Edge(i, RandRange(0, i - 1), RandRange()));
	}
	return result;
}

Edge Generator::GenerateRandomEdge(size_t n) {
	for (;;) {
		size_t u = RandRange(0, n - 1);
		size_t v = RandRange(0, n - 1);
		int64_t cost = RandRange();
		if (u == v) continue;
		return Edge(u, v, cost);
	}
} 

vector<Edge> Generator::GenerateLinkedGraph(size_t n, bool sparse = true) {
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
	
}