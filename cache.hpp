#pragma once
namespace cache
{
struct page_t
{
	int id;
	int val;
};

template <typename T = page_t, typename KeyT = int, typename GetEl = T (**) (KeyT)>
class cache_t
{
	protected:
	std::size_t sz_;
	GetEl const get_elem_ptr_;
	public:
	virtual bool lookup(const KeyT &key) = 0;
	std::size_t get_max_size() const
	{
		return sz_;
	}
	cache_t(std::size_t size, GetEl get_elem_ptr) :
		sz_(size),
		get_elem_ptr_(get_elem_ptr)
	{}
	virtual ~cache_t()
	{}
};
}
