#include <iostream>
#include <fstream>
#include "ARC.cpp"

std::size_t ARC_ctr_;
int ARC_val()
{
	ARC_ctr_++;
	return 0;
}

int main(int argc, char **argv)
{
	//auto in = (argc > 1) ? std::ifstream(argv[1]) : std::cin;
	std::ifstream in;
	if (argc > 1)
		in.open(argv[1]);
	else
		return 1;
		
	ARC_cache_t<int> cache(10, ARC_val);
	int n;
	while (in >> n)
		cache.lookup(n);
	std::cout << "ARC misses: " << ARC_ctr_ << std::endl;
}
