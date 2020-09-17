template <typename T, typename KeyT, typename GetEl>
std::ostream & operator << (std::ostream &os, const LRU_cache_t<T, KeyT, GetEl> &cache)
{
	for (auto it = cache.cache_.begin(); it != cache.cache_.end(); it++) 
		os << it->id << ' ';
	return os;
}
