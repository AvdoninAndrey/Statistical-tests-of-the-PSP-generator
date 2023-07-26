#pragma once
#include "BinaryMatrixRankTest.h"
#include "MaurerUniversalStatisticalTest.h"

class StatisticalTests
{
private:
	const std::vector<std::string> FILE_PATHS_FOR_RANK_TEST = {
		//"C:\\Programming\\c++\\pract\\mainTestAtlas\\psp.bin",
		"C:\\Programming\\c++\\pract\\mainTestAtlas\\psp1024.bin",
		//"C:\\Programming\\c++\\pract\\mainTestAtlas\\psp1000.bin",
		//"C:\\Programming\\c++\\pract\\mainTestAtlas\\testText.txt",
	};
	const std::vector<std::pair<uint16_t, uint16_t>> M_AND_Q_RANK_TEST = {
		//{32, 32},
		{32, 32},
		//{24, 24},
		//{32, 32},
	};
	const std::vector<std::string> FILE_PATHS_FOR_MAURER_TEST = {
	   "C:\\Programming\\c++\\pract\\mainTestAtlas\\psp.bin",
		"C:\\Programming\\c++\\pract\\mainTestAtlas\\psp1000.bin", 
		"C:\\Programming\\c++\\pract\\mainTestAtlas\\testText.txt",
	};
public:
	void executeBinaryMatrixRankTest();
	void executeMaurerUniversalStatisticalTest();
};