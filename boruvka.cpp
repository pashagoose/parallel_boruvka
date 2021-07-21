#include <cstdint>
#include <iostream>
#include <latch>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "thread_safe_vector.h"
#include "parallel_dsu.h"

using std::vector;
using std::pair;
using std::thread;

struct Edge {
	Edge() = delete;

	Edge(size_t from, size_t to, int64_t cost) :
		From(from),
		To(to),
		Cost(cost)
	{}

	size_t From;
	size_t To;
	int64_t Cost;

	bool operator<(const Edge& other) const {
		return Cost < other.Cost;
	}
};


class Boruvka {
public:

	Boruvka() = default;

	Boruvka(const vector<vector<pair<size_t, int64_t>>> graph, size_t workers) :
		Workers(workers),
		Vertices(graph.size()),
		Graph(graph),
		Dsu(graph.size())
	{
		for (size_t u = 0; u < Vertices; ++u) {
			for (auto [v, cost] : Graph[u]) {
				if (v > u) {
					Edges.emplace_back(u, v, cost);
				}
			}
		}
	}

	Boruvka(const vector<Edge>& edges, size_t n, size_t workers) :
		Workers(workers),
		Vertices(n),
		Graph(n),
		Edges(edges),
		Dsu(n)
	{
		for (const auto& e : Edges) {
			Graph[e.From].emplace_back(e.To, e.Cost);
			Graph[e.To].emplace_back(e.From, e.Cost);
		}
	}


	int64_t CalcMST() {
		while (true) {
			size_t componentsBeforeIteration = Dsu.GetComponentsQuantity();
			BoruvkaIteration();
			size_t componentsAfterIteration = Dsu.GetComponentsQuantity();

			if (componentsAfterIteration == componentsBeforeIteration) {
				// can not adjust MST
				break;
			}
		}
		return CostMST.load();
	}

private:

	Edge FindChippestEdge(size_t l, size_t r) {
		Edge bestEdge(0, 0, std::numeric_limits<int64_t>::max());
		for (size_t i = l; i < r; ++i) {
			if (!Dsu.SameComponents(Edges[i].From, Edges[i].To) && Edges[i] < bestEdge) {
				bestEdge = Edges[i];
			}
		}	
		return bestEdge;
	}

	void DoWork(size_t l, size_t r, std::latch& synchronize_unite) {
		bestEdge = FindChippestEdge(l, r);
		synchronize_unite.arrive_and_wait();
		if (Dsu.Unite(bestEdge.From, bestEdge.To)) {
			CostMST.fetch_add(bestEdge.Cost);
			MST.PushBack(bestEdge);
		}
	}

	void BoruvkaIteration() {
		vector<thread> threads;
		std::latch synchronize_unite(Workers);
		size_t blockLen = (Edges.size() + Workers - 1) / Workers;
		for (size_t i = 0; i < Workers; ++i) {
			threads.emplace_back(
				DoWork,
			 	i * blockLen,
			 	min((i + 1) * blockLen, Edges.size()),
			  	std::rref(synchronize_unite)
			 );
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}

	size_t Workers = 1;
	size_t Vertices = 0;
	vector<vector<pair<size_t, int64_t>>> Graph;
	vector<Edge> Edges;
	// TODO: replace thread_safe_vector with mpmc_queue, should be a bit faster 
	// (it is majorized by finding chippest edges)
	ThreadSafeVector<Edge> MST;
	ParallelDsu Dsu;
	std::atomic<int64_t> CostMST = 0;
};

int main() {

	return 0;
}
