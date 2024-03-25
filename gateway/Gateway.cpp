#include "Gateway.h"
#include "Log.h"
#include <unistd.h>
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <functional>
#include "json.h"
#include "Util.h"
#include "Wifi.h"
#include "Base64.h"
#include "Config.h"

Gateway *gateway = NULL;

Gateway::Gateway(string mac, int port, string ip, string localAddress, int localPort, string localClientid, string localUsername, string localPassword, int localKeepalive) : SocketProtocol(port, ip),
																																											  LocalProtocol(mac, localAddress, localPort, localClientid, localUsername, localPassword, localKeepalive)
{
	this->mac = mac;
	this->id = "";
	this->data = "";
}

Gateway::~Gateway()
{
}

void Gateway::init()
{
	SocketProtocol::init();
	LocalProtocol::init();
}

void Gateway::InitSocketMessage()
{
	SocketCmdCallbackRegister("start", bind(&Gateway::OnSocketStart, this, placeholders::_1, placeholders::_2));
	SocketCmdCallbackRegister("stop", bind(&Gateway::OnSocketStop, this, placeholders::_1, placeholders::_2));
}

int Gateway::OnSocketStart(Json::Value &reqValue, Json::Value &respValue)
{
	LOGD("Start test");

	uint8_t b = 1;
	uint8_t r = 1;
	uint8_t g = 1;
	int rssi = rand() % 100;
	int rgb = CODE_OK;
	for (int i = 0; i < 4; i++)
	{
		if (bleProtocol->ControlRgbSwitch(bleProtocol->getAddrDevTesting(), i + 1, b, g, r, 100, 20) != CODE_OK)
		{
			rgb = CODE_ERROR;
			break;
		}
	}

	// for ()
	
}

int Gateway::OnSocketStop(Json::Value &reqValue, Json::Value &respValue)
{
}

void Gateway::InitMqttLocalMessage()
{
	OnLocalCallbackRegister("device", bind(&Gateway::OnDeviceTest, this, placeholders::_1, placeholders::_2));
	OnLocalCallbackRegister("startScanBle", bind(&Gateway::OnStartScanBle, this, placeholders::_1, placeholders::_2));
	OnLocalCallbackRegister("stopScanBle", bind(&Gateway::OnStopScanBle, this, placeholders::_1, placeholders::_2));
}

int Gateway::OnDeviceTest(Json::Value &reqValue, Json::Value &respValue)
{
}

int Gateway::OnStartScanBle(Json::Value &reqValue, Json::Value &respValue)
{
	int rsCode = CODE_OK;
	if (bleProtocol)
	{
		bleProtocol->SetProvisioning(true);
		bleProtocol->StartScan();
	}
	else
	{
		rsCode = CODE_ERROR;
		LOGW("bleProtocol null");
	}

	respValue["data"]["code"] = rsCode;
	respValue["cmd"] = "startScanBle";

	return CODE_OK;
}

int Gateway::OnStopScanBle(Json::Value &reqValue, Json::Value &respValue)
{
	int rsCode = CODE_OK;
	if (bleProtocol)
	{
		bleProtocol->StopScan();
	}
	else
	{
		rsCode = CODE_ERROR;
		LOGW("BleProtocol null");
	}

	respValue["data"]["code"] = rsCode;
	respValue["cmd"] = "stopScanBle";
	return CODE_OK;
}

int Gateway::pushDeviceUpdateLocal(Json::Value &dataValue)
{
	return PublishToLocalMessage("deviceTest", dataValue, "deviceTestRsp", NULL, 0);
}

string Gateway::getId()
{
	return id;
}

string Gateway::getVersion()
{
	return version;
}

string Gateway::getName()
{
	return "";
}

string Gateway::getData()
{
	return data;
}

string Gateway::getMac()
{
	return mac;
}

void Gateway::setData(string data)
{
	this->data = data;
}

void Gateway::setId(string id)
{
	this->id = id;
}

void Gateway::setMac(string mac)
{
	this->mac = mac;
}

void Gateway::setVersion(string version)
{
	this->version = version;
}

void Gateway::setName(string name)
{
}