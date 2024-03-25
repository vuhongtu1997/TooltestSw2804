#pragma once

#include <vector>
#include <mutex>
#include <functional>

using namespace std;

typedef function<void()> TimerCallbackFunc;

class Timer
{
private:
	int index;
	int time;

public:
	TimerCallbackFunc timerCallbackFunc;
	Timer(int index, int time, TimerCallbackFunc timerCallbackFunc);

	int GetIndex();
	bool IsAtTime(int time);
	void run();
};

class TimerSchedule
{
private:
	int index;

public:
	mutex mtx;

	TimerSchedule();
	vector<Timer *> timerList;

	void init();
	int RegisterTimer(string timer, TimerCallbackFunc timerCallbackFunc);
	int RegisterTimer(int time, TimerCallbackFunc timerCallbackFunc);
	int UnregisterTimer(int index);
};

extern TimerSchedule *timerSchedule;
