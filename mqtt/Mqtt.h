#pragma once

#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <mosquittopp.h>

#include "ActionCallback.h"
#include "ErrorCode.h"

using namespace std;

enum mqtt_err_t
{
	MQTT_ERR_AUTH_CONTINUE = -4,
	MQTT_ERR_NO_SUBSCRIBERS = -3,
	MQTT_ERR_SUB_EXISTS = -2,
	MQTT_ERR_CONN_PENDING = -1,
	MQTT_ERR_SUCCESS = 0,
	MQTT_ERR_NOMEM = 1,
	MQTT_ERR_PROTOCOL = 2,
	MQTT_ERR_INVAL = 3,
	MQTT_ERR_NO_CONN = 4,
	MQTT_ERR_CONN_REFUSED = 5,
	MQTT_ERR_NOT_FOUND = 6,
	MQTT_ERR_CONN_LOST = 7,
	MQTT_ERR_TLS = 8,
	MQTT_ERR_PAYLOAD_SIZE = 9,
	MQTT_ERR_NOT_SUPPORTED = 10,
	MQTT_ERR_AUTH = 11,
	MQTT_ERR_ACL_DENIED = 12,
	MQTT_ERR_UNKNOWN = 13,
	MQTT_ERR_ERRNO = 14,
	MQTT_ERR_EAI = 15,
	MQTT_ERR_PROXY = 16,
	MQTT_ERR_PLUGIN_DEFER = 17,
	MQTT_ERR_MALFORMED_UTF8 = 18,
	MQTT_ERR_KEEPALIVE = 19,
	MQTT_ERR_LOOKUP = 20,
	MQTT_ERR_MALFORMED_PACKET = 21,
	MQTT_ERR_DUPLICATE_PROPERTY = 22,
	MQTT_ERR_TLS_HANDSHAKE = 23,
	MQTT_ERR_QOS_NOT_SUPPORTED = 24,
	MQTT_ERR_OVERSIZE_PACKET = 25,
	MQTT_ERR_OCSP = 26,
	MQTT_ERR_TIMEOUT = 27,
};

#define MAX_BUFFER_SIZE 100
class MQTTPubSub
{
private:
	bool state;

public:
	int id;
	void setState(bool state_) { state = state_; }
	bool getState() { return state; }
};

class Mqtt : public mosqpp::mosquittopp
{
public:
	Mqtt(string host, int port, string client_id, string username, string password, int keepalive, char *cert = NULL, string willset_topic = "", string willset_payload = "");
	virtual ~Mqtt();

	void init();

	void SetServer(string host, int port, string client_id, string username, string password, int keepalive);
	void SetWillset(string willset_topic, string willset_payload);
	int Connect();
	int Reconnect();
	void SubscribeList();
	int Subscribe(string topic, int maxTime = 5, int duration = 5);
	int Unsubscribe(string topic, int maxTime = 5, int duration = 5);
	int Publish(string topic, string payload);
	int Publish(string topic, const char *payload, int payloadLen);
	bool isConnected();
	int removeObjectFromVector(vector<MQTTPubSub *> *mqttPubSubs, MQTTPubSub *mqttPubSub);

	virtual void OnConnect(bool isConnected, bool isReconnect) {}
	virtual void OnMessage(string topic, char *payload, int payloadLen);

	void addActionCallback(ActionCallbackFuncType1 actionCallbackFuncType1, string topic);
	void addActionCallback(ActionCallbackFuncType2 actionCallbackFuncType2, string topic);
	void addActionCallback(ActionCallbackFuncType3 actionCallbackFuncType3, string topic);
	void addActionCallback(ActionCallbackFuncType4 actionCallbackFuncType4, string topic);
	int findActionCallbackFuncFromTopic(string topic, ActionCallback **actionCallback);

protected:
	string host;
	int port;
	string client_id;
	string username;
	string password;
	int keepalive;
	char *cert;
	string willset_topic;
	string willset_payload;
	volatile bool connected;
	volatile bool reconnected;
	mutex mtx;

	vector<MQTTPubSub *> mqttPublishs;
	vector<MQTTPubSub *> mqttSubscribes;
	vector<MQTTPubSub *> mqttUnsubscribes;
	vector<ActionCallback> actionCallbacks;
	vector<string> topicList;

	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_publish(int mid);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
	void on_unsubscribe(int mid);
	void on_message(const struct mosquitto_message *message);
};
