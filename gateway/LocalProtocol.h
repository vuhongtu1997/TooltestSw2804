#pragma once

#include "json.h"
#include <map>
#include <atomic>
#include "Mqtt.h"

using namespace std;

class LocalProtocol : public Mqtt
{
private:
	typedef struct
	{
		bool status;
		string respCmd;
		Json::Value *respValue;
	} request_t;
	map<string, request_t *> requestList;

	string mac;

	string subReqTopic;
	string subRespTopic;
	string pubReqTopic;
	string pubRespTopic;

	atomic<bool> isBusy;

	typedef function<int(Json::Value &reqValue, Json::Value &respValue)> OnLocalCallbackFunc;
	map<string, OnLocalCallbackFunc> onLocalCallbackFuncList;

	void OnLocalMessage(string &topic, string &payload);
	void OnLocalReq(string &topic, string &payload);
	void OnLocalResp(string &topic, string &payload);

public:
	LocalProtocol(string mac, string address, int port, string client, string username, string password, int keepalive);
	virtual ~LocalProtocol();

	void init();
	void localAddActionCallback(ActionCallbackFuncType1 actionCallbackFuncType1, string topic);
	void localAddActionCallback(ActionCallbackFuncType2 actionCallbackFuncType2, string topic);
	void localAddActionCallback(ActionCallbackFuncType3 actionCallbackFuncType3, string topic);
	void localAddActionCallback(ActionCallbackFuncType4 actionCallbackFuncType4, string topic);

	bool IsBusy() { return isBusy; }

	int LocalConnect();
	void OnConnect(bool isConnected, bool isReconnect);
	virtual void OnLocalConnect(bool isConnected, bool isReconnect) {}

	int OnLocalCallbackRegister(string cmd, OnLocalCallbackFunc onLocalCallbackFunc);

	int LocalPublish(string topic, string payload);
	int LocalPublish(string topic, char *payload, int payloadLen);
	int LocalPublish(string &payload);
	int LocalPublish(Json::Value &payloadJson);

	int PublishToLocalMessage(string &payload);
	int PublishToLocalMessage(Json::Value &payloadJson);
	int PublishToLocalMessage(string reqCmd, Json::Value &reqValue, string respCmd, Json::Value *respValue, uint32_t timeout = 1000);
};