#include "RenderThreads.h"



template<class T>
RenderThreads<T>::RenderThreads()
{
}

template<class T>
RenderThreads<T>::RenderThreads(size_t number_of_threads,
	T* obj,
	void(*thr_funct)(T* obj,
		std::mutex *locker,
		bool *end_flag)) :
	number_of_threads(number_of_threads)
{
	
	for (size_t i = 0; i < number_of_threads; ++i)
	{
		lock_target_thread.push_back(new std::mutex);
		lock_target_thread[i]->lock();
		threads.push_back(std::move(std::thread((*thr_funct),
			obj,
			lock_target_thread[i],
			&end_flag)));
		threads[i]->detach();
	}
}

template<class T>
void RenderThreads<T>::CreateThreads(size_t number_of_threads, 
	T * obj, 
	void(*thr_funct)(T *obj, 
		std::mutex *locker, 
		bool *end_flag))
{
	this->number_of_threads = number_of_threads;
	for (size_t i = 0; i < number_of_threads; ++i)
	{
		lock_target_thread.push_back(new std::mutex);
		lock_target_thread[i]->lock();
		threads.push_back(std::move(std::thread((*thr_funct),
			obj,
			lock_target_thread[i],
			&end_flag)));
		threads[i]->detach();
	}
}

template<class T>
void RenderThreads<T>::StartAllThreads()
{
	for (auto locker : lock_target_thread)
		locker->unlock();
}
template<class T>
void RenderThreads<T>::StopAllThreads()
{
	for (auto locker : lock_target_thread)
		locker->lock();
}
template<class T>
void RenderThreads<T>::StartConcreteThread(int idx)
{
	if (idx < number_of_threads && idx >= 0)
		lock_target_thread[idx]->unlock();
}

template<class T>
void RenderThreads<T>::StopConcreteThread(int idx)
{
	if (idx < number_of_threads && idx >= 0)
		lock_target_thread[idx]->lock();
}

template<class T>
void RenderThreads<T>::EndThreads()
{
	end_flag = true;
	lock_target_thread.clear();
	threads.clear();
}



template<class T>
RenderThreads<T>::~RenderThreads()
{
}
