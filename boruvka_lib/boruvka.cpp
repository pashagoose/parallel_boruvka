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


void Boruvka::SetMinEdge(size_t index, size_t vertex) {
	for (;;) {
		auto currentMinEdgeId = chippestEdgeOut[vertex].load();
		if (currentMinEdgeId != Edges_.size() && Edges_[currentMinEdgeId] < Edges_[index]) {
			break;
		}
		if (chippestEdgeOut[vertex].compare_exchange_weak(currentMinEdgeId, index)) {
			break;
		}
		std::this_thread::yield();
	}
}

void Boruvka::FindChippestEdges(size_t l, size_t r) {
	for (size_t i = l; i < r; ++i) {
		if (!Dsu_.SameComponent(Edges_[i].From, Edges_[i].To)) {
			SetMinEdge(i, Edges_[i].From);
			SetMinEdge(i, Edges_[i].To);
		}
	}
}

void Boruvka::Unite(const Edge& edge) {
	if (Dsu_.Unite(edge.From, edge.To)) {
		CostMST_.fetch_add(edge.Cost);
		MST_.PushBack(edge);
	}
}

void Boruvka::MergeComponents(size_t l, size_t r) {
	for (size_t i = l; i < r; ++i) {
		if (chippestEdgeOut[Edges_[i].From].load() == i) {
			Unite(Edges_[i]);
			continue;
		}
		if (chippestEdgeOut[Edges_[i].To].load() == i) {
			Unite(Edges_[i]);
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
	Latch& synchronize_unite) 
{
	FindChippestEdges(lEdges, rEdges);
	synchronize_unite.ArriveAndWait();
	MergeComponents(lEdges, rEdges);
	ClearChippestEdgeInfo(lVertices, rVertices);
}

void Boruvka::BoruvkaIteration() {
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

} // namespace BoruvkaMSTAlgorithm