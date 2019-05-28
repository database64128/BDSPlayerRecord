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
		entry.timestamp.assign(logLine, 1, 19);
		// strncpy(entry.timestamp, logLine.c_str + 1, 19);

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

	// overwrite LogDB
	overwriteLogDB();
	
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
		simLog << LogDB[i].timestamp << ' '
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

void LogParser::eraseLogDB() // erase all data from LogDB
{
	LogDB.clear();
	overwriteLogDB();
}

int LogParser::readLogDB()
{
	std::ifstream DBfile("LogDB", std::ios::binary);
	char timestamp[20];
	char gamertag[16];

	if (!DBfile)
	{
		return overwriteLogDB();
	}

	while (!DBfile.eof())
	{
		DBfile.read((char*)timestamp, 19);
		DBfile.read((char*)gamertag, 15);
		timestamp[19] = '\0';
		gamertag[15] = '\0';
		entry.timestamp.assign(timestamp);
		entry.gamertag.assign(gamertag);

		DBfile.read((char*)& entry.xuid, sizeof(uint64_t));
		DBfile.read((char*)& entry.event, sizeof(int8_t));
		if (DBfile.fail())
			break;

		LogDB.push_back(entry);
	}
	
	DBfile.clear();
	DBfile.close();
	return 0;
}

int LogParser::saveLogDB(std::ofstream& DBfile)
{
	for (int i = 0; i < LogDB.size(); i++)
	{
		DBfile.write((char*)LogDB[i].timestamp.c_str(), 19);
		DBfile.write((char*)LogDB[i].gamertag.c_str(), 15);
		DBfile.write((char*)& LogDB[i].xuid, sizeof(uint64_t));
		DBfile.write((char*)& LogDB[i].event, sizeof(int8_t));

		// consider removing this:
		if (DBfile.fail())
			return -1;
	}

	DBfile.clear();
	DBfile.close();
	return 0;
}

int LogParser::appendLogDB()
{
	std::ofstream DBfile("LogDB", std::ios::app | std::ios::binary);
	if (DBfile)
		return saveLogDB(DBfile);
	
	return -1;
}

int LogParser::overwriteLogDB()
{
	std::ofstream DBfile("LogDB", std::ios::binary);
	if (DBfile)
		return saveLogDB(DBfile);
	
	return -1;
}