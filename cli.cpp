// cli.cpp: temporary Command Line Interface

#include <iostream>

#include "PlayerDB.h"

int cli()
{
	PlayerDB log_0;
	int selection = 0, type = 0;
	std::string filename;
	
	/*
	if (log_0.readLogDB() < 0)
		std::cerr << "failed" << std::endl;
	*/
	std::cout
		<< "1. load log once" << std::endl
		<< "2. erase LogDB" << std::endl
		<< "3. make a simplified log" << std::endl
		<< "4. make PlayerDB" << std::endl
		<< "5. make a simple player log" << std::endl
		<< "6. export LogDB CSV" << std::endl
		<< "7. export PDB CSV" << std::endl
		<< "0. Exit" << std::endl;
	
	while (std::cin >> selection)
	{
		if (selection == 0)
			break;

		switch (selection)
		{
		case 1:
			std::cout << "Enter file name: ";
			std::cin >> filename;
			if (log_0.loadLogOnce(filename) < 0)
				std::cout << "failed." << std::endl;
			break;
		case 2:
			log_0.clearLogDB();
			break;
		case 3:
			if (log_0.simpleLog() < 0)
				std::cout << "failed." << std::endl;
			break;
		case 4:
			if (log_0.buildPDB() < 0)
				std::cout << "failed." << std::endl;
			break;
		case 5:
			std::cout << "Type: ";
			std::cin >> type;
			if (log_0.simplePlayerReport(type) < 0)
				std::cout << "failed." << std::endl;
			std::cout << "Done." << std::endl;
			break;
		case 6:
			if (log_0.exportCSV_LogDB() < 0)
				std::cout << "failed." << std::endl;
			break;
		case 7:
			if (log_0.exportCSV_PDB() < 0)
				std::cout << "failed." << std::endl;
			break;
		default:
			break;
		}
	}
	
	return 0;
}
