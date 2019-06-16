#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <chrono>

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

	// Note that copySortLogDB() is implemented but currently not used anywhere!
	// TO-DO: sort LogDB after appending
	std::vector<LogDBEntry> copySortLogDB(int sortBy = 1); // copy and sort LogDB, 0: no sorting, 1: by timestamp, 2: by gamertag, 3: by event
public:
	LogParser();
	~LogParser();

	int appendLogDB(std::string filename); // append from log file
	int appendLogDB(); // append from log directory
	int clearLogDB(); // erase all data from LogDB
	int simpleLog(); // make a simplified log file
	int exportCSV_LogDB();
};
