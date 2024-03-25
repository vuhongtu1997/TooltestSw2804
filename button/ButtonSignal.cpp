#include "ButtonSignal.h"
#include <unistd.h>
#include <stdio.h>
#include "Log.h"
#include "Util.h"
#include "Wifi.h"
#include "Gateway.h"

#define DOUBLE_CLICK_TIME 400
#define AP_MODE_WIFI 5
#define MODE_SEND_UDP_BROADCAST 3

ButtonSignal *buttonSignal = NULL;

ButtonSignal::ButtonSignal()
{
	pressTime = 0;
	releaseTime = 0;
	startProcess = true;
	clickCount = 0;
}

void ButtonSignal::OnPress()
{
	LOGI("OnPress");
}

void ButtonSignal::OnRelease()
{
	LOGI("OnRelease");
}
