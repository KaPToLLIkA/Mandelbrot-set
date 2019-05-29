#include "RenderThreads.h"



template<class T>
RenderThreads<T>::RenderThreads()
{
}


template<class T>
RenderThreads<T>::RenderThreads(
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
void RenderThreads<T>::CreateThreads(
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

template<class T>
void RenderThreads<T>::StartAllThreads()
{
	for (auto locker : work_locker_target_thread)
		locker->lock();

	std::unique_lock <std::mutex> locker(*wait_restart);

	for (size_t i = 0; i < notified.size(); ++i)
		notified[i] = true;

	alert->notify_all();

	for (auto locker : work_locker_target_thread)
		locker->unlock();
}
template<class T>
void RenderThreads<T>::StopAllThreads()
{
	stop_render = true;
	for (auto locker : work_locker_target_thread)
		locker->lock();
	for (auto locker : work_locker_target_thread)
		locker->unlock();
	stop_render = false;
}
template<class T>
void RenderThreads<T>::StartConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
	{
		work_locker_target_thread[idx]->lock();
		work_locker_target_thread[idx]->unlock();
		std::unique_lock <std::mutex> locker(*wait_restart);
		notified[idx] = true;
		alert->notify_one();

	}
}

template<class T>
void RenderThreads<T>::StopConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
	{
		stop_render = true;
		work_locker_target_thread[idx]->lock();
		work_locker_target_thread[idx]->unlock();
		stop_render = false;
	}
}

template<class T>
void RenderThreads<T>::setPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->lock();
}

template<class T>
void RenderThreads<T>::unsetPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->unlock();
}

template<class T>
void RenderThreads<T>::setPauseConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
		pauser_target_thread[idx]->lock();
}

template<class T>
void RenderThreads<T>::unsetPauseConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
		pauser_target_thread[idx]->unlock();
}

template<class T>
void RenderThreads<T>::EndThreads()
{
	end_flag = true;
	
	StopAllThreads();
	work_locker_target_thread.clear();
	pauser_target_thread.clear();
	notified.clear();
	threads.clear();
}



template<class T>
RenderThreads<T>::~RenderThreads()
{
}
