#include "boruvka.h"

namespace BoruvkaMSTAlgorithm {

bool operator==(const Edge& a, const Edge& b) {
	return (a.From == b.From && a.To == b.To && a.Cost == b.Cost);
}

std::ostream& operator<<(std::ostream& out, const Edge& edge) {
	out << "Edge from: " << edge.From << " to: " << edge.To << ", Cost: " << edge.Cost;
	return out;
}


Boruvka::Boruvka(const vector<Edge>& edges, size_t n, size_t workers) :
	Workers_(workers),
	Vertices_(n),
	Graph_(n),
	Edges_(edges),
	chippestEdgeOut(n),
	Dsu_(n)
{
	for (const auto& e : Edges_) {
		Graph_[e.From].emplace_back(e.To, e.Cost);
		Graph_[e.To].emplace_back(e.From, e.Cost);
	}
	ClearChippestEdgeInfo(0, Vertices_);
}


int64_t Boruvka::CalcMST() {
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

vector<Edge> Boruvka::GetBuiltMST() const {
	return MST_.copy();
}


bool Boruvka::SetMinEdge(size_t index, size_t vertex) {
	for (;;) {
		auto currentMinEdgeId = chippestEdgeOut[vertex].load();
		if (currentMinEdgeId != Edges_.size() && Edges_[currentMinEdgeId] < Edges_[index]) {
			return false;
		}
		if (chippestEdgeOut[vertex].compare_exchange_weak(currentMinEdgeId, index)) {
			return true;
		}
	}
}

void Boruvka::UpdateChippestEdges(size_t l, size_t r) {
	for (size_t i = l; i < r; ++i) {
		if (!Dsu_.SameComponent(Edges_[i].From, Edges_[i].To)) {
			SetMinEdge(i, Dsu_.FindLeader(Edges_[i].From));
			SetMinEdge(i, Dsu_.FindLeader(Edges_[i].To));
		}
	}
}

void Boruvka::Unite(const Edge& edge) {
	if (Dsu_.Unite(edge.From, edge.To)) {
		CostMST_.fetch_add(edge.Cost);
		MST_.PushBack(edge);
	}
}

void Boruvka::FindGoodEdges(size_t l, size_t r, vector<size_t>& goodEdges) {
	for (size_t i = l; i < r; ++i) {
		size_t fromParent = Dsu_.FindLeader(Edges_[i].From);
		size_t toParent = Dsu_.FindLeader(Edges_[i].To);
		if (chippestEdgeOut[fromParent].load() == i) {
			goodEdges.push_back(i);
			continue;
		}
		if (chippestEdgeOut[toParent].load() == i) {
			goodEdges.push_back(i);
		}
	}
}

void Boruvka::ClearChippestEdgeInfo(size_t l, size_t r) {
	for (size_t i = l; i < r; ++i) {
		chippestEdgeOut[i].store(Edges_.size());
	}
}

void Boruvka::DoWork(size_t lEdges, 
	size_t rEdges, 
	size_t lVertices, 
	size_t rVertices, 
	Latch& synchronize_unite,
	Latch& synchronize_cleaning) 
{
	UpdateChippestEdges(lEdges, rEdges);
	synchronize_unite.ArriveAndWait();

	vector<size_t> goodEdges;
	FindGoodEdges(lEdges, rEdges, goodEdges);
	synchronize_cleaning.ArriveAndWait();

	// merge components
	while (!goodEdges.empty()) {
		Unite(Edges_[goodEdges.back()]);
		goodEdges.pop_back();
	}

	// clear information about chippest edges from each component
	ClearChippestEdgeInfo(lVertices, rVertices);
}

void Boruvka::BoruvkaIteration() {
	vector<thread> threads;
	Latch synchronize_unite(Workers_);
	Latch synchronize_cleaning(Workers_);
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
		  	std::ref(synchronize_unite),
		  	std::ref(synchronize_cleaning)
		);
	}
	for (auto& thread : threads) {
		thread.join();
	}
}

} // namespace BoruvkaMSTAlgorithm