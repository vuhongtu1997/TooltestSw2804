#pragma once

#include <string>
#include <stdint.h>

using namespace std;

class Object
{
protected:
	string id;
	string name;
	uint16_t addr;

public:
	Object(string id, uint16_t addr, string name);
	~Object();

	string GetId();
	void SetId(string id);
	string GetName();
	void SetName(string name);
	uint16_t GetAddr();
	void SetAddr(uint16_t addr);
};
