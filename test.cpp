#include <iostream>
#include <fstream>
#include "ARC.h"

class ctr_t
{
	std::size_t ctr_;
	public:
	page_t operator() (int key)
	{
		ctr_++;
		return {key, 0};
	}
	std::size_t res()
	{
		return ctr_;
	}
	ctr_t() : ctr_(0)
	{}
};

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
	
	int n, size;
	in >> n >> size;
	
	ctr_t ARC_ctr;
	ARC_cache_t<page_t, int, ctr_t> cache(size, ARC_ctr);

	for (int id, i = 0; i < n && (in >> id); i++)
		cache.lookup(id);
	std::cout << n - ARC_ctr.res() << std::endl;
}
