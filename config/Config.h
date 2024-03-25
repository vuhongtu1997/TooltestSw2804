#pragma once

#include <stdint.h>
#include <string.h>

using namespace std;

class Config
{
private:

public:
	Config();
	void ReadConfig();
	void Print();
};

extern Config *config;
