#include <Windows.h>
#include <time.h>
#include <random>
#include <algorithm>
#include <utility>

#include "HashTables.h"

void TestHashTableLinearProbing()
{
	HashTableLinearProbing<int, string> hashTable;

	srand(time(NULL));
	for (int i = 0; i < 1000; i++)
	{
		unsigned int randomNumber = rand() % 1000;
		string data = "[[" + to_string(randomNumber) + "]]";

		hashTable.insert(make_pair(randomNumber, data));
		hashTable.insert(make_pair(randomNumber, data));

		if (hashTable.find(randomNumber) == nullptr)
		{
			DebugBreak();
		}
	}

	for (int i = 0; i < 1000; i++)
	{
		if (hashTable.find(i) != nullptr)
		{
			hashTable.erase(i);
		}
		if (hashTable.find(i) != nullptr)
		{
			DebugBreak();
		}
	}

	if (hashTable.size() != 0)
	{
		DebugBreak();
	}

}

int main()
{
	TestHashTableLinearProbing();

	return 0;
}