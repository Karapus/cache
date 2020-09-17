#include <iostream>
#include "LRU.h"

template <typename T, typename KeyT, typename GetEl>
class ARC_cache_t : public cache_t<T, KeyT, GetEl>
{
	std::size_t p_;

	LRU_cache_t<T, KeyT, GetEl> t1_;
	LRU_cache_t<T, KeyT, GetEl> t2_;

	LRU_cache_t<void_page, KeyT, GetEl> b1_;
	LRU_cache_t<void_page, KeyT, GetEl> b2_;
	
	public:
	bool lookup(KeyT key);
	void replace(const KeyT &key, bool in_b2 = false);

	ARC_cache_t(std::size_t size, GetEl &get_elem) :
		cache_t<T, KeyT, GetEl>(size, get_elem),
		t1_(size, get_elem),
		t2_(size, get_elem),
		b1_(size, get_elem),
		b2_(size, get_elem)
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
