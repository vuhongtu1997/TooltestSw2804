#include "Wifi.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <sys/socket.h>
#include "Log.h"
#include "Util.h"

#include "Gateway.h"

void Wifi::init()
{
}

string Wifi::GetMacAddress()
{
	LOGD("GetMacAddress");
	struct ifreq s;
	unsigned char *mac = NULL;
	char uc_Mac[100];
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
#ifdef ESP_PLATFORM
	strcpy(s.ifr_name, "enp1s0");
#else
	strcpy(s.ifr_name, "eth0");
#endif
	if (0 == ioctl(fd, SIOCGIFHWADDR, &s))
	{
		mac = (unsigned char *)s.ifr_addr.sa_data;
	}
	sprintf((char *)uc_Mac, (const char *)"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return string(uc_Mac);
}

string Wifi::GetIP()
{
	// LOGD("GetIP");
	string string_ip_1;
	string string_ip_2;
	string string_ip_3;
	string msg_rsp = Util::ExecuteCMD("ip -4 addr show ${link_name} | sed -Ene \'s/^.*inet ([0-9.]+)\\/.*$/\\1/p\'");
	// LOGV("IP list: %s", msg_rsp.c_str());
	vector<string> ipList = Util::splitString(msg_rsp, '\n');
	for (size_t i = 0; i < ipList.size(); i++)
	{
		if (ipList.at(i) != "127.0.0.1" && ipList.at(i) != "10.10.10.1")
		{
			return ipList.at(i);
		}
	}
	for (size_t i = 0; i < ipList.size(); i++)
	{
		if (ipList.at(i) != "127.0.0.1" && ipList.at(i) != "")
		{
			return ipList.at(i);
		}
	}
	return "";
}

// trim from start
static inline std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c)
																	{ return !std::isspace(c); }));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int c)
											 { return !std::isspace(c); })
							.base(),
					s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
	return ltrim(rtrim(s));
}

void Wifi::ScanWifi(Json::Value &data)
{
	size_t pos = 0;
	string wifi;
	vector<string> wifiDataList;
	char buff[128] = "Cell 02";
	int count = 0;
	vector<string> lineList;
	string ssidStr, encryptionStr;
	int quality;

	string msg_rsp = Util::ExecuteCMD("iwinfo wlan0 scan");
	while ((pos = msg_rsp.find(buff)) != string::npos)
	{
		wifi = msg_rsp.substr(0, pos);
		msg_rsp.erase(0, pos + 19);
		wifiDataList.push_back(wifi);
		++count;
		sprintf(buff, "Cell %02d", count + 2);
	}
	for (auto &wifiData : wifiDataList)
	{
		lineList = Util::splitString(wifiData, '\n');
		if (lineList.size() == 5 && (lineList[1].find("\"") != string::npos))
		{
			ssidStr = trim(lineList[1]);
			ssidStr.erase(0, 8);
			ssidStr.erase(ssidStr.length() - 1);

			encryptionStr = trim(lineList[4]);
			encryptionStr.erase(0, 12);

			char *p;
			p = (char *)strstr(lineList[3].c_str(), "Quality");
			char qlt[3] = {(*(p + 9)), *(p + 10)};
			quality = stoi(string(qlt));
			if (quality >= 50)
			{
				Json::Value wifiValue;
				wifiValue["SSID"] = ssidStr;
				wifiValue["QUALITY"] = quality;
				wifiValue["MAC"] = lineList[0];
				wifiValue["ENCRYPTION"] = encryptionStr;
				data.append(wifiValue);
			}
		}
	}
}

int Wifi::ConnectToWifi(string ssid, string password, string encryption)
{
	LOGD("Connect to Wifi");
	string encry = "none";
	if (encryption.find("OWE") != string::npos)
	{
		encry = "owe";
	}
	if (encryption.find("none") != string::npos)
	{
		encry = "none";
	}
	if (encryption.find("WPA") != string::npos && encryption.find("PSK") != string::npos)
	{
		encry = "psk";
	}
	if (encryption.find("WPA2") != string::npos && encryption.find("PSK") != string::npos)
	{
		encry = "psk2";
	}
	if (encryption.find("WPA3") != string::npos && encryption.find("SAE") != string::npos)
	{
		encry = "sae";
	}
	if (encryption.find("mixed") != string::npos && encryption.find("WPA/WPA2") != string::npos && encryption.find("PSK") != string::npos)
	{
		encry = "psk-mixed";
	}
	LOGD("Encryption: %s", encry.c_str());
	try
	{
		system("rm /output.txt");
		system("uci del network.wan.ifname >> /output.txt 2>&1");
		system("uci del wireless.wifinet1  >> /output.txt 2>&1");
		system("uci set wireless.wifinet1=wifi-iface >> /output.txt 2>&1");
		system(string("uci set wireless.wifinet1.ssid=\"" + ssid + "\" >> /output.txt 2>&1").c_str());
		system("uci set wireless.wifinet1.mode='sta' >> /output.txt 2>&1");
		system("uci set wireless.wifinet1.network='wan' >> /output.txt 2>&1");
		system("uci set wireless.wifinet1.device='radio0' >> /output.txt 2>&1");
		system(string("uci set wireless.wifinet1.key='" + password + "' >> /output.txt 2>&1").c_str());
		system(string("uci set wireless.wifinet1.encryption='" + encry + "' >> /output.txt 2>&1").c_str());
		system("uci commit wireless >> /output.txt 2>&1");
		system("uci commit network >> /output.txt 2>&1");
		system("wifi >> /output.txt 2>&1");
		system("/etc/init.d/network restart");
	}
	catch (...)
	{
	}
	sleep(30);
	string ip = GetIP();
	LOGI("GW ip: %s", GetIP().c_str());
	if (ip == "10.10.10.1")
	{
		system("uci del wireless.wifinet1 >> /output.txt 2>&1");
		system("uci set wireless.wifinet1=wifi-iface >> /output.txt 2>&1");
		// ExecuteCMD("uci set wireless.default_radio0.mode='ap'");
		system("uci commit wireless");
		system("uci commit network");
		system("wifi");
		system("/etc/init.d/network restart");
		return CODE_ERROR;
	}
	return CODE_OK;
}

int Wifi::SetModeApWifi()
{
	system("rm /output.txt");
	system("uci set network.wan.ifname='eth0' >> /output.txt 2>&1");
	system("uci commit network >> /output.txt 2>&1");
	system("/etc/init.d/network restart >> /output.txt 2>&1");
	system("uci del wireless.wifinet1 >> /output.txt 2>&1");
	system("uci commit wireless >> /output.txt 2>&1");
	system("wifi >> /output.txt 2>&1");
	return CODE_OK;
}

bool Wifi::WifiIsAPMode(void)
{
	return false;
	// return wifiMode == WIFI_MODE_APSTA;
}

bool Wifi::WifiIsStaMode(void)
{
	return false;
	// return wifiMode == WIFI_MODE_AP;
}
