#pragma once

#include <windows.h>
#include <string>

SOCKET ConnectToServer(void *world, std::string srv_ip);

DWORD WINAPI FindServer(void *param);
DWORD WINAPI Client(void *param);