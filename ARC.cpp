template <typename T, typename KeyT, typename GetEl>
bool cache::ARC_cache_t<T, KeyT, GetEl>::lookup(const KeyT &key)
{
#ifdef DEBUG
	std::cout << "Key = " << key << std::endl	\
		<< "p = " << p_ << std::endl		\
		<< *this << std::endl;
#endif
	auto hit = t1_.map_.find(key);
	if (hit != t1_.map_.end())
	{
		t2_.cache_.splice(t2_.cache_.begin(), t1_.cache_, hit->second);
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
		replace();
		t2_.emplace_mru(key);
		b1_.erase(ghost_hit);
		return false;
	}

	ghost_hit = b2_.map_.find(key);
	if (ghost_hit != b2_.map_.end())
	{
		p_ = std::max((int)t1_.sz_ - (int)((b2_.cache_.size() >= b1_.cache_.size()) ? 1 : b1_.cache_.size()/b2_.cache_.size()), 0);
		replace(true);
		t2_.emplace_mru(key);
		b2_.erase(ghost_hit);
		return false;
	}

	if (t1_.cache_.size() + b1_.cache_.size() == cache_t<T, KeyT, GetEl>::sz_)
	{
		if (t1_.cache_.size() < cache_t<T, KeyT, GetEl>::sz_)
		{
			b1_.erase_lru();
			replace();
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
			replace();
		}
	}
	t1_.emplace_mru(key);
	return false;
}

template <typename T, typename KeyT, typename GetEl>
void cache::ARC_cache_t<T, KeyT, GetEl>::replace(bool in_b2)
{
	if ((!t1_.cache_.empty() && t1_.cache_.size() > p_) || (in_b2 && t1_.cache_.size() == p_))
	{
		const KeyT &key = t1_.cache_.back().id;
		b1_.cache_.push_front({key});
		b1_.map_[key] = b1_.cache_.begin();
		t1_.erase_lru();
	}
	else
	{
		const KeyT &key = t2_.cache_.back().id;
		b2_.cache_.push_front({key});
		b2_.map_[key] = b2_.cache_.begin();
		t2_.erase_lru();
	}
}
