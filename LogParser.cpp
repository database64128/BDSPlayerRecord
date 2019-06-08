// LogParser.cpp: LogParser class loads log into LogDB.

#include "LogParser.h"

LogParser::LogParser()
{
	readLogDB();
}

LogParser::~LogParser()
{

}

int LogParser::loadLogOnce(std::string filename)
{
	std::string logLine;
	std::stringstream ss_timestamp;
	std::ifstream log(filename);
	if (!log)
		return -1;

	log.seekg(0, std::ios::beg);
	while (!log.eof())
	{
		// read a line each time
		getline(log, logLine);
		// skip useless lines
		if (logLine[0] != '[' || logLine[27] != 'P') // no timestamp or not player event
			continue;
		// copy timestamp
		ss_timestamp.clear();
		ss_timestamp.str(logLine.substr(1, 19));
		ss_timestamp >> std::get_time(&entry.timestamp, "%Y-%m-%d %H:%M:%S");

		// detect event type and copy gamertag
		if (logLine[34] == 'c') // connected
		{
			entry.event = 0;
			entry.gamertag = logLine.substr(45, 15);
		}
		else
			if (logLine[34] == 'd') // disconnected
			{
				entry.event = 1;
				entry.gamertag = logLine.substr(48, 15);
			}
			else
			{
				entry.event = -1; // error
				return -1;
			}
		// trim gamertag
		for (int i = 14; i >= 0; i--)
		{
			if (entry.gamertag[i] == ',')
			{
				entry.gamertag.erase(entry.gamertag.begin() + i, entry.gamertag.end());
				break;
			}
		}
		// strncpy(entry.gamertag, gamertag.c_str, gamertag.length());

		// copy xuid
		entry.xuid = stoull(logLine.substr(45 + (int64_t)entry.event * 3 + entry.gamertag.length() + 8));

		// push back entry to vector
		LogDB.push_back(entry);
	}

	log.clear();
	log.close();

	saveLogDB();
	
	return 0;
}

int LogParser::loadLog()
{
	// to be added after 1.12
	return -1;
}

int LogParser::simpleLog() // make a simplified log file
{
	std::ofstream simLog("simlog.log");
	if (!simLog)
		return -1;

	for (int i = 0; i < LogDB.size(); i++)
	{
		simLog << std::put_time(&LogDB[i].timestamp, "%Y-%m-%d %H:%M:%S") << ' '
			<< LogDB[i].gamertag << ' ';
		if (LogDB[i].event == 0)
			simLog << "Joined the game." << std::endl;
		else
			if (LogDB[i].event == 1)
				simLog << "Left the game." << std::endl;
			else
				simLog << std::endl << "Error: Undefined event" << std::endl;
	}
	simLog.close();

	return 0;
}

std::vector<LogParser::LogDBEntry> LogParser::getLogDB()
{
	// copy the vector for exporting
	std::vector<LogParser::LogDBEntry> expLogDB(LogDB.begin(), LogDB.end());
	return expLogDB;
}

void LogParser::clearLogDB() // erase all data from LogDB
{
	LogDB.clear();
	saveLogDB();
}

int LogParser::readLogDB()
{
	std::ifstream LDBfile("LogDB", std::ios::binary);
	char gamertag[16];

	if (!LDBfile)
	{
		return saveLogDB();
	}

	while (!LDBfile.eof())
	{
		LDBfile.read((char*)& entry.timestamp, sizeof(tm));
		LDBfile.read((char*)gamertag, 15);
		gamertag[15] = '\0';
		entry.gamertag.assign(gamertag);

		LDBfile.read((char*)& entry.xuid, sizeof(uint64_t));
		LDBfile.read((char*)& entry.event, sizeof(int8_t));
		if (LDBfile.fail())
			break;

		LogDB.push_back(entry);
	}
	
	LDBfile.clear();
	LDBfile.close();
	return 0;
}

int LogParser::saveLogDB()
{
	std::ofstream LDBfile("LogDB", std::ios::binary);
	if (!LDBfile)
		return -1;
	
	for (int i = 0; i < LogDB.size(); i++)
	{
		LDBfile.write((char*)& LogDB[i].timestamp, sizeof(tm));
		LDBfile.write((char*)LogDB[i].gamertag.c_str(), 15);
		LDBfile.write((char*)& LogDB[i].xuid, sizeof(uint64_t));
		LDBfile.write((char*)& LogDB[i].event, sizeof(int8_t));

		// consider removing this:
		if (LDBfile.fail())
			return -1;
	}

	LDBfile.clear();
	LDBfile.close();
	return 0;
}
