#pragma once
#include <utility>
#include <vector>
#include <string>
#include <map>

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

// lazy deletion
template<typename K, typename V>
class HashTableLinearProbingV2
{
public:
	HashTableLinearProbingV2()
	{
		vector<Element*> buckets(10, nullptr);
		_buckets = move(buckets);
	}
	~HashTableLinearProbingV2()
	{
		for (int i = 0; i < _buckets.size(); i++)
		{
			if (_buckets[i])
			{
				delete _buckets[i];
			}
		}
	}

	void insert(const pair<K,V>& p)
	{
		const K& k = p.first;
		const V& v = p.second;

		_rehashIfRequired();

		int hashIndex = hash(k);
		while (_buckets[hashIndex])
		{
			if (_buckets[hashIndex]->state == empty)
			{
				_createNewElement(hashIndex, k, v);
				return;
			}

			if (_buckets[hashIndex]->key == k) 
			{ 
				return; // element of the same key already exists.
			}
			hashIndex = (hashIndex + 1) % _buckets.size();
		}

		_createNewElement(hashIndex, k, v);
	}

	V* find(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			return &(element->value);
		}
		return nullptr;
	}

	void erase(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			_numberOfElements--;
			element->state = empty;
		}
	}

	int size()
	{
		return _numberOfElements;
	}

private:

	int hash(const int& key)
	{
		return key % _buckets.size();
	}

	enum State
	{
		empty, // bucket was filled before but now empty
		filled // bucket is filled
	};

	struct Element
	{
		K key;
		V value; 
		State state;
	};

	void _createNewElement(int hashIndex, const K& k, const V& v)
	{
		if (_buckets[hashIndex] == nullptr)
		{
			_buckets[hashIndex] = new Element();
		}
		_buckets[hashIndex]->key = k;
		_buckets[hashIndex]->value = v;
		_buckets[hashIndex]->state = filled;
		_numberOfElements++;
	}

	void _rehashIfRequired()
	{
		double loadFactor = (double)_numberOfElements / (double)_buckets.size();
		if (loadFactor > 0.8)
		{
			vector<Element*> oldBuckets = move(_buckets);
			_buckets = vector<Element*>(oldBuckets.size() * 2, nullptr);

			for (int i = 0; i < oldBuckets.size(); i++)
			{
				if (oldBuckets[i])
				{
					if (oldBuckets[i]->state == empty)
					{
						delete oldBuckets[i];
						oldBuckets[i] = nullptr;
					}
					else
					{
						Element* element = oldBuckets[i];
						int hashIndex = hash(element->key);
						while (_buckets[hashIndex])
						{
							hashIndex = (hashIndex + 1) % _buckets.size();
						}

						_buckets[hashIndex] = element;
					}
				}
			}
		}
	}

	Element* _find(const K& k)
	{
		int hashIndex = hash(k);
		while (_buckets[hashIndex])
		{
			if ((_buckets[hashIndex]->state == filled) && (_buckets[hashIndex]->key == k))
			{
				return _buckets[hashIndex];
			}
			hashIndex = (hashIndex + 1) % _buckets.size();
		}
		return nullptr;
	}

	int _numberOfElements = 0;
	vector<Element*> _buckets;
};

// bucket size must be prime and should alway be less than half fill to gurantee 
// probing doesn't fall into infinite loop. can be proven mathematical contradiction.
template <typename K, typename V>
class HashTableQuadricProbing
{
public:
	HashTableQuadricProbing()
	{
		_buckets = vector<Element*>(_getNextPrime(), nullptr);
	}
	~HashTableQuadricProbing()
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
		_rehashIfRequired();

		const K& k = p.first;
		const V& v = p.second;

		int i = _getHash(k);
		int q = 1;
		while (_buckets[i])
		{
			if(_buckets[i]->state == empty)
			{
				_createElement(i, k, v);
				return;
			}
			else
			{
				if (_buckets[i]->k == k)
				{
					return; // the element with the same key already exists.
				}
			}

			i = (_getHash(k) + (q*q)) % _buckets.size();
			q++;
		}
		_createElement(i, k, v);

	}
	V* find(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			return &(element->v);
		}
		return nullptr;
	}
	void erase(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			_numberOfElements--;
			element->state = empty;
		}
	}

	int size()
	{
		return _numberOfElements;
	}

private:
	enum State
	{
		empty,
		filled
	};
	struct Element
	{
		K k;
		V v;
		State state = filled;
	};

	int _getHash(const int& k)
	{
		return k % _buckets.size();
	}

	void _rehashIfRequired()
	{
		double loadFactor = (double)_numberOfElements / (double)_buckets.size();
		if (loadFactor >= 0.45)
		{
			vector<Element*> oldBuckets(move(_buckets));
			_buckets = vector<Element*>(_getNextPrime(), nullptr);

			for (int i = 0; i < oldBuckets.size(); i++)
			{
				if (oldBuckets[i])
				{
					if (oldBuckets[i]->state == empty)
					{
						delete oldBuckets[i];
						oldBuckets[i] = nullptr;
					}
					else
					{
						Element* element = oldBuckets[i];
						oldBuckets[i] = nullptr;

						int hashIndex = _getHash(element->k);
						int q = 1;
						while (_buckets[hashIndex])
						{
							hashIndex = (_getHash(element->k) + (q*q)) % _buckets.size();
							q++;
						}
						_buckets[hashIndex] = element;
					}
				}
			}

		}
	}

	Element* _find(const K& k)
	{
		int i = _getHash(k);
		int q = 1;
		while (_buckets[i])
		{
			if (_buckets[i]->state == filled && _buckets[i]->k == k)
			{
				return _buckets[i];
			}
			i = (_getHash(k) + (q*q)) % _buckets.size();
			q++;
		}

		return nullptr;
	}

	void _createElement(const int& i, const K& k, const V& v)
	{
		if (_buckets[i] == nullptr)
		{
			_buckets[i] = new Element();
		}
		_buckets[i]->k = k;
		_buckets[i]->v = v;
		_buckets[i]->state = filled;
		_numberOfElements++;
	}

	int _getNextPrime()
	{
		static int i = -1;
		static int primeNumbers[] = {31, 73, 127, 283, 467, 877, 1663, 3259, 6763, 12491, 25111, 51907, 100129};
		i++;
		if (i > 12) { throw "error... array out of bound"; }
		return primeNumbers[i];
	}

	vector<Element*> _buckets;
	int _numberOfElements = 0;

};


// bucket size must be prime, because prime number cannot be divded evenly.
// e.g bucket size odd 9, double hash 3
// e.g. bucket size even 4, double hash 2
// double hash function must not return 0. usually prime p < size. p - (key % p)
template <typename K, typename V>
class HashTableDoubleHashing
{
public:
	HashTableDoubleHashing()
	{
		_buckets = vector<Element*>(_getNextPrime(), nullptr);
	}
	~HashTableDoubleHashing()
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
		_rehashIfRequired();

		const K& k = p.first;
		const V& v = p.second;

		int i = _getHash(k);
		while (_buckets[i])
		{
			if (_buckets[i]->state == empty)
			{
				_createElement(i, k, v);
				return;
			}
			else
			{
				if (_buckets[i]->k == k)
				{
					return; // the element with the same key already exists.
				}
			}

			i = (i + _getDoubleHash(k)) % _buckets.size();
		}
		_createElement(i, k, v);

	}
	V* find(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			return &(element->v);
		}
		return nullptr;
	}
	void erase(const K& k)
	{
		Element* element = _find(k);
		if (element)
		{
			_numberOfElements--;
			element->state = empty;
		}
	}

	int size()
	{
		return _numberOfElements;
	}

private:
	enum State
	{
		empty,
		filled
	};
	struct Element
	{
		K k;
		V v;
		State state = filled;
	};

	int _getHash(const int& k)
	{
		return k % _buckets.size();
	}

	int _getDoubleHash(const int& k)
	{
		int prime = primeForDoubleHash[primeIndex];
		return (prime - (k % prime));
	}

	void _rehashIfRequired()
	{
		double loadFactor = (double)_numberOfElements / (double)_buckets.size();
		if (loadFactor >= 0.45)
		{
			vector<Element*> oldBuckets(move(_buckets));
			_buckets = vector<Element*>(_getNextPrime(), nullptr);

			for (int i = 0; i < oldBuckets.size(); i++)
			{
				if (oldBuckets[i])
				{
					if (oldBuckets[i]->state == empty)
					{
						delete oldBuckets[i];
						oldBuckets[i] = nullptr;
					}
					else
					{
						Element* element = oldBuckets[i];
						oldBuckets[i] = nullptr;

						int hashIndex = _getHash(element->k);
						while (_buckets[hashIndex])
						{
							hashIndex = (hashIndex + _getDoubleHash(element->k)) % _buckets.size();
						}
						_buckets[hashIndex] = element;
					}
				}
			}

		}
	}

	Element* _find(const K& k)
	{
		int i = _getHash(k);
		while (_buckets[i])
		{
			if (_buckets[i]->state == filled && _buckets[i]->k == k)
			{
				return _buckets[i];
			}
			i = (i + _getDoubleHash(k)) % _buckets.size();
		}

		return nullptr;
	}

	void _createElement(const int& i, const K& k, const V& v)
	{
		if (_buckets[i] == nullptr)
		{
			_buckets[i] = new Element();
		}
		_buckets[i]->k = k;
		_buckets[i]->v = v;
		_buckets[i]->state = filled;
		_numberOfElements++;
	}

	int _getNextPrime()
	{
		primeIndex++;
		if (primeIndex > 13) 
		{ 
			throw "error... array out of bound"; 
		}
		return primeNumbers[primeIndex];
	}

	vector<Element*> _buckets;
	int _numberOfElements = 0;

	int primeNumbers[13] = { 31, 73, 127, 283, 467, 877, 1663, 3259, 6763, 12491, 25111, 51907, 100129 };
	int primeForDoubleHash[13] = { 17, 31, 73, 127, 283, 467, 877, 1663, 3259, 6763, 12491, 25111, 51907};
	int primeIndex = -1;

};

template<typename K, typename V>
class HashTableChaining
{
public:
	HashTableChaining()
	{
		_buckets = vector<map<K, V>>(100);
	}
	~HashTableChaining()
	{

	}

	void insert(const pair<K, V>& p)
	{
		_rehashIfRequired();

		const K& k = p.first;
		int i = _getHash(k);
		map<K, V>& bucket = _buckets[i];
		auto iter = bucket.find(k);
		if (iter == bucket.end())
		{
			_numberOfElements++;
			bucket.insert(p);
		}
	}
	V* find(const K& k)
	{
		int i = _getHash(k);
		map<K, V>& bucket = _buckets[i];
		auto iter = bucket.find(k);
		if (iter != bucket.end())
		{
			return &(iter->second);
		}
	}
	void erase(const K& k)
	{
		int i = _getHash(k);
		map<K, V>& bucket = _buckets[i];
		auto iter = bucket.find(k);
		if (iter != bucket.end())
		{
			_numberOfElements--;
			bucket.erase(iter);
		}
	}

	int size()
	{
		return _numberOfElements;
	}
private:

	void _rehashIfRequired()
	{
		double loadFactor = (double)_numberOfElements / (double)_buckets.size();
		if (loadFactor > 0.5)
		{
			vector<map<K, V>> oldBuckets(move(_buckets));
			_buckets = vector<map<K, V>>(oldBuckets.size() * 2);

			for (int i = 0; i < oldBuckets.size(); i++)
			{
				map<K, V>& oldBucket = oldBuckets[i];
				for (auto iter = oldBucket.begin(); iter != oldBucket.end(); iter++)
				{
					const pair<K, V>& p = *iter;
					const K& k = p.first;

					int i = _getHash(k);
					map<K, V>& bucket = _buckets[i];
					bucket.insert(p);
				}
			}
		}
	}
	int _getHash(const int& k)
	{
		return k % _buckets.size();
	}

	int _numberOfElements = 0;
	vector<map<K, V>> _buckets;
};