#include "StatisticalTests.h"

void StatisticalTests::executeBinaryMatrixRankTest()
{
	std::vector<BinaryMatrixRankTest> tests;
	for (size_t i = 0; i < this->FILE_PATHS_FOR_RANK_TEST.size(); ++i)
	{
		BinaryMatrixRankTest test(FILE_PATHS_FOR_RANK_TEST[i], M_AND_Q_RANK_TEST[i].first, M_AND_Q_RANK_TEST[i].second);
		tests.push_back(test);
	}
	for (size_t i = 0; i < tests.size(); ++i)
	{
		tests[i].getResultBinaryMatrixRankTest();
	}
}
void StatisticalTests::executeMaurerUniversalStatisticalTest()
{
	std::vector<MaurerUniversalStatisticalTest> tests;
	for (size_t i = 0; i < this->FILE_PATHS_FOR_MAURER_TEST.size(); ++i)
	{
		MaurerUniversalStatisticalTest test(FILE_PATHS_FOR_MAURER_TEST[i]);
		tests.push_back(test);
	}
	for (size_t i = 0; i < tests.size(); ++i)
	{
		tests[i].getResultMaurerUniversalStatisticalTest();
	}
}

