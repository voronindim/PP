#pragma once
#include "List.h"
#include <Windows.h>
#include "LogFileWriter.h"

class LogBuffer
{
public:
	LogBuffer();
	void Log(int value);
	~LogBuffer();

private:
	static DWORD WINAPI LogSizeMonitoringThread(CONST LPVOID lpParam);

	CRITICAL_SECTION m_criticalSection;
	List m_list;
	LogFileWriter m_fileWriter;

	HANDLE m_event;
	HANDLE m_logSizeMonitoringThread;
};