#pragma once

#include <boruvka_lib/boruvka.h>

#include <random>

namespace RandomGraphs {

using BoruvkaMSTAlgorithm::Edge;
using std::vector;

class Generator {
public:
	constexpr static inline int MAX_RAND_NUM = 2e9;

	Generator(size_t seed) : rnd(seed) {}

	int RandRange(int l = 0, int r = MAX_RAND_NUM) {
		return l + (rnd() % (r - l + 1));
	}

	void SetSeed(size_t number);
	vector<Edge> GenerateTree(size_t n);
	Edge GenerateRandomEdge(size_t n);
	vector<Edge> GenerateLinkedGraph(size_t n, bool sparse);

private:
	std::mt19937 rnd;
};

}