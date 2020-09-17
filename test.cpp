#include <iostream>
#include <fstream>
#include "ARC.cpp"

std::size_t ARC_ctr_;
int ARC_val(int i)
{
	ARC_ctr_++;
	return 0;
}

int main(int argc, char **argv)
{
	//std::istream in = (argc > 1) ? std::ifstream(argv[1]):std::istream();
	std::istream *in_ptr;
	if (argc > 1)
	{
		std::ifstream inf;
		inf.open(argv[1]);
		in_ptr = &inf;
	}
	else
		in_ptr = &std::cin;

	std::istream &in  = *in_ptr;
		
	ARC_cache_t<int> cache(10, ARC_val);
	int n;
	while (in >> n)
		cache.lookup(n);
	std::cout << "ARC misses: " << ARC_ctr_ << std::endl;
}
