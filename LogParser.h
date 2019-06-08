#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>
// consider remove:
#include <cstring>

class LogParser
{
protected:
	struct LogDBEntry
	{
		tm timestamp = { 0 };
		std::string gamertag; // 15
		uint64_t xuid = 0;
		int8_t event = -1; // 0 for connect, 1 for disconnect, 2 for clearing connection state, -1 for error
	} entry;
	std::vector<LogDBEntry> LogDB;
	int readLogDB(); // read from LogDB file to vector
	int saveLogDB();
public:
	LogParser();
	~LogParser();

	int loadLogOnce(std::string filename);
	// TODO: Auto load and save to LogDB
	int loadLog();
	int simpleLog(); // make a simplified log file
	std::vector<LogDBEntry> getLogDB(); // get data from LogDB
	void clearLogDB(); // erase all data from LogDB
};

