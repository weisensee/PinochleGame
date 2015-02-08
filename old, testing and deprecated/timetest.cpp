// timetest.cpp
// lucas weisensee
// time string test program
// 

#include "string"
#include <iostream>
#include <iomanip>
#include <ctime>

int main(int argc, char const *argv[])
{
	printf("\nTime string creation testing program.\n");

	std::string temp;

	time_t t = std::time(nullptr);
	tm t2 = *std::localtime(&t);
	std::cout << std::put_time(&t2, "%d-%m-%Y %H-%M-%S") << std::endl;



	return 0;
}