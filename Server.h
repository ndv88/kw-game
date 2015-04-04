#pragma once

#include <windows.h>

DWORD WINAPI WaitClients(void *param);
DWORD WINAPI Accept(void *param);
DWORD WINAPI Server(void *param);