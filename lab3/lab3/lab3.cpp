#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <tchar.h>
#include <vector>
#include <windows.h>

unsigned int startTime;

struct Params
{
	int number;
	std::ofstream* out;
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	auto params = static_cast<Params*>(lpParam);
	for (int i = 0; i < 20; i++)
	{
		auto x = 0;
		for (int j = 0; j < 1'000'000; j++)
		{
			x += 2 * 11;
		}
		*params->out << (std::to_string(clock() - startTime) + "\n");
	}


	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int x;
	std::cin >> x;
	startTime = clock();
	if (argc != 2)
	{
		return 1;
	}
	int numberOfThreads = std::stoi(argv[1]);

	HANDLE* handles = new HANDLE[numberOfThreads];
	std::ofstream* files = new std::ofstream[numberOfThreads];
	std::vector<Params> arrayParams;

	for (int i = 0; i < numberOfThreads; i++)
	{
		files[i] = std::ofstream("out" + std::to_string(i) + ".txt");
		Params params;
		params.out = &files[i];
		params.number = i;

		arrayParams.push_back(params);
	}
	for (int i = 0; i < numberOfThreads; i++)
	{
		handles[i] = CreateThread(NULL, 0, &ThreadProc, &arrayParams[i], CREATE_SUSPENDED, NULL);
		ResumeThread(handles[i]);
	}
	WaitForMultipleObjects(numberOfThreads, handles, true, INFINITE);

	return 0;
}