#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>

class LogParser
{
protected:
	struct LogDBEntry
	{
		tm timestamp = { 0 };
		std::string gamertag; // 15
		uint64_t xuid = 0;
		int8_t event = -1; // 0 for connect, 1 for disconnect, 2 for clearing connection state, -1 for error
	};
	std::vector<LogDBEntry> LogDB;
	std::filesystem::path logs = "logs/";
	int readLogDB(); // read from LogDB file to vector
	int saveLogDB();
public:
	LogParser();
	~LogParser();

	// consider rename for parity:
	int loadLogOnce(std::string filename);
	int loadLog();

	int clearLogDB(); // erase all data from LogDB
	int simpleLog(); // make a simplified log file
	int exportCSV_LogDB();
};
