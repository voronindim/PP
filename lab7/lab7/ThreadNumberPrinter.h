#pragma once
#include "ITask.h"

class ThreadNumberPrinter : public ITask
{
public:
	ThreadNumberPrinter(int number);
	void Execute() override;

private:
	int m_number;
};