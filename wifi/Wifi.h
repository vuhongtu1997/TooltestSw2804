#pragma once

#include <string>
#include <vector>
#include "json.h"
#include <iostream>

// #include "esp_wifi.h"

using namespace std;

namespace Wifi
{
	// private:
	// 	wifi_mode_t wifiMode;

	// public:
	void init();
	void ScanWifi(Json::Value & jsonValue);
	int ConnectToWifi(string ssid, string password, string encryption);
	int SetModeApWifi();
	void WifiStartAP(void);
	void WifiStartSta(void);
	bool WifiIsAPMode(void);
	bool WifiIsStaMode(void);

	string GetMacAddress();
	string GetIP();
}
