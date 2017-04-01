#pragma once
#include <utility>
#include <vector>
#include <string>

using namespace std;

template<typename K, typename V>
class HashTableLinearProbing
{
public:
	HashTableLinearProbing() 
	{
		// buckets size should really always be prime numbers, but not doing it for brevity
		vector<pair<K, V>*> buckets(10, nullptr);
		_buckets = move(buckets);
	}
	~HashTableLinearProbing()
	{
		for (int i = 0; i < _buckets.size(); i++)
		{
			if (_buckets[i])
			{
				delete _buckets[i];
			}
		}
	}

	void insert(const pair<K, V>& p) 
	{
		rehashIfRequired();

		unsigned int hashValue = hash(p.first);
		while (_buckets[hashValue] != nullptr)
		{
			if (_buckets[hashValue]->first == p.first)
			{
				// the element already exists.
				return;
			}
			hashValue = (hashValue + 1) % _buckets.size();
		}

		_buckets[hashValue] = new pair<K, V>(p.first, p.second);
		_elements++;
	}

	V* find(const K& k) 
	{
		unsigned int hashValue = hash(k);
		while (_buckets[hashValue] != nullptr)
		{
			if (_buckets[hashValue]->first == k)
			{
				return &(_buckets[hashValue]->second);
			}
			hashValue = (hashValue + 1) % _buckets.size();
		}
		return nullptr;
	}

	void erase(const K& k)
	{
		unsigned int hashValue = hash(k);
		while (_buckets[hashValue] != nullptr)
		{
			if (_buckets[hashValue]->first == k)
			{
				_elements--;
				delete _buckets[hashValue];
				_buckets[hashValue] = nullptr;
				shift(hash(k), hashValue, (hashValue + 1) % _buckets.size());
			}
			hashValue = (hashValue + 1) % _buckets.size();
		}
	}

	unsigned int size()
	{
		return _elements;
	}


private:

	template<typename K>
	unsigned int hash(const K& k) 
	{
		return 0;
	}
	template<>
	unsigned int hash(const unsigned int& k)
	{
		return k % _buckets.size();
	}
	template<>
	unsigned int hash(const string& k)
	{
		unsigned int hashValue = 0;
		for (int i = 0; i < k.size(); i++)
		{
			hashValue += ((unsigned int)k[i] * 31);
		}
		return hashValue % _buckets.size();
	}

	void shift(unsigned int hashValue, unsigned int deletedPosition, unsigned int currentPosition)
	{
		if (_buckets[currentPosition] == nullptr)
		{
			return;
		}
		else
		{
			if (hash(_buckets[currentPosition]->first) <= hashValue)
			{
				_buckets[deletedPosition] = _buckets[currentPosition];
				_buckets[currentPosition] = nullptr;
				shift(hashValue, currentPosition, (currentPosition + 1) % _buckets.size());
			}
			else
			{
				shift(hashValue, deletedPosition, (currentPosition + 1) % _buckets.size());
			}
		}
	}

	void rehashIfRequired() 
	{
		double loadFactor = (double)_elements / (double)_buckets.size();
		if (loadFactor > 0.8)
		{
			vector<pair<K, V>*> newBuckets(_buckets.size() * 2, nullptr);
			vector<pair<K, V>*> oldBuckets(move(_buckets));

			_buckets = move(newBuckets);

			for (int i = 0; i < oldBuckets.size(); i++)
			{
				if (oldBuckets[i])
				{
					pair<K, V>* p = oldBuckets[i];
					unsigned int hashValue = hash(p->first);
					while (_buckets[hashValue] != nullptr)
					{
						hashValue = (hashValue + 1) % _buckets.size();
					}

					_buckets[hashValue] = p;
				}
			}
		}
	}


	vector<pair<K, V>*> _buckets;
	
	unsigned int _elements = 0;
};

class HashTableQuadricProbing
{

};

class HashTableDoubleHashing
{

};

class HashTableChaining
{

};