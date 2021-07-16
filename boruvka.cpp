#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include "mpsc_queue.h"
#include "parallel_dsu.h"

using std::vector;
using std::pair;


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
};


class Boruvka {
public:

	Boruvka() = default;

	Boruvka(const vector<vector<pair<size_t, int64_t>>> graph) :
			Graph_(graph),
			Vertices_(graph.size())
	{
		for (size_t u = 0; u < Vertices_; ++u) {
			for (auto [v, cost] : Graph_[u]) {
				if (v > u) {
					Edges_.emplace_back(u, v, cost);
				}
			}
		}
	}

	Boruvka(const vector<Edge>& edges, size_t n) :
		Edges_(edges),
		Vertices_(n),
		Graph_(n); 
	{
		for (const auto& e : Edges_) {
			g[e.From].emplace_back(e.To, e.Cost);
			g[e.To].emplace_back(e.From, e.Cost);
		}
	}

	void CalcMST() {
		
	}

private:

	size_t Vertices = 0;
	vector<vector<pair<size_t, int64_t>>> Graph;
	vector<Edge> Edges;
	vector<Edge> MST;
};
