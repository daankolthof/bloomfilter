#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <openssl/md5.h>

#include <bitset>
#include <memory>

class Bloomfilter
{
public:
	Bloomfilter()
	: object_count_(0) {}

	void insert(const std::string& object)
	{
		const std::unique_ptr<unsigned char[]> object_md5_hash = hash(object);
		const uint16_t* const object_hashes = reinterpret_cast<const uint16_t * const>(object_md5_hash.get());

		for (size_t i = 0; i < hash_function_count; i++)
		{
			const uint16_t index_to_set = object_hashes[i];
			bloomfilter_store_[index_to_set] = true;
		}
		++object_count_;
	}

	void clear()
	{
		bloomfilter_store_.reset();
		object_count_ = 0;
	}

	bool contains(const std::string& object) const
	{
		const std::unique_ptr<unsigned char[]> object_md5_hash = hash(object);
		const uint16_t* const object_hashes = reinterpret_cast<const uint16_t * const>(object_md5_hash.get());		

		for (size_t i = 0; i < hash_function_count; i++)
		{
			const uint16_t index_to_get = object_hashes[i];
			if (!bloomfilter_store_[index_to_get]) return false;
		}
		return true;
	}

	size_t object_count() const
	{
		return object_count_;
	}

	bool empty() const
	{
		return 0 == object_count();
	}

private:

	// Size of the MD5 hash result, always fixed to 16 bytes.
	static const size_t MD5_result_size_bytes = 16;
	
	// Size of the bloom filter state in bits (2^16).
	static const size_t bloomfilter_store_size = 65536;

	// Number of hash functions to use when hashing objects (cannot be larger than MD5_result_size_bytes/bytes_per_hash_function).
	static const size_t hash_function_count = 4;

	// Set to 2 bytes so all bloom filter bits can be indexed (2^16 different values).
	static const size_t bytes_per_hash_function = 2;

	static std::unique_ptr<unsigned char[]> hash(const std::string& val)
	{
		std::unique_ptr<unsigned char[]> result = std::make_unique<unsigned char[]>(MD5_result_size_bytes);
		const unsigned char* const md5_input_val = reinterpret_cast<const unsigned char*>(val.data());
		const size_t md5_input_length = val.length();
		MD5(md5_input_val, md5_input_length, result.get());

		return result;
	}

	std::bitset<bloomfilter_store_size> bloomfilter_store_;

	size_t object_count_;
};

#endif // BLOOMFILTER_H
