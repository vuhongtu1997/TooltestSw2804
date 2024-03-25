#include <string>
#include <iostream>
#include <endian.h>
#include <fstream>
#include "Config.h"
#include "Log.h"
#include "json.h"
#include "Define.h"

#define TAG "Config"

Config *config = NULL;

static bool get_str_config_entry(Json::Value &jsonData, string key, string &value)
{
	if (jsonData.isObject() && jsonData.isMember(key) && jsonData[key].isString())
	{
		string temp = jsonData[key].asString();
		value = temp;
		return true;
	}
	else
	{
		LOGW("Json data error: %s", jsonData.toString().c_str());
		return false;
	}
}

static bool get_int_config_entry(Json::Value &jsonData, string key, int &value)
{
	if (jsonData.isObject() && jsonData.isMember(key) && jsonData[key].isInt())
	{
		int temp = jsonData[key].asInt();
		value = temp;
		return true;
	}
	else
	{
		LOGW("Json data error: %s", jsonData.toString().c_str());
		return false;
	}
}

static bool set_str_config_entry(Json::Value &jsonData, string key, string &value)
{
	if (jsonData.isObject() && jsonData.isMember(key) && jsonData[key].isString())
	{
		jsonData[key] = value;
		return true;
	}
	else
	{
		LOGW("Json data error: %s", jsonData.toString().c_str());
		return false;
	}
}

static bool set_int_config_entry(Json::Value jsonData, string key, int &value)
{
	if (jsonData.isObject() && jsonData.isMember(key) && jsonData[key].isInt())
	{
		jsonData[key] = value;
		return true;
	}
	else
	{
		LOGW("Json data error: %s", jsonData.toString().c_str());
		return false;
	}
}

static bool OpenFile(string file, Json::Value &jsonData)
{
	std::ifstream input(file.c_str());
	if (!input.is_open())
	{
		std::cerr << "Không thể mở tệp JSON." << std::endl;
		return false;
	}

	input >> jsonData;
	input.close();
	return true;
}

static bool Write2File(string file, Json::Value &jsonData)
{
	std::ofstream output(file.c_str());
	if (!output.is_open())
	{
		std::cerr << "Không thể mở tệp JSON." << std::endl;
		return false;
	}

	output << jsonData;
	output.close();
	return true;
}


Config::Config()
{
}

void Config::ReadConfig()
{
}

void Config::Print()
{
}