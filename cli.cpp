// cli.cpp: temporary Command line interface

#include <iostream>

#include "LogParser.h"
#include "PlayerDB.h"

int cli()
{
	LogParser log_0;
	char selection = 0;
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
		<< "0. Exit" << std::endl;
	
	while (std::cin >> selection)
	{
		if (selection == '0')
			break;

		switch (selection)
		{
		case '1':
			std::cout << "Enter file name: ";
			std::cin >> filename;
			if (log_0.loadLogOnce(filename) < 0)
				std::cout << "failed." << std::endl;
			break;
		case '2':
			log_0.eraseLogDB();
			break;
		case '3':
			if (log_0.simpleLog() < 0)
				std::cout << "failed." << std::endl;
			break;
		default:
			break;
		}
	}
	

	return 0;
}