#include "System.h"
#include <windows.h>

float GetTime()
{
	SYSTEMTIME sys_time;
	float tmp;

	GetLocalTime(&sys_time);
	tmp = sys_time.wSecond;
	tmp = sys_time.wMinute + tmp/60.0f;
	tmp = sys_time.wHour + tmp/60.0f;

	return tmp;
}
