#include "LogBuffer.h"

constexpr int MAX_BUFFER_COUNT = 1000;

LogBuffer::LogBuffer()
{
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, 1);
	m_event = CreateEvent(nullptr, TRUE, FALSE, TEXT("Buffer is over"));
	m_logSizeMonitoringThread = CreateThread(NULL, 0, &LogSizeMonitoringThread, this, 0, NULL);
}

LogBuffer::~LogBuffer()
{
	DeleteCriticalSection(&m_criticalSection);
}

void LogBuffer::Log(int value)
{
	EnterCriticalSection(&m_criticalSection);
	if (m_list.GetCount() > MAX_BUFFER_COUNT)
	{
		SetEvent(m_event);
		DWORD wait = WaitForSingleObject(m_logSizeMonitoringThread, INFINITE);
		ResetEvent(m_event);
		m_logSizeMonitoringThread = CreateThread(NULL, 0, &LogSizeMonitoringThread, this, 0, NULL);
	}

	m_list.Append(value);

	LeaveCriticalSection(&m_criticalSection);
}

DWORD WINAPI LogBuffer::LogSizeMonitoringThread(CONST LPVOID lpParam)
{
	auto logBuffer = (LogBuffer*)lpParam;

	DWORD wait = WaitForSingleObject(logBuffer->m_event, INFINITE);

	if (wait == WAIT_OBJECT_0)
	{
		logBuffer->m_fileWriter.SaveData(&logBuffer->m_list);
	}

	ExitThread(0);
}