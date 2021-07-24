#pragma once

#include <atomic>
#include <vector>

using std::vector;

// Lock-Free DSU
// only path-compression heuristics, I believe rank heuristics
// does not help much in Boruvka.

class ParallelDsu {
public:
	ParallelDsu(size_t n = 0) : 
		InitialComponents_(n),
		CurrentComponents_(n),
		Parent_(n)
	{
		this->ToIdPermutation(n);
	}

	size_t FindLeader(size_t v) {
		auto currentParent = Parent_[v].load();
		if (currentParent == v) {
			return v;
		}
		auto res = FindLeader(currentParent);
		Parent_[v].store(res);
		return res;
	}

	bool Unite(size_t u, size_t v) {
		size_t uParent_ = FindLeader(u);
		size_t vParent_ = FindLeader(v);
		if (uParent_ == vParent_) {
			return false;
		}

		for (;;) {
			if (Parent_[uParent_].compare_exchange_weak(uParent_, vParent_)) {
				if (uParent_ != vParent_) {
					CurrentComponents_.fetch_sub(1);
					return true;
				}
				return false;
			}
			std::this_thread::yield();
			uParent_ = FindLeader(uParent_);
		}	
	}

	bool SameComponent(size_t u, size_t v) {
		u = FindLeader(u);
		v = FindLeader(v);
		// works only in this partial case (boruvka), at this stage we cannot unite components
		return u == v;
	}

	size_t GetComponentsQuantity() const {
		return CurrentComponents_.load();
	}

private:
	size_t InitialComponents_ = 0;
	std::atomic<size_t> CurrentComponents_{0};
	vector<std::atomic<size_t>> Parent_;

	void ToIdPermutation(size_t n) {
		InitialComponents_ = n;
		CurrentComponents_.store(n);
		for (size_t i = 0; i < InitialComponents_; ++i) {
			Parent_[i].store(i);
		}
	}
};