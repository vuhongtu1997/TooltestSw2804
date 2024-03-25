#include "LocalProtocol.h"
#include <string.h>
#include <unistd.h>
#include "Log.h"
#include "Util.h"
#include "Wifi.h"

LocalProtocol::LocalProtocol(string mac, string address, int port, string client_id, string username, string password, int keepalive)
	: Mqtt(address, port, client_id, username, password, keepalive)
{
	this->mac = mac;
	subReqTopic = "/tooltest/app/hc/json_req";
	subRespTopic = "/tooltest/app/hc/json_resp";
	pubReqTopic = "/tooltest/hc/app/json_req";
	pubRespTopic = "/tooltest/hc/app/json_resp";
}

LocalProtocol::~LocalProtocol()
{
}

void LocalProtocol::init()
{

	Mqtt::init();
	isBusy = false;
	addActionCallback(bind(&LocalProtocol::OnLocalReq, this, placeholders::_1, placeholders::_2), subReqTopic);
	addActionCallback(bind(&LocalProtocol::OnLocalResp, this, placeholders::_1, placeholders::_2), subRespTopic);
}

void LocalProtocol::localAddActionCallback(ActionCallbackFuncType1 actionCallbackFuncType1, string topic)
{
	addActionCallback(actionCallbackFuncType1, topic);
}

void LocalProtocol::localAddActionCallback(ActionCallbackFuncType2 actionCallbackFuncType2, string topic)
{
	addActionCallback(actionCallbackFuncType2, topic);
}

void LocalProtocol::localAddActionCallback(ActionCallbackFuncType3 actionCallbackFuncType3, string topic)
{
	addActionCallback(actionCallbackFuncType3, topic);
}

void LocalProtocol::localAddActionCallback(ActionCallbackFuncType4 actionCallbackFuncType4, string topic)
{
	addActionCallback(actionCallbackFuncType4, topic);
}

int LocalProtocol::LocalConnect()
{
	return Connect();
}

void LocalProtocol::OnConnect(bool isConnected, bool isReconnect)
{
	OnLocalConnect(isConnected, isReconnect);
}

void LocalProtocol::OnLocalReq(string &topic, string &payload)
{
	Json::Value respValue;
	Json::Value payloadJson;
	vector<string> topics = Util::splitString(topic, '/');
	// if (topics.size() == 7)
	// {
	// 	if (topics[5] == mac || topics[5] == "all")
	// 	{
	Util::LedServiceLock();
	if (payloadJson.parse(payload) && payloadJson.isObject() &&
		payloadJson.isMember("cmd") && payloadJson["cmd"].isString() &&
		payloadJson.isMember("rqi") && payloadJson["rqi"].isString() &&
		payloadJson.isMember("data") && payloadJson["data"].isObject())
	{
		string cmd = payloadJson["cmd"].asString();
		string rqi = payloadJson["rqi"].asString();
		if (onLocalCallbackFuncList.find(cmd) != onLocalCallbackFuncList.end())
		{
			OnLocalCallbackFunc onLocalCallbackFunc = onLocalCallbackFuncList[cmd];
			isBusy = true;
			int rs = onLocalCallbackFunc(payloadJson["data"], respValue);
			isBusy = false;
			if (rs == CODE_OK)
			{
				LOGD("Call %s OK, rs: %d", cmd.c_str(), rs);
				respValue["rqi"] = rqi;
				LOGD("local publish: %s: %s", (pubRespTopic + topics[2] + "/json_resp").c_str(), respValue.toString().c_str());
				Publish(pubRespTopic + topics[2] + "/json_resp", respValue.toString());
			}
			else if (rs == CODE_DATA_ARRAY)
			{
				LOGD("Call %s OK, rs: %d", cmd.c_str(), rs);
				if (respValue.isArray())
				{
					for (auto &respV : respValue)
					{
						respV["rqi"] = rqi;
						Publish(pubRespTopic + topics[2] + "/json_resp", respV.toString());
					}
				}
			}
			else if (rs == CODE_NOT_RESPONSE)
			{
				LOGD("Call %s OK, rs: %d", cmd.c_str(), rs);
			}
			else if (rs == CODE_EXIT)
			{
				LOGD("Call %s OK, rs: %d", cmd.c_str(), rs);
				respValue["rqi"] = rqi;
				LOGD("local publish: %s: %s", (pubRespTopic + topics[2] + "/json_resp").c_str(), respValue.toString().c_str());
				Publish(pubRespTopic + topics[2] + "/json_resp", respValue.toString());
				sleep(2);
				exit(1);
			}
			else
			{
				LOGW("Call %s ERR rs: %d", cmd.c_str(), rs);
			}
		}
		else
		{
			LOGW("Method %s not registed", cmd.c_str());
			LOGW("OnLocalMessage payload: %s", payload.c_str());
		}
	}
	else
	{
		LOGW("OnLocalMessage topic: %s", topic.c_str());
		LOGW("OnLocalMessage payload: %s", payload.c_str());
	}
	// 	}
	// }

	Util::LedServiceUnlock();
}

void LocalProtocol::OnLocalResp(string &topic, string &payload)
{
	Json::Value respValue;
	Json::Value payloadJson;
	vector<string> topics = Util::splitString(topic, '/');
	// if (topics.size() == 7)
	// {
	// 	if (topics[5] == mac || topics[5] == "all")
	// 	{
	Util::LedServiceLock();
	if (payloadJson.parse(payload) && payloadJson.isObject() &&
		payloadJson.isMember("cmd") && payloadJson["cmd"].isString() &&
		payloadJson.isMember("rqi") && payloadJson["rqi"].isString())
	{
		string cmd = payloadJson["cmd"].asString();
		string rqi = payloadJson["rqi"].asString();
		if (requestList.find(rqi) != requestList.end())
		{
			request_t *request = requestList[rqi];
			if (cmd == request->respCmd)
			{
				request->status = true;
				if (request->respValue && payloadJson.isMember("data") && payloadJson["data"].isObject())
				{
					*request->respValue = payloadJson["data"];
				}
			}
		}
		else
		{
			LOGW("rqi %s not found", rqi.c_str());
			LOGW("OnLocalResp payload: %s", payload.c_str());
		}
	}
	else
	{
		LOGW("OnLocalResp topic: %s", topic.c_str());
		LOGW("OnLocalResp payload: %s", payload.c_str());
	}
	// 	}
	// }
	Util::LedServiceUnlock();
}

int LocalProtocol::OnLocalCallbackRegister(string cmd, OnLocalCallbackFunc onLocalCallbackFunc)
{
	LOGI("OnLocalCallbackRegister cmd: %s", cmd.c_str());
	onLocalCallbackFuncList[cmd] = onLocalCallbackFunc;
	return CODE_OK;
}

int LocalProtocol::LocalPublish(string topic, string payload)
{
	return Publish(topic, payload);
}

int LocalProtocol::LocalPublish(string topic, char *payload, int payloadLen)
{
	return Publish(topic, payload, payloadLen);
}

int LocalProtocol::LocalPublish(string &payload)
{
	return Publish(pubReqTopic, payload);
}

int LocalProtocol::LocalPublish(Json::Value &payloadJson)
{
	return Publish(pubReqTopic, payloadJson.toString());
}

int LocalProtocol::PublishToLocalMessage(string reqCmd, Json::Value &reqValue, string respCmd, Json::Value *respValue, uint32_t timeout)
{
	LOGD("PublishToLocalMessage: %s", reqValue.toString().c_str());
	if (!connected)
		return CODE_TIMEOUT;
	int rs = CODE_OK;
	Json::Value sendValue;
	string rqi = Util::genRandRQI(16);
	sendValue["data"] = reqValue;
	sendValue["rqi"] = rqi;
	sendValue["cmd"] = reqCmd;
	request_t request = {
		.status = false,
		.respCmd = respCmd,
		.respValue = respValue,
	};
	requestList[rqi] = &request;
	Publish(pubReqTopic, sendValue.toString());
	while (!request.status && timeout--)
	{
		usleep(1000);
	}
	if (!request.status)
	{
		rs = CODE_ERROR;
	}
	requestList.erase(rqi);
	LOGD("PublishToLocalMessage rs: %d", rs);
	return rs;
}

int LocalProtocol::PublishToLocalMessage(string &payload)
{
	return Publish(pubReqTopic, payload);
}

int LocalProtocol::PublishToLocalMessage(Json::Value &payloadJson)
{
	return Publish(pubReqTopic, payloadJson.toString());
}
