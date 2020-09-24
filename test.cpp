#include <iostream>
#include <fstream>
#include "ARC.hpp"

namespace {
class ctr_t
{
	std::size_t ctr_;
	public:
	cache::page_t operator() (int key)
	{
		ctr_++;
		return {key, 0};
	}
	std::size_t res() const
	{
		return ctr_;
	}
	ctr_t() : ctr_(0)
	{}
};
}

int main(int argc, char **argv)
{
	std::istream *in_ptr;
	std::ifstream inf;
	if (argc > 1)
	{
		inf.open(argv[1]);
		in_ptr = &inf;
	}
	else
		in_ptr = &std::cin;

	std::istream &in  = *in_ptr;
	
	int size;
	in >> size;
	
	ctr_t ARC_ctr;
	cache::ARC_cache_t<cache::page_t, int, ctr_t *> ARC(size, &ARC_ctr);
	cache::cache_t<cache::page_t, int, ctr_t *> &cache = ARC;

	int n;
	in >> n;

	for (int id, i = 0; i < n && (in >> id); i++)
		cache.lookup(id);
	std::cout << n - ARC_ctr.res() << std::endl;
}
