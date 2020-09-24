#pragma once
#include <iostream>
#include "LRU.hpp"

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
	
	void replace(bool in_b2 = false);

	public:
	bool lookup(const KeyT &key);

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

template <typename T, typename KeyT, typename GetEl>
bool ARC_cache_t<T, KeyT, GetEl>::lookup(const KeyT &key)
{
#ifdef DEBUG
	std::cout << "Key = " << key << std::endl	\
		<< "p = " << p_ << std::endl		\
		<< *this << std::endl;
#endif
	auto hit = t1_.find(key);
	if (t1_.inside(hit))
	{
		move(t1_, t2_, hit);
		return true;
	}

	hit = t2_.find(key);
	if (t2_.inside(hit))
	{
		t2_.advance_to_mru(hit);
		return true;
	}

	auto ghost_hit = b1_.find(key);
	if (b1_.inside(ghost_hit))
	{
		p_ = std::min(t1_.get_max_size() + ((b1_.get_size() >= b2_.get_size()) ? 1 : b2_.get_size()/b1_.get_size()), cache_t<T, KeyT, GetEl>::sz_);
		replace();
		t2_.emplace_mru(key);
		b1_.erase(ghost_hit);
		return false;
	}

	ghost_hit = b2_.find(key);
	if (b2_.inside(ghost_hit))
	{
		p_ = std::max((int)t1_.get_max_size() - (int)((b2_.get_size() >= b1_.get_size()) ? 1 : b1_.get_size()/b2_.get_size()), 0);
		replace(true);
		t2_.emplace_mru(key);
		b2_.erase(ghost_hit);
		return false;
	}

	if (t1_.get_size() + b1_.get_size() == cache_t<T, KeyT, GetEl>::sz_)
	{
		if (t1_.get_size() < cache_t<T, KeyT, GetEl>::sz_)
		{
			b1_.erase_lru();
			replace();
		}
		else
			t1_.erase_lru();
	}
	else 
	{
		std::size_t size = t1_.get_size() + t2_.get_size() + b1_.get_size() + b2_.get_size();
		if (size >= cache_t<T, KeyT, GetEl>::sz_)
		{
			if (size == 2 * cache_t<T, KeyT, GetEl>::sz_)
				b2_.erase_lru();
			replace();
		}
	}
	t1_.emplace_mru(key);
	return false;
}

template <typename T, typename KeyT, typename GetEl>
void ARC_cache_t<T, KeyT, GetEl>::replace(bool in_b2)
{
	if ((t1_.get_size() && t1_.get_size() > p_) || (in_b2 && t1_.get_size() == p_))
	{
		b1_.emplace_mru(t1_.get_lru_key());
		t1_.erase_lru();
	}
	else
	{
		b2_.emplace_mru(t2_.get_lru_key());
		t2_.erase_lru();
	}
}
}
