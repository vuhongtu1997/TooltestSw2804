#include "Util.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <ctime>
#include <locale>
#include "Base64.h"
#include "Log.h"
#include <fstream>
#include <sstream>
#include <iomanip>

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

string Util::genRandRQI(int size)
{
	string rqi = "";
	srand(time(0));
	for (int i = 0; i < size; i++)
	{
		rqi += 'a' + rand() % 26;
	}
	return rqi;
}

string getTimeStrFromTime(time_t t)
{
	struct tm start;
	start = *localtime(&t);
	char timeBuffer[100];
	sprintf(timeBuffer, "%04d%02d%02dT%02d%02d%02d", start.tm_year + 1900, start.tm_mon + 1, start.tm_mday, start.tm_hour, start.tm_min, start.tm_sec);
	return string(timeBuffer);
}

string Util::GetCurrentTimeStr()
{
	return getTimeStrFromTime(time(NULL));
}

int Util::GetCurrentTimer()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_hour * 3600 + lt.tm_min * 60 + lt.tm_sec;
}

int Util::GetYearsCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_year + 1900;
}

int Util::GetMonthsCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_mon + 1;
}

int Util::GetDateCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_mday;
}

int Util::GetDaysCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_wday + 1;
}

int Util::GetHoursCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_hour;
}

int Util::GetMinutesCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_min;
}

int Util::GetSecondsCurrent()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_sec;
}

double Util::millis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

int Util::GetCurrentWeekDay()
{
	time_t t = time(NULL);
	struct tm lt = *localtime(&t);
	return lt.tm_wday;
}

int Util::ConvertStrTimeToInt(string time)
{
	int hour;
	int minute;
	int second;
	if (sscanf(time.c_str(), "%d:%d:%d", &hour, &minute, &second) == 3)
		return hour * 3600 + minute * 60 + second;
	return CODE_ERROR;
}

uint8_t Util::CalCrc(uint8_t length, uint8_t *data)
{
	uint8_t crc = 0;
	int i = 0;
	for (i = 0; i < length; i++)
	{
		crc = crc ^ data[i];
	}
	crc = crc & 0xFF;
	return crc;
}

string Util::setString(const char *value)
{
	return value ? value : "";
}

string Util::ConvertU32ToHexString(uint8_t *data, int len)
{
	char buff[100];
	if (len > 50)
		len = 50;
	for (int i = 0; i < len; i++)
	{
		sprintf(buff + i * 2, "%02x", data[i]);
	}
	return string(buff);
}

int Util::ConvertStringToHex(string str, uint8_t *data, int len)
{
	LOGE("Bo sung code");
	return 0;
}

int Util::CheckDayInWeek(int day, int repeater)
{
	int byte;
	switch (day)
	{
	case 0: // sun
		byte = 6;
		break;
	case 1: // mon
	case 2: // tue
	case 3: // wed
	case 4: // thu
	case 5: // fri
	case 6: // sat
		byte = day - 1;
		break;
	default:
		return false;
	}
	if (repeater & (1 << byte))
		return true;
	return false;
}

vector<string> Util::splitString(string str, char splitter)
{
	vector<string> result;
	string current = "";
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == splitter)
		{
			if (current != "")
			{
				result.push_back(current);
				current = "";
			}
			continue;
		}
		current += str[i];
	}
	if (current.size() != 0)
		result.push_back(current);
	return result;
}

string Util::ExecuteCMD(char const *command)
{
	string msg_rsp = "";
#ifndef ESP_PLATFORM
	FILE *file;
	char msg_line[100] = {0};
	file = popen(command, "r");
	if (file == NULL)
	{
		LOGE("ExecuteCMD");
		exit(1);
	}
	while (1)
	{
		fgets(msg_line, 100, file);
		msg_rsp += msg_line;
		if (feof(file))
		{
			break;
		}
	}
	pclose(file);
#endif
	return msg_rsp;
}

string Util::uuidToStr(uint8_t *uuid)
{
	char buf[100];
	sprintf(buf, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
					uuid[0], uuid[1], uuid[2], uuid[3],
					uuid[4], uuid[5], uuid[6], uuid[7],
					uuid[8], uuid[9], uuid[10], uuid[11],
					uuid[12], uuid[13], uuid[14], uuid[15]);
	buf[36] = '\0';
	return string(buf);
}

string Util::GenUuidFromMac(string mac)
{
	if (mac.length() == 16)
	{
		string mac1 = mac.substr(0, 4);
		string mac2 = mac.substr(4, 4);
		string mac3 = mac.substr(8, 4);
		string mac4 = mac.substr(12, 4);
		return mac1 + mac2 + "-" + mac3 + "-" + mac4 + "-" + mac1 + "-" + mac2 + mac3 + mac4;
	}
	return "";
}

static bool ledInternet = false;
static bool ledService = false;
static bool ledZigbee = false;
static bool ledBle = false;

void Util::LedInternet(bool value)
{
	ledInternet = value;
#ifdef __OPENWRT__
	if (value)
	{
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:internet/brightness");
	}
	else
	{
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:internet/brightness");
	}
#endif
}

void Util::LedService(bool value)
{
	ledService = value;
#ifdef __OPENWRT__
	if (value)
	{
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:service/brightness");
	}
	else
	{
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:service/brightness");
	}
#endif
}

void Util::LedZigbee(bool value)
{
	ledZigbee = value;
#ifdef __OPENWRT__
	if (value)
	{
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:ble2/brightness");
	}
	else
	{
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:ble2/brightness");
	}
#endif
}

void Util::LedBle(bool value)
{
	ledBle = value;
#ifdef __OPENWRT__
	if (value)
	{
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:ble1/brightness");
	}
	else
	{
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:ble1/brightness");
	}
#endif
}

void Util::LedAll(bool value)
{
#ifdef __OPENWRT__
	if (value)
	{
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:internet/brightness");
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:service/brightness");
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:ble1/brightness");
		ExecuteCMD("/bin/echo \"1\" > /sys/class/leds/linkit-smart-7688:orange:ble2/brightness");
	}
	else
	{
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:internet/brightness");
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:service/brightness");
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:ble1/brightness");
		ExecuteCMD("/bin/echo \"0\" > /sys/class/leds/linkit-smart-7688:orange:ble2/brightness");
	}
#endif
}

void Util::LedRestoreLastValue()
{
	LedInternet(ledInternet);
	LedService(ledService);
	LedZigbee(ledZigbee);
	LedBle(ledBle);
}

static int ledServiceCount = 0;
void Util::LedServiceLock()
{
	ledServiceCount++;
	if (ledServiceCount)
		LedService(false);
}

void Util::LedServiceUnlock()
{
	ledServiceCount--;
	if (!ledServiceCount)
		LedService(true);
}

bool Util::GetStatusLedBle()
{
	return ledBle;
}

bool Util::GetStatusLedService()
{
	return ledService;
}

bool Util::GetStatusLedZigbee()
{
	return ledZigbee;
}

bool Util::GetStatusLedInternet()
{
	return ledInternet;
}