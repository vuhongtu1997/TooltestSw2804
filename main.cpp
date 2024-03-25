#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <signal.h>
#include <fstream>
#include "json.h"
#include "Log.h"
#include "Config.h"
#include "Gateway.h"
#include "Util.h"
#include "Wifi.h"
#include "TimerSchedule.h"
#include "ButtonSignal.h"
#include "BleProtocol.h"

#define TAG "MAIN"

using namespace std;

static void signal_handler(int sig)
{
	LOGI("signal_handler: %d", sig);
	if (sig == SIGUSR1)
	{
		buttonSignal->OnPress();
	}
	else if (sig == SIGUSR2)
	{
		buttonSignal->OnRelease();
	}
	signal(sig, signal_handler);
}

int main(int argc, char *argv[])
{
	LOGI("Start ver " STR(VERSION));

	buttonSignal = new ButtonSignal();
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);

	srand(time(0));

	config = new Config();
	config->ReadConfig();

	timerSchedule = new TimerSchedule();
	timerSchedule->init();

	bleProtocol = new BleProtocol((char *)BLE_UART_PORT, B115200);
	bleProtocol->init();

	string mac = Wifi::GetMacAddress();
	LOGI("mac: %s", mac.c_str());

	string passMqttLocal = "1";
	gateway = new Gateway(mac, 12345, "localhost", "localhost", 1883, "", "RD", "1", 10);
	gateway->init();

	bleProtocol->InitKey();

	Util::LedService(true);
	Util::LedZigbee(false);

	while (1)
	{
		sleep(10);
	}
	LOGI("exit main");
	return 0;
}
