#include <Windows.h>
#include <time.h>
#include <random>
#include <algorithm>
#include <utility>
#include <chrono>


#include "HashTables.h"

template<typename T>
void TestHashTable(T& hashTable)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


	unsigned int RANGE = 1000;

	srand(time(NULL));
	for (int i = 0; i < RANGE; i++)
	{
		unsigned int randomNumber = rand() % RANGE;
		string data = "[[" + to_string(randomNumber) + "]]";

		hashTable.insert(make_pair(randomNumber, data));
		hashTable.insert(make_pair(randomNumber, data));

		if (hashTable.find(randomNumber) == nullptr)
		{
			DebugBreak();
		}
	}

	for (int i = 0; i < RANGE; i++)
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

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

int main()
{
	{
		printf("[HashTableLinearProbing] \t\t");
		HashTableLinearProbing<int, string> hashTable;
		TestHashTable(hashTable);
	}

	{
		printf("[HashTableLinearProbingV2] \t\t");
		HashTableLinearProbingV2<int, string> hashTable;
		TestHashTable(hashTable);
	}

	{
		printf("[HashTableQuadricProbing] \t\t");
		HashTableQuadricProbing<int, string> hashTable;
		TestHashTable(hashTable);
	}

	{
		printf("[HashTableDoubleHashing] \t\t");
		HashTableDoubleHashing<int, string> hashTable;
		TestHashTable(hashTable);
	}


	{
		printf("[HashTableChaining] \t\t\t");
		HashTableChaining<int, string> hashTable;
		TestHashTable(hashTable);
	}


	return 0;
}