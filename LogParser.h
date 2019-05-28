#pragma once

#include <fstream>
#include <string>
#include <vector>
// consider remove:
#include <cstring>

class LogParser
{
private:
	// log db
	struct LogDBEntry
	{
		// TODO: change time format
		std::string timestamp; // 19
		std::string gamertag; // 15
		uint64_t xuid = 0;
		int8_t event = -1; // 0 for connect, 1 for disconnect, 2 for clearing connection state, -1 for error
	} entry;
	std::vector<LogDBEntry> LogDB;
	int readLogDB(); // read from LogDB file to vector
	int saveLogDB(std::ofstream& DBfile);
	int appendLogDB(); // append to LogDB file from vector
	int overwriteLogDB(); // overwrite LogDB file from vector
public:
	LogParser();
	~LogParser();

	int loadLogOnce(std::string filename);
	// TODO: Auto load and save to LogDB
	int loadLog();
	int simpleLog(); // make a simplified log file
	std::vector<LogDBEntry> getLogDB(); // get data from LogDB
	void eraseLogDB(); // erase all data from LogDB
};

