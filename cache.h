#pragma once
namespace cache
{
struct page_t
{
	int id;
	int val;
};

template <typename T = page_t, typename KeyT = int, typename GetEl = T (**) (KeyT)>
struct cache_t
{
	std::size_t sz_;
	GetEl const get_elem_ptr_;
	virtual bool lookup(const KeyT &key) = 0;
	cache_t(std::size_t size, GetEl get_elem_ptr) :
		sz_(size),
		get_elem_ptr_(get_elem_ptr)
	{}
};
}
