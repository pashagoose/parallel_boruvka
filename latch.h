#include <atomic>
#include <thread>

class Latch {
public:
	
	Latch(size_t n_threads)
		: Counter_(n_threads)
	{
	}

	void ArriveAndWait() {
		Counter_.fetch_sub(1);
		for (;;) {
			if (Counter_.load() == 0) {
				break;
			}
			std::this_thread::yield();
		}
	}

private:
	std::atomic<size_t> Counter_{0};
};