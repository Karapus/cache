#pragma once
#include <iostream>
#include "LRU.h"

namespace cache 
{

template <typename KeyT>
struct void_page_t
{
	KeyT id;
};

template <typename T = page_t, typename KeyT = int, typename GetEl = T (*) (const KeyT&)>
class ARC_cache_t : public cache_t<T, KeyT, GetEl>
{
	static void_page_t<KeyT> get_void_page(const KeyT& key)
	{
		return {key};
	}
	
	std::size_t p_;

	LRU_cache_t<T, KeyT, GetEl> t1_;
	LRU_cache_t<T, KeyT, GetEl> t2_;

	LRU_cache_t<void_page_t<KeyT>, KeyT, void_page_t<KeyT> (*) (const KeyT&)> b1_;
	LRU_cache_t<void_page_t<KeyT>, KeyT, void_page_t<KeyT> (*) (const KeyT&)> b2_;
	
	public:
	bool lookup(const KeyT &key);
	void replace(bool in_b2 = false);

	ARC_cache_t(std::size_t size, GetEl get_elem_ptr) :
		cache_t<T, KeyT, GetEl>(size, get_elem_ptr),
		p_(0),
		t1_(size, get_elem_ptr),
		t2_(size, get_elem_ptr),
		b1_(size, &get_void_page),
		b2_(size, &get_void_page)
	{}

	friend std::ostream & operator << (std::ostream &os, const ARC_cache_t &cache)
	{
		std::cout					\
		       << "T1 = " << cache.t1_ << std::endl	\
		       << "B1 = " << cache.b1_ << std::endl	\
		       << "T2 = " << cache.t2_ << std::endl	\
		       << "B2 = " << cache.b2_ << std::endl;
		return os;
	}
};
}
#include "ARC.cpp"
