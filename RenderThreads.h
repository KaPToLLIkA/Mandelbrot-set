#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include "SetDescriptor.h"


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
	template <class T>
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
	template <class T>
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

template<class T>
inline RenderThreads::RenderThreads(
	size_t number_of_threads,
	T* obj,
	void(*thr_funct)(T * obj,
		std::mutex * pauser,
		std::mutex * work_locker,
		std::mutex * wait_restart,
		std::condition_variable * alert,
		bool * notified,
		bool * exit,
		bool * stop_render)) :
	number_of_threads(number_of_threads),
	wait_restart(new std::mutex),
	alert(new std::condition_variable),
	notified(std::vector <bool>(number_of_threads, false))
{

	for (size_t i = 0; i < number_of_threads; ++i)
	{
		work_locker_target_thread.push_back(new std::mutex);
		pauser_target_thread.push_back(new std::mutex);




		/*threads.push_back(new std::thread((*thr_funct),
			obj,
			pauser_target_thread[i],
			work_locker_target_thread[i],
			wait_restart,
			alert,
			&notified[i],
			&end_flag,
			&stop_render));
		threads[i]->detach();*/
	}
}

template<class T>
inline void RenderThreads::CreateThreads(
	size_t number_of_threads,
	T * obj,
	void(*thr_funct)(T * obj,
		std::mutex * pauser,
		std::mutex * work_locker,
		std::mutex * wait_restart,
		std::condition_variable * alert,
		bool * notified,
		bool * exit,
		bool * stop_render))
{
	this->number_of_threads = number_of_threads;
	wait_restart = new std::mutex;
	alert = new std::condition_variable;
	notified = std::vector <bool>(number_of_threads, false);

	for (size_t i = 0; i < number_of_threads; ++i)
	{
		work_locker_target_thread.push_back(new std::mutex);
		pauser_target_thread.push_back(new std::mutex);




		/*threads.push_back(new std::thread((*thr_funct),
			obj,
			pauser_target_thread[i],
			work_locker_target_thread[i],
			wait_restart,
			alert,
			&notified[i],
			&end_flag,
			&stop_render));
		threads[i]->detach();*/
	}
}