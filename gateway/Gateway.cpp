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

string messageReqSocket = "{\"dsID\":\"HCRemoteMonitor\",\"reqType\":\"query\",\"queryAddr\":[\"Addr-811\"]}";
string messageRepButtonSocket = "{\"dsID\":\"HCRemoteMonitor\",\"reqType\":\"query\",\"queryAddr\":[\"Addr-812\"]}";

Gateway::Gateway(string mac, int port, string ip, string localAddress, int localPort, string localClientid, string localUsername, string localPassword, int localKeepalive) : SocketProtocol(port, ip),
																																											  LocalProtocol(mac, localAddress, localPort, localClientid, localUsername, localPassword, localKeepalive)
{
	this->mac = mac;
	this->id = "";
	this->data = "";
	this->isTesting = false;
	this->addrDevTesting = 0;

	// listErrorTouch13.push_back(9);
	listErrorTouch24.push_back(9);
	// listErrorRgb4.push_back(9);
	// listErrorRgb34.push_back(9);
	// listErrorRgb234.push_back(9);
}

Gateway::~Gateway()
{
}

void Gateway::init()
{
	SocketProtocol::setMessageSend(messageReqSocket);
	SocketProtocol::init();
	LocalProtocol::init();
	InitSocketMessage();
	InitMqttLocalMessage();
	LocalConnect();
}

void Gateway::InitSocketMessage()
{
	SocketCmdCallbackRegister("HCRemoteMonitor", bind(&Gateway::OnSocketStart, this, placeholders::_1, placeholders::_2));
	SocketCmdCallbackRegister("stop", bind(&Gateway::OnSocketStop, this, placeholders::_1, placeholders::_2));
}

Json::Value Gateway::SocketCmdStartCheck(Json::Value &dataRspLocal)
{
	Json::Value result = Json::objectValue;
	bleProtocol->SetOnOffLight(65535, 0, 0, false);
	bleProtocol->SetOnOffLight(65535, 0, 0, false);
	bleProtocol->ControlRgbSwitch(bleProtocol->addrDevTesting, 255, 0, 0, 255, 100, 20);
	if (bleProtocol->addrDevTesting != addrDevTesting) // dang test thiet bi cu
	{
		// LocalProtocol::PublishToLocalMessage(dataRspLocal);
	}
	addrDevTesting = bleProtocol->addrDevTesting;
	return result;
}

Json::Value Gateway::SocketCmdButtonCheck(Json::Value &dataRspLocal)
{
	Json::Value result = Json::objectValue;
	addrDevTesting = bleProtocol->addrDevTesting;
	if (checkTypeError(addrDevTesting) == 0)
	{
		dataRspLocal["touch1"] = 1;
		dataRspLocal["touch2"] = bleProtocol->SetOnOffLight(addrDevTesting + 1, 1, 0, true);
		dataRspLocal["touch3"] = 1;
		dataRspLocal["touch4"] = bleProtocol->SetOnOffLight(addrDevTesting + 3, 1, 0, true);

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 1, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb3"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 2, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb4"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 3, 0, 128, 255, 0, 100, 20);
	}
	else if (checkTypeError(addrDevTesting) == 1)
	{
		dataRspLocal["touch1"] = bleProtocol->SetOnOffLight(addrDevTesting, 1, 0, true);
		dataRspLocal["touch2"] = 1;
		dataRspLocal["touch3"] = bleProtocol->SetOnOffLight(addrDevTesting + 2, 1, 0, true);
		dataRspLocal["touch4"] = 1;

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 1, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb3"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 2, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb4"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 3, 0, 128, 255, 0, 100, 20);
	}
	else if (checkTypeError(addrDevTesting) == 2)
	{
		dataRspLocal["touch1"] = bleProtocol->SetOnOffLight(addrDevTesting, 1, 0, true);
		dataRspLocal["touch2"] = bleProtocol->SetOnOffLight(addrDevTesting + 1, 1, 0, true);
		dataRspLocal["touch3"] = bleProtocol->SetOnOffLight(addrDevTesting + 2, 1, 0, true);
		dataRspLocal["touch4"] = bleProtocol->SetOnOffLight(addrDevTesting + 3, 1, 0, true);

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 1, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb3"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 2, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb4"] = 1;
	}
	else if (checkTypeError(addrDevTesting) == 3)
	{
		dataRspLocal["touch1"] = bleProtocol->SetOnOffLight(addrDevTesting, 1, 0, true);
		dataRspLocal["touch2"] = bleProtocol->SetOnOffLight(addrDevTesting + 1, 1, 0, true);
		dataRspLocal["touch3"] = bleProtocol->SetOnOffLight(addrDevTesting + 2, 1, 0, true);
		dataRspLocal["touch4"] = bleProtocol->SetOnOffLight(addrDevTesting + 3, 1, 0, true);

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 1, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb3"] = 1;
		dataRspLocal["rgb4"] = 1;
	}
	else if (checkTypeError(addrDevTesting) == 4)
	{
		dataRspLocal["touch1"] = bleProtocol->SetOnOffLight(addrDevTesting, 1, 0, true);
		dataRspLocal["touch2"] = bleProtocol->SetOnOffLight(addrDevTesting + 1, 1, 0, true);
		dataRspLocal["touch3"] = bleProtocol->SetOnOffLight(addrDevTesting + 2, 1, 0, true);
		dataRspLocal["touch4"] = bleProtocol->SetOnOffLight(addrDevTesting + 3, 1, 0, true);

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = 1;
		dataRspLocal["rgb3"] = 1;
		dataRspLocal["rgb4"] = 1;
	}
	else
	{
		dataRspLocal["touch1"] = bleProtocol->SetOnOffLight(addrDevTesting, 1, 0, true);
		dataRspLocal["touch2"] = bleProtocol->SetOnOffLight(addrDevTesting + 1, 1, 0, true);
		dataRspLocal["touch3"] = bleProtocol->SetOnOffLight(addrDevTesting + 2, 1, 0, true);
		dataRspLocal["touch4"] = bleProtocol->SetOnOffLight(addrDevTesting + 3, 1, 0, true);

		dataRspLocal["load1"] = dataRspLocal["touch1"];
		dataRspLocal["load2"] = dataRspLocal["touch2"];
		dataRspLocal["load3"] = dataRspLocal["touch3"];
		dataRspLocal["load4"] = dataRspLocal["touch4"];

		dataRspLocal["rgb1"] = bleProtocol->ControlRgbSwitch(addrDevTesting, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb2"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 1, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb3"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 2, 0, 128, 255, 0, 100, 20);
		dataRspLocal["rgb4"] = bleProtocol->ControlRgbSwitch(addrDevTesting + 3, 0, 128, 255, 0, 100, 20);
	}

	string rs = "1";
	if (dataRspLocal["touch1"] != CODE_OK ||
		dataRspLocal["touch1"] != CODE_OK ||
		dataRspLocal["touch1"] != CODE_OK ||
		dataRspLocal["touch1"] != CODE_OK ||
		dataRspLocal["load1"] != CODE_OK ||
		dataRspLocal["load2"] != CODE_OK ||
		dataRspLocal["load3"] != CODE_OK ||
		dataRspLocal["load4"] != CODE_OK ||
		dataRspLocal["rgb1"] != CODE_OK ||
		dataRspLocal["rgb2"] != CODE_OK ||
		dataRspLocal["rgb3"] != CODE_OK ||
		dataRspLocal["rgb4"] != CODE_OK)
	{
		rs = "0";
	}
	result["dsID"] = "HCRemoteMonitor";
	result["reqType"] = "command";
	Json::Value arrayJson = Json::arrayValue;
	arrayJson.append("rewriteData");
	arrayJson.append(SOCKET_ADDR_FINISH);
	arrayJson.append(rs);
	result["cmdData"] = arrayJson;

	return result;
}

void Gateway::PushLocalResult(Json::Value data)
{
	if (!data.empty())
	{
		Json::Value rs = Json::objectValue;
		rs["cmd"] = "resultTestSwitch";
		rs["rqi"] = "rangdong2804";
		rs["data"] = data;
		LocalProtocol::PublishToLocalMessage(rs);
	}
}

int Gateway::OnSocketStart(Json::Value &reqValue, Json::Value &respValue)
{
	respValue = Json::objectValue;
	if (reqValue.isMember("queryAddr") && reqValue["queryAddr"].isArray() &&
		reqValue.isMember("queryData") && reqValue["queryData"].isArray() &&
		reqValue.isMember("reqType") && reqValue["reqType"].isString())
	{
		Json::Value queryDataJson = reqValue["queryData"];
		Json::Value queryAddrJson = reqValue["queryAddr"];
		string reqTypeStr = reqValue["reqType"].asString();
		if (queryDataJson.size() == 1 && queryAddrJson.size() == 1)
		{
			if ((queryDataJson[0].isString()) && queryAddrJson[0].isString())
			{
				if (queryAddrJson[0].asString() == SOCKET_ADDR_START)
				{
					if (queryDataJson[0].asString() == "1")
					{
						respValue = SocketCmdStartCheck(dataRspLocal);
						// SocketProtocol::setMessageSend(messageRepButtonSocket);

						respValue = SocketCmdButtonCheck(dataRspLocal);
						PushLocalResult(dataRspLocal);
						dataRspLocal.clear();
						SocketProtocol::setMessageSend(messageReqSocket);
					}
				}
				else if (queryAddrJson[0].asString() == SOCKET_ADDR_BUTTON)
				{
					respValue = SocketCmdButtonCheck(dataRspLocal);
					LocalProtocol::PublishToLocalMessage(dataRspLocal);
					dataRspLocal.clear();
					SocketProtocol::setMessageSend(messageReqSocket);
				}
			}
		}
	}
	else
		LOGW("Data error: %s", reqValue.toString().c_str());
	return CODE_OK;
}

int Gateway::OnSocketStop(Json::Value &reqValue, Json::Value &respValue)
{
	return CODE_OK;
}

void Gateway::InitMqttLocalMessage()
{
	OnLocalCallbackRegister("device", bind(&Gateway::OnDeviceTest, this, placeholders::_1, placeholders::_2));
	OnLocalCallbackRegister("startScanBle", bind(&Gateway::OnStartScanBle, this, placeholders::_1, placeholders::_2));
	OnLocalCallbackRegister("stopScanBle", bind(&Gateway::OnStopScanBle, this, placeholders::_1, placeholders::_2));
}

int Gateway::OnDeviceTest(Json::Value &reqValue, Json::Value &respValue)
{
	return CODE_OK;
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

int Gateway::checkTypeError(uint16_t devAddr)
{
	for (auto &adr : listErrorTouch13)
	{
		if (devAddr == adr)
			return 0;
	}

	for (auto &adr : listErrorTouch24)
	{
		if (devAddr == adr)
			return 1;
	}

	for (auto &adr : listErrorRgb4)
	{
		if (devAddr == adr)
			return 2;
	}

	for (auto &adr : listErrorRgb34)
	{
		if (devAddr == adr)
			return 3;
	}

	for (auto &adr : listErrorRgb234)
	{
		if (devAddr == adr)
			return 4;
	}

	return -1;
}