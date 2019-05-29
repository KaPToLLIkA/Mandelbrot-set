#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include "SetDescriptor.h"

template <class T>
class RenderThreads
{
	
private:

	bool end_flag = false;
	bool stop_render = false;
	std::vector <std::mutex*> work_locker_target_thread;
	std::vector <std::mutex*> pauser_target_thread;
	std::mutex *wait_restart;
	std::condition_variable *alert;
	std::vector <bool> notified;
	std::vector <std::thread*> threads;

	size_t number_of_threads = 0;
public:
	
	RenderThreads(const RenderThreads&) = delete;
	RenderThreads& operator=(const RenderThreads&) = delete;


	RenderThreads();
	RenderThreads(size_t number_of_threads,
					T* obj,
					void(*thr_funct)(T * obj,
									std::mutex * pauser,
									std::mutex * work_locker,
									std::mutex * wait_restart,
									std::condition_variable * alert,
									bool * notified,
									bool * exit,
									bool * stop_render));
	
	void CreateThreads(size_t number_of_threads,
						T* obj,
						void(*thr_funct)(T * obj,
							std::mutex * pauser,
							std::mutex * work_locker,
							std::mutex * wait_restart,
							std::condition_variable * alert,
							bool * notified,
							bool * exit,
							bool * stop_render));
	void StartAllThreads();
	void StopAllThreads();
	void StartConcreteThread(int idx);
	void StopConcreteThread(int idx);

	void setPauseAllThreads();
	void unsetPauseAllThreads();
	void setPauseConcreteThread(int idx);
	void unsetPauseConcreteThread(int idx);


	void EndThreads();
	

	~RenderThreads();
};

