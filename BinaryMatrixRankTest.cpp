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
std::vector<std::vector<uint8_t>> BinaryMatrixRankTest::generationBinaryMatrixLittleEndian(uint32_t* bufferForReadBytes, uint16_t& modBits, const bool flag)
{
	std::vector<std::vector<uint8_t>> binaryMatrixLittleEndian(this->M);
	int indexBuffer = 0, shift = 32, counterIterations = (this->M * this->Q) - modBits, counter = 0;
	int i;
	if (flag == false && modBits != 0)
	{
		for (int k = 0 , l = modBits; k < 8 - modBits; ++k, --l)
		{
			binaryMatrixLittleEndian[0].push_back(this->tmpBitsMod & 0x1);
			this->tmpBitsMod >>= 1;
			//binaryMatrixLittleEndian[0].push_back((this->tmpBitsMod >> l) & 0x1);
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
			binaryMatrixLittleEndian[i].push_back(bufferForReadBytes[indexBuffer] & 0x1);
			//binaryMatrixLittleEndian[i].push_back((bufferForReadBytes[indexBuffer] >> shift) & 0x1);
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
		for (int k = counter, l = 7; k < (this->M * this->Q); ++k, --l)
		{
			binaryMatrixLittleEndian[i].push_back(this->tmpBitsMod & 0x1);
			this->tmpBitsMod >>= 1;
			//binaryMatrixLittleEndian[0].push_back((this->tmpBitsMod >> l) & 0x1);
		}
	}
	/*std::cout << "matrix" << std::endl;
	for (i = 0; i < M; ++i)
	{
		std::cout << "[ ";
		for (int j = 0; j < Q; ++j)
		{
			if (j == Q - 1)
			{
				std::cout << (int)binaryMatrixLittleEndian[i][j] << "";
			}
			else
			{
				std::cout << (int)binaryMatrixLittleEndian[i][j] << ",";
			}

		}
		std::cout << "], "  << std::endl;
	}*/
	return binaryMatrixLittleEndian;
}

uint8_t BinaryMatrixRankTest::getRankBinaryMatrix(std::vector<std::vector<uint8_t>>& binaryMatrixLittleEndian)
{
	std::vector<int> rowsValuesNum (this->M, 0);
	for (int i = 0; i < binaryMatrixLittleEndian[0].size(); ++i)
	{
		char tmpBit;
		for (int j = 0; j < binaryMatrixLittleEndian[0].size(); ++j) {
			tmpBit = static_cast<char>(binaryMatrixLittleEndian[i][j]);
			rowsValuesNum[i] = (rowsValuesNum[i] << 1) | tmpBit;
		}
	}
	uint8_t rank = 0;
	while (!rowsValuesNum.empty())
	{
		int pivotRow = rowsValuesNum.back();
		rowsValuesNum.pop_back();
		if (pivotRow)
		{
			++rank;
			int lsb = pivotRow & -pivotRow;
			for (size_t index = 0; index < rowsValuesNum.size(); ++index)
			{
				int32_t row = rowsValuesNum[index];
				if (row & lsb)
				{
					rowsValuesNum[index] = row ^ pivotRow;
				}
			}
		}
	}
	//std::cout << "Rank Matrix = " << (int)rank <<  std::endl;
	return rank;
}
//uint8_t BinaryMatrixRankTest::getRankMatrix(std::vector<std::vector<uint8_t>>& binaryMatrixLittleEndian)
//{
//	int n = binaryMatrixLittleEndian[0].size();
//	uint8_t rank = 0;
//	for (int col = 0; col < n; col++)
//	{
//		int j = 0;
//		std::vector<int> rows;
//		while (j < binaryMatrixLittleEndian.size())
//		{
//			if (binaryMatrixLittleEndian[j][col] == 1)
//			{
//				rows.push_back(j);
//			}
//			j++;
//		}
//		if (rows.size() >= 1)
//		{
//			for (int c = 1; c < rows.size(); c++)
//			{
//				for (int k = 0; k < n; k++)
//				{
//					binaryMatrixLittleEndian[rows[c]][k] ^=  binaryMatrixLittleEndian[rows[0]][k];
//				}
//			}
//			binaryMatrixLittleEndian.erase(binaryMatrixLittleEndian.begin() + rows[0]);
//			rank++;
//		}
//	}
//	for (auto& row : binaryMatrixLittleEndian)
//	{
//		if (std::accumulate(row.begin(), row.end(), 0) > 0)
//		{
//			rank++;
//		}
//	}
//	return rank;
//}

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
	uint16_t flagCountReadBytes = floor(M * Q / 8), modBits = (M * Q) - flagCountReadBytes * 8; // 12 байт и 4 бита будет остаток при 10
	//112 (896 бит) байт и 4 бита остаток

	uint32_t currentIterations = 0;
	while (flagCountReadBits != countBitsForStatistics)
	{
		uint32_t* bufferForReadBytes = new uint32_t[static_cast<uint16_t>(flagCountReadBytes / 4)];
		file.read(reinterpret_cast<char*>(bufferForReadBytes), static_cast<uint16_t>(flagCountReadBytes));
		uint8_t tmpBits = 0;
		if (modBits > 0 && currentIterations % 2 == 0)
		{
			file.read(reinterpret_cast<char*>(&tmpBits), 1);
			this->tmpBitsMod = tmpBits;
		}

		std::vector<std::vector<uint8_t>> binaryMatrixLittleEndian;
		if (modBits > 0 && currentIterations % 2 == 0)
		{
			binaryMatrixLittleEndian = generationBinaryMatrixLittleEndian(bufferForReadBytes, modBits, true);
		}
		else
		{
			binaryMatrixLittleEndian = generationBinaryMatrixLittleEndian(bufferForReadBytes, modBits, false);
			//rank(binaryMatrixLittleEndian);
		}
		uint8_t rankMatrix = getRankBinaryMatrix(binaryMatrixLittleEndian);
		calcListCountRanks(listCountRanksFm, rankMatrix);

		++currentIterations;
		//uint8_t rankMatrix = getRankMatrix(binaryMatrixLittleEndian);
		//calcListCountRanks(listCountRanksFm, rankMatrix);

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