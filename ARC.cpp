#include <unordered_map>
#include <list>
#include <iostream>

template <typename T, typename KeyT = int, typename GetEl = T (*) (KeyT)>
class cache_t
{
	protected:
	std::size_t sz_;
	GetEl get_elem_;
	public:
	bool lookup(KeyT key);
	cache_t(std::size_t size, T get_elem(KeyT key)) :
		sz_(size),
		get_elem_(get_elem)
	{}
};

template <typename T, typename KeyT = int, typename GetEl = T (*) (KeyT)>
class ARC_cache_t;

template <typename T, typename KeyT = int, typename GetEl = T (*) (KeyT)>
class LRU_cache_t : public cache_t<T, KeyT, GetEl>
{
	std::list<std::pair<KeyT, T>> cache_;
	
	typedef typename std::list<std::pair<KeyT, T>>::iterator ListIt;
	std::unordered_map<KeyT, ListIt> map_;

	void emplace_mru(KeyT key)
	{
		cache_.push_front({key, cache_t<T, KeyT, GetEl>::get_elem_(key)});
		map_[key] = cache_.begin();
	}
	
	void erase(typename std::unordered_map<KeyT, ListIt>::iterator hit)
	{
		cache_.erase(hit->second);
		map_.erase(hit);
	}
	
	void erase_lru()
	{
		map_.erase(cache_.back().first);
		cache_.pop_back();
	}

	public:
	LRU_cache_t(std::size_t size, T get_elem(KeyT)) :
		cache_t<T, KeyT, GetEl>(size, get_elem)
	{}

	friend ARC_cache_t<T, KeyT, GetEl>;
	
	template <typename To, typename KeyTo, typename GetElo>
	friend std::ostream & operator << (std::ostream, const LRU_cache_t<To, KeyTo, GetElo>);
};

template <typename T, typename KeyT = int, typename GetEl = T (*) (KeyT)>
std::ostream & operator << (std::ostream &os, const LRU_cache_t<T, KeyT, GetEl> &cache)
{
	for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it++) 
		os << it->first << ' ';
	return os;
}

template <typename T, typename KeyT, typename GetEl>
class ARC_cache_t : public cache_t<T, KeyT, GetEl>
{
	std::size_t p_;

	LRU_cache_t<T, KeyT, GetEl> t1_;
	LRU_cache_t<T, KeyT, GetEl> t2_;

	LRU_cache_t<int, KeyT> b1_;
	LRU_cache_t<int, KeyT> b2_;
	
	public:
	bool lookup(KeyT key);
	void replace(const KeyT &key, bool in_b2 = false);

	ARC_cache_t(std::size_t size, T get_elem(KeyT)) :
		cache_t<T, KeyT, GetEl>(size, get_elem),
		t1_(size, get_elem),
		t2_(size, get_elem),
		b1_(size, get_elem),
		b2_(size, get_elem)
	{}

	template <typename To, typename KeyTo, typename GetElo>
	friend std::ostream & operator << (std::ostream, const ARC_cache_t<To, KeyTo, GetElo>);
};

template <typename T, typename KeyT, typename GetEl>
bool ARC_cache_t<T, KeyT, GetEl>::lookup(KeyT key)
{
	std::cout << "Key = " << key << std::endl	\
	       << "T1 = " << t1_ << std::endl	\
	       << "B1 = " << b1_ << std::endl	\
	       << "T2 = " << t2_ << std::endl	\
	       << "B2 = " << b2_ << std::endl	\
	       << std::endl;

	auto hit = t1_.map_.find(key);
	if (hit != t1_.map_.end())
	{
		t2_.cache_.splice(t2_.cache_.begin(), t2_.cache_, hit->second);
		t2_.map_.insert(*hit);
		t1_.map_.erase(hit);
		return true;
	}

	hit = t2_.map_.find(key);
	if (hit != t2_.map_.end())
	{
		auto eltit = hit->second;
		if (eltit != t2_.cache_.begin())
			t2_.cache_.splice(t2_.cache_.begin(), t2_.cache_, eltit, std::next(eltit));
		return true; 
	}

	auto ghost_hit = b1_.map_.find(key);
	if (ghost_hit != b1_.map_.end())
	{
		p_ = std::min(t1_.sz_ + ((b1_.cache_.size() >= b2_.cache_.size()) ? 1 : b2_.cache_.size()/b1_.cache_.size()), cache_t<T, KeyT, GetEl>::sz_);
		replace(key);
		t2_.emplace_mru(key);
		b1_.erase(ghost_hit);
		return false;
	}

	ghost_hit = b2_.map_.find(key);
	if (ghost_hit != b2_.map_.end())
	{
		p_ = std::max((int)t1_.sz_ - (int)((b2_.cache_.size() >= b1_.cache_.size()) ? 1 : b1_.cache_.size()/b2_.cache_.size()), 0);
		replace(key, true);
		t2_.emplace_mru(key);
		b2_.erase(ghost_hit);
		return false;
	}

	if (t1_.cache_.size() + b1_.cache_.size() == cache_t<T, KeyT, GetEl>::sz_)
	{
		if (t1_.cache_.size() < cache_t<T, KeyT, GetEl>::sz_)
		{
			b1_.erase_lru();
			replace(key);
		}
		else
			t1_.erase_lru();
	}
	else 
	{
		std::size_t size = t1_.cache_.size() + t2_.cache_.size() + b1_.cache_.size() + b2_.cache_.size();
		if (size >= cache_t<T, KeyT, GetEl>::sz_)
		{
			if (size == 2 * cache_t<T, KeyT, GetEl>::sz_)
				b2_.erase_lru();
			replace(key);
		}
	}
	t1_.emplace_mru(key);
	return false;
}

template <typename T, typename KeyT, typename GetEl>
void ARC_cache_t<T, KeyT, GetEl>::replace(const KeyT &key, bool in_b2)
{
	if ((!t1_.cache_.empty() && t1_.cache_.size() > p_) || (in_b2 && t1_.cache_.size() == p_))
	{
		t1_.erase_lru();
		b1_.cache_.push_front({key, 0});
		b1_.map_[key] = b1_.cache_.begin();
	}
	else
	{
		t2_.erase_lru();
		b2_.cache_.push_front({key, 0});
		b2_.map_[key] = b2_.cache_.begin();
	}
}

template <typename T, typename KeyT = int, typename GetEl = T (*) (KeyT)>
std::ostream & operator << (std::ostream &os, const ARC_cache_t<T, KeyT, GetEl> &cache)
{
	std::cout					\
	       << "T1 = " << cache.t1_ << std::endl	\
	       << "B1 = " << cache.b1_ << std::endl	\
	       << "T2 = " << cache.t2_ << std::endl	\
	       << "B2 = " << cache.b2_ << std::endl	\
	       << std::endl;
	return os;
}
