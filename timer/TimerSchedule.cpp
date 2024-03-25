#include "TimerSchedule.h"
#include <unistd.h>
#include "Util.h"
#include "Log.h"
#include "ErrorCode.h"
#include <thread>

TimerSchedule *timerSchedule = NULL;

Timer::Timer(int index, int time, TimerCallbackFunc timerCallbackFunc)
{
	this->index = index;
	this->time = time;
	this->timerCallbackFunc = timerCallbackFunc;
}

int Timer::GetIndex()
{
	return index;
}

bool Timer::IsAtTime(int time)
{
	if (this->time == time)
		return true;
	return false;
}

static void TimerDoThread(void *data)
{
	LOGI("TimerDoThread Start");
	Timer *timer = (Timer *)data;
	try
	{
		timer->timerCallbackFunc();
	}
	catch (...)
	{
		LOGE("Timer run error");
	}
}

void Timer::run()
{
	LOGD("run");
	thread timerDoThread(TimerDoThread, this);
	timerDoThread.detach();
}

TimerSchedule::TimerSchedule()
{
	index = 0;
}

static void TimerThread(void *data)
{
	LOGI("Start Timer Thread");
	TimerSchedule *timerSchedule = (TimerSchedule *)data;
	int currentTimer, oldTimer = 0;
	while (1)
	{
		currentTimer = Util::GetCurrentTimer();
		if (currentTimer != oldTimer)
		{
			// LOGD("h:m: %d-%d", currentTimer, currentTimer);
			timerSchedule->mtx.lock();
			for (auto &timer : timerSchedule->timerList)
			{
				if (timer->IsAtTime(currentTimer))
				{
					timer->run();
				}
			}
			timerSchedule->mtx.unlock();
			oldTimer = currentTimer;
		}
		usleep(500000);
	}
}

void TimerSchedule::init()
{
	LOGI("Start Timer init");
	thread timerThread(TimerThread, this);
	timerThread.detach();
}

int TimerSchedule::RegisterTimer(string timerStr, TimerCallbackFunc timerCallbackFunc)
{
	int timer = Util::ConvertStrTimeToInt(timerStr);
	return RegisterTimer(timer, timerCallbackFunc);
}

int TimerSchedule::RegisterTimer(int time, TimerCallbackFunc timerCallbackFunc)
{
	Timer *timer = new Timer(++index, time, timerCallbackFunc);
	mtx.lock();
	timerList.push_back(timer);
	mtx.unlock();
	return index;
}

int TimerSchedule::UnregisterTimer(int index)
{
	mtx.lock();
	for (auto &timer : timerList)
	{
		if (timer->GetIndex() == index)
		{
			timerList.erase(remove(timerList.begin(), timerList.end(), timer), timerList.end());
			delete timer;
		}
	}
	mtx.unlock();
	return CODE_OK;
}
