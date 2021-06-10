#pragma once
#include <string>

class List
{
public:
	struct Node
	{
		int value;
		Node* next;
	};

	void Append(int value);
	int Pop();
	void Clear();
	int GetCount() { return m_count; }

	~List() { Clear(); }

private:
	Node* m_first = nullptr;
	int m_count = 0;
};