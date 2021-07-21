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
		InitialComponents(n),
		CurrentComponents(n),
		Parent(n)
	{
		this->ToIdPermutation(n);
	}

	size_t FindLeader(size_t v) {
		auto currentParent = Parent[v].load();
		if (currentParent == v) {
			return v;
		}
		auto res = FindLeader(v);
		Parent[v].store(res);
		return res;
	}

	bool Unite(size_t u, size_t v) {
		size_t uParent = FindLeader(u);
		size_t vParent = FindLeader(v);
		if (uParent == vParent) {
			return false;
		}

		for (;;) {
			if (Parent[uParent].compare_exchange_weak(uParent, vParent)) {
				if (uParent != vParent) {
					CurrentComponents.fetch_sub(1);
					return true;
				}
				return false;
			}
			std::this_thread::yield();
			uParent = FindLeader(uParent);
		}	
	}

	bool SameComponent(size_t u, size_t v) {
		u = FindLeader(u);
		v = FindLeader(v);
		// works only in this partial case (boruvka), at this stage we cannot unite components
		return u == v;
	}

	size_t GetComponentsQuantity() const {
		return CurrentComponents.load();
	}

private:
	size_t InitialComponents = 0;
	std::atomic<size_t> CurrentComponents = 0;
	vector<std::atomic<size_t>> Parent;

	void ToIdPermutation(size_t n) {
		InitialComponents = n;
		CurrentComponents.store(n);
		for (size_t i = 0; i < InitialComponents; ++i) {
			Parent[i].store(i);
		}
	}
};