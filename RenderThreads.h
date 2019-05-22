#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include "SetDescriptor.h"

template <class T>
class RenderThreads
{
	
private:

	bool end_flag = false;
	std::vector <std::mutex*> lock_target_thread;
	std::vector <std::thread*> threads;

	size_t number_of_threads = 0;
public:
	
	RenderThreads(const RenderThreads&) = delete;
	//RenderThreads& operator=(const RenderThreads&) = delete;

	RenderThreads();
	RenderThreads(size_t number_of_threads,
					T* obj,
					void(*thr_funct)(T* obj,
									std::mutex* locker,
									bool *end_flag));
	void CreateThreads(size_t number_of_threads,
		T* obj,
		void(*thr_funct)(T* obj,
			std::mutex* locker,
			bool *end_flag));
	void StartAllThreads();
	void StopAllThreads();
	void StartConcreteThread(int idx);
	void StopConcreteThread(int idx);
	void EndThreads();
	

	~RenderThreads();
};

