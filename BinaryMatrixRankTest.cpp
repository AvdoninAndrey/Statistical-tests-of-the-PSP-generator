#include "BinaryMatrixRankTest.h"

BinaryMatrixRankTest::BinaryMatrixRankTest(const std::string& filePath, const uint16_t& M, const uint16_t& Q)
{
	std::ifstream checkFile(filePath);
	if (checkFile.good())
	{
		this->filePath = filePath;
		checkFile.close();
	}
	else
	{
		checkFile.close();
		throw std::invalid_argument("file opening error");
	}
	this->M = M;
	this->Q = Q;
	this->maximumPossibleRank = std::min(this->M, this->Q);
}

std::vector<int32_t> BinaryMatrixRankTest::generationBinaryMatrixLittleEndianRowsNumber(uint32_t* bufferForReadBytes, uint16_t& modBits, const bool flag)
{
	std::vector<int32_t> rowsMatrixNum(this->M, 0);

	int indexBuffer = 0, shift = 32, counterIterations = (this->M * this->Q) - modBits, counter = 0;
	int i;
	if (flag == false && modBits != 0)
	{
		for (int k = 0; k < 8 - modBits; ++k)
		{
			rowsMatrixNum[0] = (rowsMatrixNum[0] << 1) | (this->tmpBitsMod & 0x1);
			this->tmpBitsMod >>= 1;
		}
	}
	for (i = 0; i < M; ++i)
	{
		bool flagEnd = false;
		for (int j = 0; j < Q; ++j)
		{
			if (i == 0 && flag == false && modBits != 0)
			{
				if (j >= Q - (8 - modBits))
				{
					break;
				}
			}
			if (counter == counterIterations)
			{
				flagEnd = true;
				break;
			}
			rowsMatrixNum[i] = (rowsMatrixNum[i] << 1) | (bufferForReadBytes[indexBuffer] & 0x1);
		
			counter++;
			--shift;
			if (shift == 0)
			{
				++indexBuffer;
				shift = 32;
				continue;
			}
			bufferForReadBytes[indexBuffer] >>= 1;
		}
		if (flagEnd)
		{
			break;
		}
	}

	if (flag == true)
	{
		for (int k = counter; k < (this->M * this->Q); ++k)
		{
			rowsMatrixNum[i] = (rowsMatrixNum[i] << 1) | (this->tmpBitsMod & 0x1);
			this->tmpBitsMod >>= 1;
		}
	}

	for (size_t i = 0; i < rowsMatrixNum.size(); ++i)
	{
		std::bitset<32> a(rowsMatrixNum[i]);
		std::cout << a.to_string() << std::endl;
		//std::cout << rowsMatrixNum[i] << std::endl;
	}

	return rowsMatrixNum;
}


uint8_t BinaryMatrixRankTest::getRankBinaryMatrix(std::vector<int32_t> & binaryMatrixLittleEndianRowsNumber)
{
	uint8_t rank = 0;
	while (!binaryMatrixLittleEndianRowsNumber.empty())
	{
		int32_t pivotRow = binaryMatrixLittleEndianRowsNumber.back();
		binaryMatrixLittleEndianRowsNumber.pop_back();
		if (pivotRow)
		{
			++rank;
			int32_t lsb = pivotRow & -pivotRow;
			for (size_t index = 0; index < binaryMatrixLittleEndianRowsNumber.size(); ++index)
			{
				int32_t row = binaryMatrixLittleEndianRowsNumber[index];
				if (row & lsb)
				{
					binaryMatrixLittleEndianRowsNumber[index] = row ^ pivotRow;
				}
			}
		}
	}
	return rank;
}


void BinaryMatrixRankTest::calcListCountRanks(uint16_t* listCountRanksFm, const uint8_t& valueRank)
{
	if (valueRank == maximumPossibleRank)
	{
		++listCountRanksFm[0];
	}
	else if (valueRank == maximumPossibleRank - 1)
	{
		++listCountRanksFm[1];
	}
	else
	{
		++listCountRanksFm[2];
	}
}

double BinaryMatrixRankTest::getPValue(uint16_t* listCountRanks)
{
	std::cout << "F_m = " << listCountRanks[0] << std::endl;
	std::cout << "F_m-1 = " << listCountRanks[1] << std::endl;
	double resultChiSquare = 0.0;
	for (int i = 0; i < 3; ++i)
	{
		resultChiSquare += std::pow(listCountRanks[i] - COEFFICIENTS_CHI_SQUARE[i] * countMatrix, 2) / (COEFFICIENTS_CHI_SQUARE[i] * countMatrix);
	}
	std::cout << "Value Chi Square = " << resultChiSquare << std::endl;
	return double(std::exp(-resultChiSquare / 2.0));
}

void BinaryMatrixRankTest::getResultBinaryMatrixRankTest()
{
	std::ifstream file;
	file.open(this->filePath, std::ios::binary | std::ios::ate);
	uint32_t countBitsInFile = file.tellg() * 8;

	this->countMatrix = static_cast<uint32_t>(floor(countBitsInFile / (M * Q))); // 10
	if (countMatrix == 0)
	{
		this->countMatrix += 1;
	}

	uint32_t countBitsForStatistics = this->countMatrix * (M * Q);
	file.seekg(0, std::ios::beg);

	uint32_t flagCountReadBits = 0;
	uint16_t flagCountReadBytes = floor(M * Q / 8), modBits = (M * Q) - flagCountReadBytes * 8; 
	
	uint32_t currentIterations = 0;
	while (flagCountReadBits != countBitsForStatistics)
	{
		uint32_t* bufferForReadBytes = new uint32_t[static_cast<uint16_t>(flagCountReadBytes / 4)];
		file.read(reinterpret_cast<char*>(bufferForReadBytes), static_cast<uint16_t>(flagCountReadBytes));
		uint8_t tmpBits = 0;
		if (modBits > 0 && currentIterations == 0)
		{
			file.read(reinterpret_cast<char*>(&tmpBits), 1);
			this->tmpBitsMod = tmpBits;
		}

		std::vector<int32_t> binaryMatrixLittleEndianRowsNumber;
		if (modBits > 0 && currentIterations % 2 == 0)
		{
			binaryMatrixLittleEndianRowsNumber = generationBinaryMatrixLittleEndianRowsNumber(bufferForReadBytes, modBits, true);
		}
		else
		{
			binaryMatrixLittleEndianRowsNumber = generationBinaryMatrixLittleEndianRowsNumber(bufferForReadBytes, modBits, true);
		}
		uint8_t rankMatrix = getRankBinaryMatrix(binaryMatrixLittleEndianRowsNumber);
		calcListCountRanks(listCountRanksFm, rankMatrix);

		++currentIterations;
	
		delete[] bufferForReadBytes;
		flagCountReadBits += flagCountReadBytes * 8 + modBits;
	}
	file.close();
	std::cout << "Result binary matrix rank test:" << std::endl;
	std::cout << "input count bits = " << countBitsInFile << std::endl;
	std::cout << "M = " << M << " Q = " << Q << std::endl;
	double resultPValue = getPValue(listCountRanksFm);
	std::cout << "P-value = " << resultPValue << "; ";
	if (resultPValue > 0.01)
	{
		std::cout << "P-value > 0.01 => binary sequence is random" << std::endl << '\n';
	}
	else
	{
		std::cout << "P-value < 0.01 => binary sequence is non-random" << std::endl << '\n';
	}
}