#pragma once

#include <functional>
#include "json.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ErrorCode.h"

using namespace std;

class SocketProtocol
{
private:
	typedef function<int(Json::Value &reqValue, Json::Value &respValue)> OnRpcCallbackFunc;
	map<string, OnRpcCallbackFunc> onRpcCallbackFuncList;

public:
	int fd;
	int port;
	string ip;
	bool isRunning;

	SocketProtocol(int port, string ip);

	void init();
	void stop();

	int SocketCmdCallbackRegister(string cmd, OnRpcCallbackFunc onRpcCallbackFunc);
	void SocketOnMessage(string message);
	int sendMessage(string message);
};
