#pragma once
#include <unordered_map>
#include <list>
#include <iostream>
#include <cassert>
#include "cache.hpp"

namespace cache 
{
template <typename T = page_t, typename KeyT = int, typename GetEl = T (*) (const KeyT&)>
class LRU_cache_t : public cache_t<T, KeyT, GetEl>
{
	std::list<T> cache_;
	
	using ListIt = typename std::list<T>::iterator;
	std::unordered_map<KeyT, ListIt> map_;

	using MapConstIt = typename std::unordered_map<KeyT, ListIt>::const_iterator;

	public:
	void emplace_mru(const KeyT &key)
	{
		cache_.push_front((*cache_t<T, KeyT, GetEl>::get_elem_ptr_)(key));
		map_[key] = cache_.begin();
	}
	
	void erase(const MapConstIt &hit)
	{
		cache_.erase(hit->second);
		map_.erase(hit);
	}
	
	void erase_lru()
	{
		map_.erase(cache_.back().id);
		cache_.pop_back();
	}

	void advance_to_mru(const MapConstIt &hit)
	{
		auto eltit = hit->second;
		if (eltit != cache_.begin())
			cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
	}
	
	KeyT get_lru_key() const
	{
		return cache_.back().id;
	}

	std::size_t get_size() const
	{
#ifdef DEBUG
		assert(map_.size() == cache_.size());
#endif
		return map_.size();
	}

	MapConstIt find(const KeyT& key) const
	{
		return map_.find(key);
	}

	bool inside(const MapConstIt &hit) const
	{
		return hit != map_.end();
	}

	LRU_cache_t(std::size_t size, GetEl get_elem_ptr) :
		cache_t<T, KeyT, GetEl>(size, get_elem_ptr)
	{}

	bool lookup(const KeyT &key);

	friend void move(LRU_cache_t &src, LRU_cache_t &dest, const MapConstIt &hit)
	{
		dest.cache_.splice(dest.cache_.begin(), src.cache_, hit->second);
                dest.map_.insert(*hit);
                src.map_.erase(hit);
	}

	friend std::ostream & operator << (std::ostream &os, const LRU_cache_t &cache)
	{
		for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it++) 
			os << it->id << ' ';
		return os;
	}
};

template <typename T, typename KeyT, typename GetEl>
bool LRU_cache_t<T, KeyT, GetEl>::lookup(const KeyT &key)
{
	auto hit = find(key);
	if (!inside(hit))
	{
		if (get_size() == cache_t<T, KeyT, GetEl>::get_max_size())
			erase_lru();
		emplace_mru(key);
		return false;
	}

	advance_to_mru(hit);
	return true;
}
}
