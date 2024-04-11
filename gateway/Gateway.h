#pragma once

#include <string>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include "json.h"
#include "Define.h"
#include "ErrorCode.h"
#include "Udp.h"
#include "BleProtocol.h"
#include "SocketProtocol.h"
#include "LocalProtocol.h"

#define SOCKET_ADDR_START "Addr-811"
#define SOCKET_ADDR_BUTTON "Addr-812"
#define SOCKET_ADDR_FINISH "Addr-813"

using namespace std;

class Gateway : public SocketProtocol, public LocalProtocol
{
private:
	string id;
	string mac;
	string version;
	string data;

	uint16_t addrDevTesting;
	bool isTesting;

	Json::Value dataRspLocal;

	vector <uint16_t> listErrorTouch13;
	vector <uint16_t> listErrorTouch24;
	vector <uint16_t> listErrorRgb4;
	vector <uint16_t> listErrorRgb34;
	vector <uint16_t> listErrorRgb234;

public:
	Gateway(string mac, int port, string ip, string localAddress, int localPort, string localClientid, string localUsername, string localPassword, int localKeepalive);
	~Gateway();
	void init();

	void InitSocketMessage();

	void PushLocalResult(Json::Value data);
	Json::Value SocketCmdStartCheck(Json::Value &dataRspLocal);
	Json::Value SocketCmdButtonCheck(Json::Value &dataRspLocal);
	void SocketSendFinish(int status);
	int OnSocketStart(Json::Value &reqValue, Json::Value &respValue);
	int OnSocketStop(Json::Value &reqValue, Json::Value &respValue);

	void InitMqttLocalMessage();
	int OnDeviceTest(Json::Value &reqValue, Json::Value &respValue);
	int OnStartScanBle(Json::Value &reqValue, Json::Value &respValue);
	int OnStopScanBle(Json::Value &reqValue, Json::Value &respValue);

	int pushDeviceUpdateLocal(Json::Value &dataValue);

	string getId();
	string getVersion();
	string getName();
	string getData();
	string getMac();

	void setId(string id);
	void setMac(string mac);
	void setVersion(string version);
	void setName(string name);
	void setData(string data);

	int checkTypeError(uint16_t devAddr);
};

extern Gateway *gateway;
