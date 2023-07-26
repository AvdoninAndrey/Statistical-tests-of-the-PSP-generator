#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

class MaurerUniversalStatisticalTest
{
private:
	const uint8_t COUNT_OF_RANGES_USED = 8;
	const std::pair<uint32_t, uint8_t> SOURCE_PARAMETERS[8] = {
		{ 999 ,3},
		{10000, 4},
		{ 100000 ,6},
		{ 387840, 6 },
		{ 904960, 7 },
		{ 2068480, 8 },
		{ 4654080, 9 },
		{ 10342400, 10 },
	};
	const std::pair<double, double> ERFC_FUNCTION_PARAMETERS[8] = {
		  {2.401, 1.901},
		 { 3.3112247, 2.358},
		{ 4.2534266, 2.704},
		{ 5.2177052, 2.954 },
		{ 6.1962507, 3.125 },
		{ 7.1836656, 3.238 },
		{ 8.1764248, 3.311 },
		{ 9.1723243, 3.356  },
	};
	std::string filePath = "";
	const uint32_t UPPER_BOUND_OF_COUNT_BITS = 22753280;

	void convertToLittleEndianFormat(uint64_t* readBytes, const uint8_t& sizeL);
	uint8_t defineTheRange(const uint32_t& countBitsInFile);
	uint32_t* createTemplateTable(const uint8_t& sizeL);
	void calcInitializationOrTestSegment(uint64_t* readBytes, uint32_t* templateTable, const uint8_t& sizeL, const uint32_t& currentBlock, bool  flag, double& sum);
	void calcFinalTestSegment(uint64_t* readBytes, uint32_t* templateTable, const uint8_t& sizeL, const uint32_t& currentBlock, double& sum, const uint8_t& modBits);
	double getPValue(const double& valueFn, const uint8_t& sizeL, const uint32_t& countKBlocks);
public:
	MaurerUniversalStatisticalTest() = delete;
	MaurerUniversalStatisticalTest(const std::string& filePath);
	void getResultMaurerUniversalStatisticalTest();
};