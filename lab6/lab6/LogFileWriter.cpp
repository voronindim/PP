#include "LogFileWriter.h"

LogFileWriter::LogFileWriter(std::string fileName)
	: m_output{ fileName }
{
}

void LogFileWriter::SaveData(List* data)
{
	while (data->GetCount() > 0)
	{
		m_output << data->Pop() << std::endl;
	}
	m_output.flush();
}