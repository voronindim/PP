#include "ThreadNumberPrinter.h"
#include <iostream>
#include <string>

ThreadNumberPrinter::ThreadNumberPrinter(int number)
	: m_number(number)
{
}

void ThreadNumberPrinter::Execute()
{
	std::cout << "thread number " + std::to_string(m_number) + " is working\n";
}