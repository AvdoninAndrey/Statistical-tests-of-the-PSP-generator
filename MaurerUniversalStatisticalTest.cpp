#include "MaurerUniversalStatisticalTest.h"

MaurerUniversalStatisticalTest::MaurerUniversalStatisticalTest(const std::string& filePath)
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
}

uint32_t* MaurerUniversalStatisticalTest::createTemplateTable(const uint8_t& sizeL)
{
	uint32_t countElements = static_cast<uint16_t>(pow(2, sizeL));
	uint32_t* templateTable = new uint32_t[countElements]{ 0 };
	return templateTable;
}

uint8_t MaurerUniversalStatisticalTest::defineTheRange(const uint32_t& countBitsInFile)
{
	if (countBitsInFile < SOURCE_PARAMETERS[0].first)
	{
		throw std::invalid_argument("insufficient number of bits in the sequence");
	}
	else if (countBitsInFile >= UPPER_BOUND_OF_COUNT_BITS)
	{
		throw std::invalid_argument("exceeded the maximum possible number of bits in the sequence");
	}
	else
	{
		for (int i = 1; i < COUNT_OF_RANGES_USED; ++i)
		{
			if (countBitsInFile < SOURCE_PARAMETERS[i].first && countBitsInFile >= SOURCE_PARAMETERS[i - 1].first)
			{
				return SOURCE_PARAMETERS[i - 1].second;
			}
		}
		return SOURCE_PARAMETERS[COUNT_OF_RANGES_USED - 1].second;
	}
}

void MaurerUniversalStatisticalTest::convertToLittleEndianFormat(uint64_t* readBytes, const uint8_t& sizeL)
{
	uint64_t copyFirst = readBytes[0], copyFirstDubl = readBytes[0], copySecond = readBytes[1];
	if (sizeL <= 8)
	{
		readBytes[0] = readBytes[1] = 0;
		for (int i = 0; i < sizeL * 8; ++i)
		{
			readBytes[0] = (readBytes[0] << 1) | (copyFirst & 0x1);
			copyFirst >>= 1;
		}
	}
	else
	{
		readBytes[0] = readBytes[1] = 0;
		uint8_t shift = (sizeL % 8) * 8;
		for (int i = 0; i < shift; ++i)
		{
			readBytes[0] = (readBytes[0] << 1) | (copyFirst & 0x1);
			copyFirst >>= 1;
		}

		copyFirstDubl >>= shift;
		for (int i = 0; i < sizeof(uint64_t) * 8 - 8; ++i)
		{
			readBytes[1] = (readBytes[1] << 1) | (copyFirstDubl & 0x1);
			copyFirstDubl >>= 1;
		}

		for (int i = 0; i < shift; ++i)
		{
			readBytes[1] = (readBytes[1] << 1) | (copySecond & 0x1);
			copySecond >>= 1;
		}
	}
}


void MaurerUniversalStatisticalTest::calcInitializationOrTestSegment(uint64_t* readBytes, uint32_t* templateTable, const uint8_t& sizeL, const uint32_t& currentBlock, bool flag, double& sum)
{
	uint32_t tmpCurrentBlock = currentBlock - 8;
	convertToLittleEndianFormat(readBytes, sizeL);
	int count = 0;
	if (sizeL <= 8)
	{
		for (int i = sizeof(uint64_t) * 8 - ((8 - sizeL) * 8) - sizeL; i >= 0; i -= sizeL)
		{
			if (flag == false)
			{
				templateTable[(readBytes[0] >> i) & (static_cast<uint32_t>(pow(2, sizeL) - 1))] = tmpCurrentBlock;
			}
			else
			{
				sum += log2(tmpCurrentBlock - templateTable[(readBytes[0] >> i) & (static_cast<uint32_t>(pow(2, sizeL) - 1))]);
				templateTable[(readBytes[0] >> i) & (static_cast<uint32_t>(pow(2, sizeL) - 1))] = tmpCurrentBlock;
			}
			++tmpCurrentBlock;
		}
	}
	else
	{
		uint8_t countUsefulBitsFirstPart = (sizeL % 8) * 8; 
		uint8_t tmpCounter = 63;


		for (int i = 0; i < (sizeL * 8) / sizeL; ++i) 
		{
			uint16_t tmp = 0;

			for (int j = 0; j < sizeL; ++j) 
			{
				if (countUsefulBitsFirstPart != 0)
				{

					tmp = (static_cast<uint64_t>(tmp) << 1) | ((readBytes[0] & static_cast<uint64_t>(pow(2, countUsefulBitsFirstPart - 1))) >> (countUsefulBitsFirstPart - 1));
					--countUsefulBitsFirstPart;
				}
				else
				{
					tmp = (static_cast<uint64_t>(tmp) << 1) | ((readBytes[1] & static_cast<uint64_t>(pow(2, tmpCounter))) >> tmpCounter);
					--tmpCounter;
				}
			}
			if (flag == false)
			{
				templateTable[tmp] = tmpCurrentBlock;
			}
			else
			{
				sum += log2(tmpCurrentBlock - templateTable[tmp]);
				templateTable[tmp] = tmpCurrentBlock;
			}
			++tmpCurrentBlock;
		}
	}
}

void MaurerUniversalStatisticalTest::calcFinalTestSegment(uint64_t* readBytes, uint32_t* templateTable, const uint8_t& sizeL, const uint32_t& currentBlock, double& sum, const uint8_t& modBits)
{
	convertToLittleEndianFormat(readBytes, sizeL);

	uint32_t tmpCurrentBlock = currentBlock - (modBits / sizeL);
	if (sizeL <= 8)
	{
		int flagEnd = sizeof(uint64_t) * 8 - ((8 - sizeL) * 8) - modBits;
		for (int i = sizeof(uint64_t) * 8 - ((8 - sizeL) * 8) - sizeL; i >= flagEnd; i -= sizeL)
		{
			sum += log2(tmpCurrentBlock - templateTable[(readBytes[0] >> i) & (static_cast<uint32_t>(pow(2, sizeL) - 1))]);
			templateTable[(readBytes[0] >> i) & (static_cast<uint32_t>(pow(2, sizeL) - 1))] = tmpCurrentBlock;
			++tmpCurrentBlock;
		}
	}
	else
	{
		uint8_t countUsefulBitsFirstPart = (sizeL % 8) * 8;
		uint8_t tmpCounter = 63;
		for (int i = 0; i < floor(modBits / sizeL); ++i)
		{
			uint16_t tmp = 0;
			for (int j = 0; j < sizeL; ++j)
			{
				if (countUsefulBitsFirstPart != 0)
				{

					tmp = (static_cast<uint64_t>(tmp) << 1) | ((readBytes[0] & static_cast<uint64_t>(pow(2, countUsefulBitsFirstPart - 1))) >> (countUsefulBitsFirstPart - 1));
					--countUsefulBitsFirstPart;
				}
				else
				{
					tmp = (static_cast<uint64_t>(tmp) << 1) | ((readBytes[1] & static_cast<uint64_t>(pow(2, tmpCounter))) >> tmpCounter);
					--tmpCounter;
				}
			}
			sum += log2(tmpCurrentBlock - templateTable[tmp]);
			templateTable[tmp] = tmpCurrentBlock;
			++tmpCurrentBlock;
		}
	}
}

double MaurerUniversalStatisticalTest::getPValue(const double& valueFn, const uint8_t& sizeL, const uint32_t& countKBlocks)
{
	std::cout << "Value Fn = " << valueFn << std::endl;
	const double L = static_cast<double>(sizeL), K = static_cast<double>(countKBlocks);
	const double valueC = 0.7 - (0.8 / L) + ((4.0 + 32.0 / L) * (pow(K, -3.0 / L)) / 15.0);
	const double valueSigma = valueC * sqrt(ERFC_FUNCTION_PARAMETERS[sizeL - 3].second / K);
	std::cout << "Value c = " << valueC << std::endl;
	std::cout << "Value sigma = " << valueSigma << std::endl;
	return double(erfc(abs((valueFn - ERFC_FUNCTION_PARAMETERS[sizeL - 3].first) / (sqrt(2) * valueSigma))));
}


void MaurerUniversalStatisticalTest::getResultMaurerUniversalStatisticalTest()
{
	std::ifstream file;
	file.open(this->filePath, std::ios::binary | std::ios::ate);
	uint32_t countBitsInFile = file.tellg() * 8;

	uint8_t sizeL = defineTheRange(countBitsInFile);

	uint32_t countQBlocks = 10 * static_cast<uint32_t>(pow(2, sizeL));
	uint32_t countKBlocks = floor(static_cast<double>(countBitsInFile) / static_cast<double>(sizeL)) - countQBlocks;

	file.seekg(0, std::ios::beg);

	uint32_t* templateTable = createTemplateTable(sizeL);

	uint32_t modBytes = (countBitsInFile / 8) % sizeL;


	uint32_t countBitsForStatistics = countQBlocks * sizeL + countKBlocks * sizeL;
	uint32_t flagCountReadBits = 0;;

	uint32_t currentBlock = 1;
	double sum = 0.0;
	while (flagCountReadBits != countBitsForStatistics)
	{
		uint64_t* bufferForReadBytes = new uint64_t[2]{ 0 };

		if (flagCountReadBits / 8 < (countBitsInFile / 8) - modBytes)
		{
			file.read(reinterpret_cast<char*>(bufferForReadBytes), sizeL);
			currentBlock += (sizeL * 8) / sizeL;
			flagCountReadBits += sizeL * 8;
		}
		else
		{
			file.read(reinterpret_cast<char*>(bufferForReadBytes), modBytes);
			uint8_t modBits = static_cast<uint8_t>(floor(modBytes * 8));
			currentBlock += floor(modBytes * 8 / sizeL);

			if (modBits > sizeL)
			{
				calcFinalTestSegment(bufferForReadBytes, templateTable, sizeL, currentBlock, sum, modBits);
			}
			delete[] bufferForReadBytes;
			flagCountReadBits += static_cast<uint16_t>(floor(modBits / sizeL) * sizeL);
			continue;
		}

		(currentBlock - 8) <= countQBlocks ? calcInitializationOrTestSegment(bufferForReadBytes, templateTable, sizeL, currentBlock, false, sum) :
			calcInitializationOrTestSegment(bufferForReadBytes, templateTable, sizeL, currentBlock, true, sum);
		delete[] bufferForReadBytes;
	}
	file.close();
	delete[] templateTable;

	double valueFn = sum / static_cast<double>(countKBlocks);

	std::cout << "Result Maurer universal statistical test:" << std::endl;
	std::cout << "input count bits = " << countBitsInFile << std::endl;
	std::cout << "size L = " << static_cast<uint16_t>(sizeL) << " bits" << std::endl;
	double resultPValue = getPValue(valueFn, sizeL, countKBlocks);
	std::cout << "Result Maurer universal statistical test (P-value) = " << resultPValue << "; ";
	if (resultPValue > 0.01)
	{
		std::cout << "P-value > 0.01 => binary sequence is random" << std::endl << '\n';
	}
	else
	{
		std::cout << "P-value < 0.01 => binary sequence is non-random" << std::endl << '\n';
	}
}