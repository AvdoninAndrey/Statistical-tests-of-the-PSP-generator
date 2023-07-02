#include "StatisticalTests.h"


int main()
{
	StatisticalTests tests;
	try
	{
		tests.executeBinaryMatrixRankTest();
	}
	catch (const std::invalid_argument &error)
	{
		std::cout << "Binary Matrix Rank Test error" << std::endl;
		std::cout << "Description: " << error.what() << std::endl;
	}
	try
	{
		tests.executeMaurerUniversalStatisticalTest();
	}
	catch (const std::invalid_argument& error)
	{
		std::cout << "Maurer Universal Statistical Test error" << std::endl;
		std::cout << "Description: " << error.what() << std::endl;
	}


	return 0;
}