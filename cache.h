#pragma once
struct page_t
{
	int id;
	int val;
};

struct void_page
{
	int id;
};

template <typename T = page_t, typename KeyT = int, typename GetEl = T (*) (KeyT)>
struct cache_t
{
	std::size_t sz_;
	GetEl &get_elem_;
	bool lookup(KeyT key);
	cache_t(std::size_t size, GetEl &get_elem) :
		sz_(size),
		get_elem_(get_elem)
	{}
};
