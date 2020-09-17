#pragma once
struct page_t
{
	int id;
	int val;
};

template <typename KeyT>
struct void_page_t
{
	KeyT id;
};

template <typename T = page_t, typename KeyT = int, typename GetEl = T (*) (KeyT)>
struct cache_t
{
	std::size_t sz_;
	GetEl &get_elem_;
	virtual bool lookup(const KeyT &key) = 0;
	cache_t(std::size_t size, GetEl &get_elem) :
		sz_(size),
		get_elem_(get_elem)
	{}
};
