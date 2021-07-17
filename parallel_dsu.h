#include <atomic>
#include <vector>

using std::vector;

// Lock-Free DSU
// only path-compression heuristics, I believe rank heuristics
// does not help much in Boruvka.

class ParallelDsu {
public:
	ParallelDsu(size_t n) : 
		Components(n),
		Parent(n),
		ToIdPermutation()
	{}

	size_t FindLeader(size_t v) {
		auto currentParent = Parent[v].load();
		if (currentParent == v) {
			return v;
		}
		auto res = FindLeader(v);
		Parent[v].store(res);
		return res;
	}

	void Unite(size_t u, size_t v) {
		size_t uParent = FindLeader(u);
		size_t vParent = FindLeader(v);
		if (uParent == vParent) {
			return;
		}

		for (;;) {
			if (Parent[uParent].compare_exchange_weak(uParent, vParent)) {
				break;
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

private:
	size_t Components = 0;
	vector<std::atomic<>> Parent;

	void ToIdPermutation() {
		for (size_t i = 0; i < Components; ++i) {
			Parent[i].store(i);
		}
	}
};