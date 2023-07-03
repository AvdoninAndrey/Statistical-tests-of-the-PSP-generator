#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <bitset>
class BinaryMatrixRankTest
{
private:
	uint16_t M = 0,  Q = 0, maximumPossibleRank = 0;
	std::string filePath = "";
	const double COEFFICIENTS_CHI_SQUARE[3] = { 0.2888, 0.5776, 0.1336 };
	uint32_t countMatrix = 0;
	uint16_t listCountRanksFm[3] = { 0 };

	//uint8_t getRankMatrix(std::vector<std::vector<uint8_t>> & binaryMatrixLittleEndian);

	uint8_t getRankBinaryMatrix(std::vector<std::vector<uint8_t>>& binaryMatrixLittleEndian);

	std::vector<std::vector<uint8_t>> generationBinaryMatrixLittleEndian(uint32_t* bufferForReadBytes, uint16_t& modBits, const bool flag = true);

	uint8_t tmpBitsMod = 0;

	void calcListCountRanks(uint16_t* listCountRanksFm, const uint8_t& valueRank);

	double getPValue(uint16_t* listCountRanks);
public:
	BinaryMatrixRankTest() = delete;
	BinaryMatrixRankTest(const std::string & filePath, const uint16_t & M, const uint16_t& Q);
	void getResultBinaryMatrixRankTest();
};