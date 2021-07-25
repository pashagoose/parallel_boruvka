#pragma once

#include <boruvka_lib/parallel_dsu.h>

class Kruskal {
public:
	Kruskal(const std::vector<BoruvkaMSTAlgorithm::Edge>& edges, size_t vertices) 
		: Vertices_(vertices)
		, Dsu_(vertices)
		, Edges_(edges)
	{
		std::sort(Edges_.begin(), Edges_.end());
	}

	int64_t CalcMST() {
		for (const auto& edge : Edges_) {
			if (Dsu_.Unite(edge.From, edge.To)) {
				Cost_ += edge.Cost;
			}
		}
		return Cost_;
	}

private:
	size_t Vertices_;
	BoruvkaMSTAlgorithm::ParallelDsu Dsu_;
	std::vector<BoruvkaMSTAlgorithm::Edge> Edges_;
	int64_t Cost_ = 0;
};