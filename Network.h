#pragma once

#include <windows.h>
#include <string>
#include "Options.h"

#pragma comment(lib, "ws2_32.lib")

struct NET_DATA
{
	SOCKET sock;
	void *data;
};

enum CMDS
{
	CONNECT = 1,
	CLIENT_DISCONNECT = 2,
	SRV_DISCONNECT = 3,
	CLIENT_UPDATE = 4
};

const char FIND_SRV[] = "FIND_SERVER_KW";
const char SRV_STR[] = "I_AM_SERVER_KW";

//int max_clients = 8;


