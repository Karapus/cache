#pragma once
#include <iostream>
#include "LRU.h"

/*template <typename KeyT>
void_page_t get_void_page(KeyT key)
{
	return {key};
}*/

template <typename KeyT>
struct get_void_page_t
{
	void_page_t<KeyT> operator() (KeyT key)
	{
		return {key};
	}
};

template <typename T = page_t, typename KeyT = int, typename GetEl = page_t (*) (const KeyT&)>
class ARC_cache_t : public cache_t<T, KeyT, GetEl>
{
	std::size_t p_;

	LRU_cache_t<T, KeyT, GetEl> t1_;
	LRU_cache_t<T, KeyT, GetEl> t2_;

	get_void_page_t<KeyT> b1_get_el, b2_get_el;
	LRU_cache_t<void_page_t<KeyT>, KeyT, get_void_page_t<KeyT>> b1_;
	LRU_cache_t<void_page_t<KeyT>, KeyT, get_void_page_t<KeyT>> b2_;
	
	public:
	bool lookup(const KeyT &key);
	void replace(bool in_b2 = false);

	ARC_cache_t(std::size_t size, GetEl &get_elem) :
		cache_t<T, KeyT, GetEl>(size, get_elem),
		p_(0),
		t1_(size, get_elem),
		t2_(size, get_elem),
		b1_(size, b1_get_el),
		b2_(size, b2_get_el)
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

#include "ARC.cpp"
