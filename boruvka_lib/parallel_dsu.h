#pragma once

#include <atomic>
#include <thread>
#include <vector>

// Lock-Free DSU
// only path-compression heuristics, I believe rank heuristics
// does not help much in Boruvka.

namespace BoruvkaMSTAlgorithm {

using std::vector;

class ParallelDsu {
public:
	ParallelDsu(size_t n);

	size_t FindLeader(size_t v);

	bool Unite(size_t u, size_t v);

	bool SameComponent(size_t u, size_t v);

	size_t GetComponentsQuantity() const;
private:
	size_t InitialComponents_ = 0;
	std::atomic<size_t> CurrentComponents_{0};
	vector<std::atomic<size_t>> Parent_;

	void ToIdPermutation(size_t n);
};

} // namespace BoruvkaMSTAlgorithm