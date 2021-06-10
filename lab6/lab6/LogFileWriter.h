#pragma once
#include "List.h"
#include <fstream>

class LogFileWriter
{
public:
	LogFileWriter(std::string fileName = "output.txt");
	void SaveData(List* data);

private:
	std::ofstream m_output;
};