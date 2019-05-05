#pragma once

#include <cstdint>
#include <functional>
#include <iostream>

// Storage in 64 bits and using std::hash to calculate object representation.
template <typename ObjectType>
class DefaultHash64Bit
{

public:
	typedef ObjectType	object_type;

	DefaultHash64Bit()
	: bloomfilter_store_(0)
	{}

	void add(const object_type& object)
	{
		const internal_storage_type object_hash = hash(object);
		bloomfilter_store_ |= object_hash;
	}

	bool contains(const object_type& object) const
	{
		const internal_storage_type object_hash = hash(object);
		const internal_storage_type and_result = object_hash & bloomfilter_store_;
		return and_result == object_hash;
	}

private:
	typedef uint64_t	internal_storage_type;

	static internal_storage_type hash(object_type val)
	{
		return std::hash<object_type>()(val);
	}

	internal_storage_type bloomfilter_store_;
};

template <typename FilterStorageHashType>
class Bloomfilter
{
public:
	typedef typename FilterStorageHashType::object_type object_type;

	Bloomfilter()
	: object_count_(0)
	{

	}

	~Bloomfilter()
	{}

	void add(const object_type& object)
	{
		filterStorage_.add(object);
		++object_count_;
	}

	bool contains(const object_type& object) const
	{
		return filterStorage_.contains(object);
	}

	size_t object_count() const
	{
		return object_count_;
	}

private:
	FilterStorageHashType filterStorage_;
	size_t object_count_;
};

