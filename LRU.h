#pragma once
#include <unordered_map>
#include <list>
#include <iostream>
#include "cache.h"
namespace cache 
{
template <typename T, typename KeyT, typename GetEl>
class ARC_cache_t;

template <typename T = page_t, typename KeyT = int, typename GetEl = T (*) (const KeyT&)>
class LRU_cache_t : public cache_t<T, KeyT, GetEl>
{
	public:
	std::list<T> cache_;
	
	using ListIt = typename std::list<T>::iterator;
	std::unordered_map<KeyT, ListIt> map_;

	void emplace_mru(const KeyT &key)
	{
		cache_.push_front((*cache_t<T, KeyT, GetEl>::get_elem_ptr_)(key));
		map_[key] = cache_.begin();
	}
	
	void erase(typename std::unordered_map<KeyT, ListIt>::iterator hit)
	{
		cache_.erase(hit->second);
		map_.erase(hit);
	}
	
	void erase_lru()
	{
		map_.erase(cache_.back().id);
		cache_.pop_back();
	}

	LRU_cache_t(std::size_t size, GetEl get_elem_ptr) :
		cache_t<T, KeyT, GetEl>(size, get_elem_ptr)
	{}
	bool lookup(const KeyT &key);

//	template <typename T_ARC>
//	friend class ARC_cache_t<T_ARC, KeyT, GetEl>;
};
}
#include "LRU.cpp"
