// main.cpp: BDSPlayerRecord main application

#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>

#include "PlayerDB.h"

// temporarily using cli
int cli();

int main(int argc, char* argv[])
{
	std::vector<std::string> argV(argv, argv + argc);
	PlayerDB server_0;

	// arg parser
	std::unordered_map<std::string, std::function<int()>> argM
	{
		{ "--cli", [&]() { return cli(); } }, // legacy CLI, Debug only
		{ "--help", [&]() { std::cout << "See README for more information." << std::endl; return 0; } },
		{ "--append-logdb", [&]() { if (argV.size() == 2) return server_0.appendLogDB(); else return server_0.appendLogDB(argV[2]); } },
		{ "--clear-logdb", [&]() { return server_0.clearLogDB(); } },
		{ "--build-playerdb", [&]() { return server_0.buildPDB(); } },
		{ "--csv-logdb", [&]() { return server_0.exportCSV_LogDB(); } },
		{ "--csv-playerdb", [&]() { try { if (argV.size() == 2) return server_0.exportCSV_PDB(); else return server_0.exportCSV_PDB(stoi(argV[2])); } catch (...) { std::cout << "Invalid argument: " << argV[2] << std::endl; return 0; } } },
		{ "--txt-logdb", [&]() { return server_0.simpleLog(); } },
		{ "--txt-playerdb", [&]() { try { if (argV.size() == 2) return server_0.simplePlayerReport(); else return server_0.simplePlayerReport(stoi(argV[2])); } catch (...) { std::cout << "Invalid argument: " << argV[2] << std::endl; return 0; } } }
	};

	// no arguments
	// GUI
	if (argV.size() == 1)
	{
		std::cout << "GUI is currently unavailable. See README for more information." << std::endl;
		return 0;
	}

	// Too many arguments
	if (argV.size() > 3)
	{
		std::cout << "Too many arguments. Please do one task at a time." << std::endl;
		return 0;
	}

	auto argIt = argM.find(argV[1]);

	// invalid argument
	if (argIt == argM.end())
	{
		std::cout << "Invalid argument: " << argV[1] << "" << std::endl;
		return 0;
	}
	else
	{
		return argIt->second();
	}

	return 0;
}
