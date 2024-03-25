#pragma once

using namespace std;

class ButtonSignal
{
private:
	double pressTime;
	double releaseTime;
	bool startProcess;
	int clickCount;
	volatile bool isBlinkLed;

public:
	ButtonSignal();

	void OnPress();
	void OnRelease();
};

extern ButtonSignal *buttonSignal;