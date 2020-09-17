template <typename T, typename KeyT, typename GetEl>
std::ostream & operator << (std::ostream &os, const LRU_cache_t<T, KeyT, GetEl> &cache)
{
	for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it++) 
		os << it->id << ' ';
	return os;
}

template <typename T, typename KeyT, typename GetEl>
bool LRU_cache_t<T, KeyT, GetEl>::lookup(const KeyT &key)
/*{
	return false;
}*/
{
	auto hit = map_.find(key);
	
	if (hit == map_.end())
	{
		if (cache_.size() == cache_t<T, KeyT, GetEl>::sz_)
			erase_lru();
		emplace_mru(key);
		return false;
	}

	auto eltit = hit->second;
	if (eltit != cache_.begin())
		cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
	return true;
}
