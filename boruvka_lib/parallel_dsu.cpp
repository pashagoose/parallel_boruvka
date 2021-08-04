#include "parallel_dsu.h"

using std::vector;

namespace BoruvkaMSTAlgorithm {

ParallelDsu::ParallelDsu(size_t n = 0) : 
	InitialComponents_(n),
	CurrentComponents_(n),
	Parent_(n)
{
	this->ToIdPermutation(n);
}

size_t ParallelDsu::FindLeader(size_t v) {
	while (v != Parent_[v].load()) {
        uint64_t papa = Parent_[v].load();
        uint64_t grandPapa = Parent_[papa].load();
        Parent_[v].compare_exchange_weak(papa, grandPapa);
        v = grandPapa;
    }
    return v;
}

bool ParallelDsu::Unite(size_t u, size_t v) {
	for (;;) {
		size_t uParent = FindLeader(u);
		size_t vParent = FindLeader(v);
		if (uParent == vParent) {
			return false;
		}
		if (!Parent_[uParent].compare_exchange_weak(uParent, vParent)) {
			continue;
		}
		if (uParent != vParent) {
			CurrentComponents_.fetch_sub(1);
			return true;
		}
		return false;
	}
}

bool ParallelDsu::SameComponent(size_t u, size_t v) {
	u = FindLeader(u);
	v = FindLeader(v);
	// works only in this partial case (boruvka), at this stage we cannot unite components
	return u == v;
}

size_t ParallelDsu::GetComponentsQuantity() const {
	return CurrentComponents_.load();
}


void ParallelDsu::ToIdPermutation(size_t n) {
	InitialComponents_ = n;
	CurrentComponents_.store(n);
	for (size_t i = 0; i < InitialComponents_; ++i) {
		Parent_[i].store(i);
	}
}

} // namespace BoruvkaMSTAlgorithm