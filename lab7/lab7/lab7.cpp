#include "ThreadNumberPrinter.h"
#include "Worker.h"
#include <iostream>
#include <string>

int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		return 1;
	}
	int numberOfThreads = std::stoi(argv[1]);

	IWorker* worker = new Worker();

	for (int i = 0; i < numberOfThreads; i++)
	{
		ITask* task = new ThreadNumberPrinter(i);
		Sleep(1000); 
		worker->ExecuteTask(task);
	}

	delete worker;
}