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

bool operator==(const Edge& a, const Edge& b) {
	return (a.From == b.From && a.To == b.To && a.Cost == b.Cost);
}

std::ostream& operator<<(std::ostream& out, const Edge& edge) {
	out << "Edge from: " << edge.From << " to: " << edge.To << ", Cost: " << edge.Cost;
	return out;
}


class Boruvka {
public:

	Boruvka() = default;

	Boruvka(const vector<vector<pair<size_t, int64_t>>> graph, size_t workers) :
		Workers_(workers),
		Vertices_(graph.size()),
		Graph_(graph),
		chippestOut(graph.size()),
		Dsu_(graph.size())
	{
		for (size_t u = 0; u < Vertices_; ++u) {
			for (auto [v, cost] : Graph_[u]) {
				if (v > u) {
					Edges_.emplace_back(u, v, cost);
				}
			}
		}
		ClearChippestEdgeInfo(0, Vertices_);
	}

	Boruvka(const vector<Edge>& edges, size_t n, size_t workers) :
		Workers_(workers),
		Vertices_(n),
		Graph_(n),
		Edges_(edges),
		chippestOut(n),
		Dsu_(n)
	{
		for (const auto& e : Edges_) {
			Graph_[e.From].emplace_back(e.To, e.Cost);
			Graph_[e.To].emplace_back(e.From, e.Cost);
		}
		ClearChippestEdgeInfo(0, Vertices_);
	}


	int64_t CalcMST() {
		while (true) {
			size_t componentsBeforeIteration = Dsu_.GetComponentsQuantity();
			BoruvkaIteration();
			size_t componentsAfterIteration = Dsu_.GetComponentsQuantity();

			if (componentsAfterIteration == componentsBeforeIteration) {
				// can not adjust MST_
				break;
			}
		}
		return CostMST_.load();
	}

	vector<Edge> GetBuiltMST() const {
		return MST_.copy();
	}

private:

	void SetMinEdge(const Edge& edge, size_t vertex) {
		for (;;) {
			auto currentCost = chippestOut[vertex].load();
			if (currentCost < edge.Cost) {
				break;
			}
			if (chippestOut[vertex].compare_exchange_weak(currentCost, edge.Cost)) {
				break;
			}
			std::this_thread::yield();
		}
	}

	void FindChippestEdges(size_t l, size_t r) {
		for (size_t i = l; i < r; ++i) {
			if (!Dsu_.SameComponent(Edges_[i].From, Edges_[i].To)) {
				SetMinEdge(Edges_[i], Edges_[i].From);
				SetMinEdge(Edges_[i], Edges_[i].To);
			}
		}
	}

	void Unite(const Edge& edge) {
		if (Dsu_.Unite(edge.From, edge.To)) {
			CostMST_.fetch_add(edge.Cost);
			MST_.PushBack(edge);
		}
	}

	void MergeComponents(size_t l, size_t r) {
		for (size_t i = l; i < r; ++i) {
			if (chippestOut[Edges_[i].From].load() == Edges_[i].Cost) {
				Unite(Edges_[i]);
				continue;
			}
			if (chippestOut[Edges_[i].To].load() == Edges_[i].Cost) {
				Unite(Edges_[i]);
			}
		}
	}

	void ClearChippestEdgeInfo(size_t l, size_t r) {
		for (size_t i = l; i < r; ++i) {
			chippestOut[i].store(COST_MAX);
		}
	}

	void DoWork(size_t lEdges, 
		size_t rEdges, 
		size_t lVertices, 
		size_t rVertices, 
		Latch& synchronize_unite) 
	{
		FindChippestEdges(lEdges, rEdges);
		synchronize_unite.ArriveAndWait();
		MergeComponents(lEdges, rEdges);
		ClearChippestEdgeInfo(lVertices, rVertices);
	}

	void BoruvkaIteration() {
		vector<thread> threads;
		Latch synchronize_unite(Workers_);
		size_t blockLenEdges = (Edges_.size() + Workers_ - 1) / Workers_;
		size_t blockLenVertices = (Vertices_ + Workers_ - 1) / Workers_;
		for (size_t i = 0; i < Workers_; ++i) {
			threads.emplace_back(
				&BoruvkaMSTAlgorithm::Boruvka::DoWork,
				this,
			 	i * blockLenEdges,
			 	std::min((i + 1) * blockLenEdges, Edges_.size()),
			 	i * blockLenVertices,
			 	std::min((i + 1) * blockLenVertices, Vertices_),
			  	std::ref(synchronize_unite)
			);
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}

	size_t Workers_ = 1;
	size_t Vertices_ = 0;
	vector<vector<pair<size_t, int64_t>>> Graph_;
	vector<Edge> Edges_;
	vector<atomic<int64_t>> chippestOut;
	ThreadSafeVector<Edge> MST_;
	ParallelDsu Dsu_;
	std::atomic<int64_t> CostMST_ = 0;
}; // class Boruvka

}; // namespace BoruvkaMSTAlgorithm


void ManualTest() {
	using namespace std;
	using namespace BoruvkaMSTAlgorithm;
	const vector<Edge> testEdges = {Edge(0, 1, 1), Edge(1, 2, 1), Edge(2, 3, 1)};
	Boruvka solveForSimpleGraph(testEdges, 4, 1);
	auto cost = solveForSimpleGraph.CalcMST();
	cout << "Cost: " << cost << endl;
	auto mst = solveForSimpleGraph.GetBuiltMST();
	cout << "Found MST:\n";
	for (auto edge : mst) {
		cout << edge << '\n';
	}
}

int main(int /*argc*/, const char* argv[]) {
	if (argv[1] == std::string("-m")) { // manual mode
		ManualTest();
	}
	return 0;
}
