#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>  
#include <thread>
#include <utility>
#include <vector>

#include "latch.h"
#include "parallel_dsu.h"
#include "thread_safe_vector.h"


namespace BoruvkaMSTAlgorithm {

using std::atomic;
using std::vector;
using std::pair;
using std::thread;

constexpr static inline int64_t COST_MAX = std::numeric_limits<int64_t>::max();


struct Edge {
	Edge() = default;

	Edge(const Edge& other) = default;

	Edge(size_t from, size_t to, int64_t cost) :
		From(from),
		To(to),
		Cost(cost)
	{}

	size_t From = 0;
	size_t To = 0;
	int64_t Cost = COST_MAX;

	bool operator<(const Edge& other) const {
		return Cost < other.Cost;
	}
};

bool operator==(const Edge& a, const Edge& b);

std::ostream& operator<<(std::ostream& out, const Edge& edge);


class Boruvka {
public:

	Boruvka() = default;

	Boruvka(const vector<Edge>& edges, size_t n, size_t workers);


	int64_t CalcMST();

	vector<Edge> GetBuiltMST() const;

private:

	void SetMinEdge(size_t index, size_t vertex);

	void FindChippestEdges(size_t l, size_t r);

	void Unite(const Edge& edge);

	void MergeComponents(size_t l, size_t r);

	void ClearChippestEdgeInfo(size_t l, size_t r);

	void DoWork(size_t lEdges, 
		size_t rEdges, 
		size_t lVertices, 
		size_t rVertices, 
		Latch& synchronize_unite);

	void BoruvkaIteration();

	size_t Workers_ = 1;
	size_t Vertices_ = 0;
	vector<vector<pair<size_t, int64_t>>> Graph_;
	vector<Edge> Edges_;
	vector<atomic<size_t>> chippestEdgeOut;
	ThreadSafeVector<Edge> MST_;
	ParallelDsu Dsu_;  
	std::atomic<int64_t> CostMST_ = 0;
}; // class Boruvka

} // namespace BoruvkaMSTAlgorithm

