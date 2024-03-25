#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "json.h"
#include "ErrorCode.h"

using namespace std;

namespace Util
{
	int GetCurrentTimer();

	int GetYearsCurrent();
	int GetMonthsCurrent();
	int GetDateCurrent();
	int GetDaysCurrent();
	int GetHoursCurrent();
	int GetMinutesCurrent();
	int GetSecondsCurrent();

	double millis();

	string genRandRQI(int size);

	int GetCurrentWeekDay();
	int ConvertStrTimeToInt(string time);
	uint8_t CalCrc(uint8_t length, uint8_t *data);
	string setString(const char *value);
	string ConvertU32ToHexString(uint8_t *data, int len);
	int ConvertStringToHex(string str, uint8_t *data, int len);
	int CheckDayInWeek(int day, int repeater);
	vector<string> splitString(string str, char splitter);

	string ExecuteCMD(char const *command);
	string GetCurrentTimeStr();

	string uuidToStr(uint8_t *uuid);
	string GenUuidFromMac(string mac);

	void LedInternet(bool value);
	void LedService(bool value);
	void LedZigbee(bool value);
	void LedBle(bool value);
	void LedAll(bool value);
	void LedRestoreLastValue();
	void LedServiceLock();
	void LedServiceUnlock();

	bool GetStatusLedBle();
	bool GetStatusLedService();
	bool GetStatusLedZigbee();
	bool GetStatusLedInternet();
	Json::Value arrangeJson(Json::Value &obj);


	string encryptAes128(string key, string plaintext);
	string calculateSHA256Checksum(string &filePath);

}
