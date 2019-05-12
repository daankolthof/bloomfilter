#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <bitset>
#include <chrono>
#include <random>

template <typename T>
class Bloomfilter
{
public:
	typedef T object_type;

	Bloomfilter()
	: object_count_(0)
	{
		const uint64_t generator_seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937_64 generator(generator_seed);
		std::uniform_int_distribution<size_t> distribution;

		for (size_t i = 0; i < hash_function_count; i++)
		{
			bitmasks[i] = distribution(generator);
		}
	}

	~Bloomfilter()
	{}

	void add(const object_type& object)
	{
		const size_t object_hash = hash(object);
		for (size_t i = 0; i < hash_function_count; i++)
		{
			const size_t index_to_set = (object_hash ^ bitmasks[i]) % bloomfilter_store_size;
			bloomfilter_store_[index_to_set] = true;
		}
		++object_count_;
	}

	bool contains(const object_type& object) const
	{
		const size_t object_hash = hash(object);
		for (size_t i = 0; i < hash_function_count; i++)
		{
			const size_t index_to_get = (object_hash ^ bitmasks[i]) % bloomfilter_store_size;
			if (!bloomfilter_store_[index_to_get]) return false;
		}
		return true;
	}

	size_t object_count() const
	{
		return object_count_;
	}

private:

	static const size_t bloomfilter_store_size = 8192 * 8; // Size of the bloom filter state in bits.
	static const size_t hash_function_count = 6; // Number of hash functions to use when hashing objects.

	inline
	static size_t hash(const object_type& val)
	{
		return std::hash<object_type>()(val);
	}

	std::bitset<bloomfilter_store_size> bloomfilter_store_;
	size_t bitmasks[hash_function_count];

	size_t object_count_;
};

#endif // BLOOMFILTER_H
