#include "Bitmap.h"
#include <Windows.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "LogBuffer.h"

unsigned int startTime;

struct Params
{
	Bitmap* in;
	uint32_t startHeight;
	uint32_t endHeight;
	uint32_t startWidth;
	uint32_t endWidth;
	LogBuffer* logBuffer;
};

void Blur(int radius, Params* params)
{
	auto rs = ceil(radius * 2.57);
	for (auto i = params->startHeight; i < params->endHeight; ++i)
	{
		for (auto j = params->startWidth; j < params->endWidth; ++j)
		{
			double r = 0, g = 0, b = 0;
			double count = 0;

			for (auto iy = i - rs; iy < i + rs + 1; ++iy)
			{
				for (auto ix = j - rs; ix < j + rs + 1; ++ix)
				{
					auto x = min(static_cast<int>(params->endWidth) - 1, max(0, ix));
					auto y = min(static_cast<int>(params->endHeight) - 1, max(0, iy));

					long long dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
					double wght = std::exp(-dsq / (2.0 * radius * radius)) / (3.14 * 2.0 * radius * radius);

					rgb32* pixel = params->in->GetPixel(x, y);

					r += pixel->r * wght;
					g += pixel->g * wght;
					b += pixel->b * wght;
					count += wght;
				}
			}

			rgb32* pixel = params->in->GetPixel(j, i);
			pixel->r = std::round(r / count);
			pixel->g = std::round(g / count);
			pixel->b = std::round(b / count);

			params->logBuffer->Log(clock() - startTime);
		}
	}
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	struct Params* params = (struct Params*)lpParam;
	Blur(2, params);
	ExitThread(0); // функция устанавливает код завершения потока в 0
}

void ThreadsRunner(Bitmap* in, int threadsCount, int coreCount, const std::vector<int>& priorities)
{
	LogBuffer logBuffer;

	int partHeight = in->GetHeight() / threadsCount;
	int heightRemaining = in->GetHeight() % threadsCount;

	Params* arrayParams = new Params[threadsCount];

	for (int i = 0; i < threadsCount; i++)
	{
		Params params;
		params.in = in;
		params.startWidth = 0;
		params.endWidth = in->GetWidth();
		params.startHeight = partHeight * i;
		params.endHeight = partHeight * (i + 1) + ((i == threadsCount - 1) ? heightRemaining : 0);
		params.logBuffer = &logBuffer;

		arrayParams[i] = params;
	}

	// создание потоков
	HANDLE* handles = new HANDLE[threadsCount];
	for (int i = 0; i < threadsCount; i++)
	{
		handles[i] = CreateThread(NULL, i, &ThreadProc, &arrayParams[i], CREATE_SUSPENDED, NULL);
		SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
		SetThreadPriority(handles[i], priorities.at(i));
	}

	// запуск потоков
	for (int i = 0; i < threadsCount; i++)
	{
		ResumeThread(handles[i]);
	}

	// ожидание окончания работы потоков
	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

	delete[] arrayParams;
	delete[] handles;
}

int main(int argc, const char** argv)
{
	unsigned int start = clock();

	if (argc > 5 && argc != atoi(argv[3]) + 5)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "1 argument - in file" << std::endl;
		std::cout << "2 argument - out file" << std::endl;
		std::cout << "3 argument - threads count" << std::endl;
		std::cout << "4 argument - core count" << std::endl;
		std::cout << "5 argument - array of priorities" << std::endl;
		std::cout << "Example: in.bmp out.bmp 3 1 2 0 1" << std::endl;

		return 1;
	}

	Bitmap bmp{ argv[1] };

	int numberOfThreads = atoi(argv[3]);
	std::vector<int> priorities;
	for (int i = 0; i < numberOfThreads; ++i)
	{
		priorities.push_back(atoi(argv[5 + i]));
	}
	ThreadsRunner(&bmp, numberOfThreads, atoi(argv[4]), priorities);

	bmp.Save(argv[2]);

	unsigned int end = clock();
	unsigned int duration = end - start;
	std::cout << duration << std::endl;

	return 0;
}