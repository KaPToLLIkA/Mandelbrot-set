#pragma once
#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include "SetDescriptor.h"

#ifdef DEBUG
#include <iostream>
#endif // DEBUG


class MandelSet;

class RenderThreads
{
	
private:
#ifdef DEBUG
	std::mutex				   lock_print;
	void safePrint(std::string msg)
	{
		lock_print.lock();
		std::cout << msg;
		lock_print.unlock();
	}
#endif // DEBUG


	bool exit        = false;
	bool stop_render = false;
	
	
	std::vector <std::mutex*>  work_locker_target_thread;
	std::vector <std::mutex*>  pauser_target_thread;
	std::vector <std::mutex*>  exit_locker;
	std::mutex				   wait_restart;
	
	std::condition_variable	   alert;
	std::vector <bool>         notified;
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
									RenderThreads * thr,
									size_t idx));
	template <class T>
	void CreateThreads(size_t number_of_threads,
						T* obj,
						void(*thr_funct)(T * obj,
										RenderThreads * thr,
										size_t idx));
	
	void WaitEndOfWorkFromAllThrds();
	void WaitEndOfWorkFromConcreteThr(int idx);

	void WaitExitFromAllThreads();
	void WaitExitFromConcreteThread(int idx);

	void StartAllThreads();
	void StopAllThreads();
	void StartConcreteThread(int idx);
	void StopConcreteThread(int idx);

	void SetPauseAllThreads();
	void UnsetPauseAllThreads();
	void SetPauseConcreteThread(int idx);
	void UnsetPauseConcreteThread(int idx);


	void DestroyThreads();
	

	friend void CreateImgPartMandelbrot(MandelSet* _this,
		RenderThreads *thr,
		size_t idx);



	~RenderThreads();
};










template<class T>
inline RenderThreads::RenderThreads(
	size_t number_of_threads,
	T* obj,
	void(*thr_funct)(T * obj,
					RenderThreads * thr,
					size_t idx)) :
	number_of_threads(number_of_threads),
	notified(std::vector <bool>(number_of_threads, false))
{
	



	for (size_t i = 0; i < number_of_threads; ++i)
	{
		work_locker_target_thread.push_back(new std::mutex);
		pauser_target_thread.push_back(new std::mutex);
		exit_locker.push_back(new std::mutex);

		threads.push_back(new std::thread((*thr_funct), obj, this, i));
		threads[i]->detach();
	}
}

template<class T>
inline void RenderThreads::CreateThreads(
	size_t number_of_threads,
	T * obj,
	void(*thr_funct)(T * obj,
					RenderThreads * thr,
					size_t idx))
{


	this->number_of_threads = number_of_threads;
	notified = std::vector <bool>(number_of_threads, false);
	exit = false;
	stop_render = false;


	for (size_t i = 0; i < number_of_threads; ++i)
	{
		work_locker_target_thread.push_back(new std::mutex);
		pauser_target_thread.push_back(new std::mutex);
		exit_locker.push_back(new std::mutex);

		threads.push_back(new std::thread((*thr_funct), obj, this, i));
		threads[i]->detach();
	}
}