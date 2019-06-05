#include "RenderThreads.h"



bool RenderThreads::CheckIDX(int &idx)
{
	return (idx < static_cast<int>(number_of_threads) && idx >= 0);
}

////////////////////////////////////////////////////////////////////////////
RenderThreads::RenderThreads()
{
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::WaitEndOfWorkFromAllThrds()
{
	for (auto locker : work_locker_target_thread)
		locker->lock();
	for (auto locker : work_locker_target_thread)
		locker->unlock();
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::WaitEndOfWorkFromConcreteThr(int idx)
{
	if (CheckIDX(idx))
	{
		work_locker_target_thread[idx]->lock();
		work_locker_target_thread[idx]->unlock();
	}
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::WaitExitFromAllThreads()
{
	for (auto locker : exit_locker)
	{
		locker->lock();
		locker->unlock();
	}
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::WaitExitFromConcreteThread(int idx)
{
	if (CheckIDX(idx))
	{
		exit_locker[idx]->lock();
		exit_locker[idx]->unlock();
	}
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::StartAllThreads()
{
	WaitEndOfWorkFromAllThrds();

	std::unique_lock <std::mutex> locker(wait_restart);

	for (size_t i = 0; i < notified.size(); ++i)
		notified[i] = true;

	alert.notify_all();
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::StopAllThreads()
{
	stop_render = true;
	
	WaitEndOfWorkFromAllThrds();

	stop_render = false;
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::StartConcreteThread(int idx)
{
	if (CheckIDX(idx))
	{
		WaitEndOfWorkFromConcreteThr(idx);

		std::unique_lock <std::mutex> locker(wait_restart);
		notified[idx] = true;
		alert.notify_one();

	}
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::StopConcreteThread(int idx)
{
	if (CheckIDX(idx))
	{
		stop_render = true;
		WaitEndOfWorkFromConcreteThr(idx);
		stop_render = false;
	}
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::SetPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->lock();
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::UnsetPauseAllThreads()
{
	for (auto locker : pauser_target_thread)
		locker->unlock();
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::SetPauseConcreteThread(int idx)
{
	if (CheckIDX(idx))
		pauser_target_thread[idx]->lock();
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::UnsetPauseConcreteThread(int idx)
{
	if (CheckIDX(idx))
		pauser_target_thread[idx]->unlock();
}

bool RenderThreads::CheckStatusFromAll()
{
	bool all_ended = true;
	for (auto ended : render_status)
		if (!ended) all_ended = false;
	return all_ended;
}

bool RenderThreads::CheckStatusFromConcrete(int idx)
{
	if (CheckIDX(idx))
	{
		return render_status[idx];
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////
void RenderThreads::DestroyThreads()
{
	exit = true;
	stop_render = true;

	StartAllThreads();
	WaitExitFromAllThreads();

	work_locker_target_thread.clear();
	pauser_target_thread.clear();
	threads.clear();
	notified.clear();
	render_status.clear();
	stop_render = false;
}

////////////////////////////////////////////////////////////////////////////
RenderThreads::~RenderThreads()
{
}
