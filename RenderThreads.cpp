#include "RenderThreads.h"




RenderThreads::RenderThreads()
{
}





void RenderThreads::StartAllThreads()
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

void RenderThreads::StopAllThreads()
{
	stop_render = true;
	for (auto locker : work_locker_target_thread)
		locker->lock();
	for (auto locker : work_locker_target_thread)
		locker->unlock();
	stop_render = false;
}

void RenderThreads::StartConcreteThread(int idx)
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


void RenderThreads::StopConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
	{
		stop_render = true;
		work_locker_target_thread[idx]->lock();
		work_locker_target_thread[idx]->unlock();
		stop_render = false;
	}
}


void RenderThreads::setPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->lock();
}


void RenderThreads::unsetPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->unlock();
}


void RenderThreads::setPauseConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
		pauser_target_thread[idx]->lock();
}


void RenderThreads::unsetPauseConcreteThread(int idx)
{
	if (idx < static_cast<int>(number_of_threads) && idx >= 0)
		pauser_target_thread[idx]->unlock();
}


void RenderThreads::EndThreads()
{
	end_flag = true;
	
	StopAllThreads();
	work_locker_target_thread.clear();
	pauser_target_thread.clear();
	notified.clear();
	threads.clear();
}




RenderThreads::~RenderThreads()
{
}
