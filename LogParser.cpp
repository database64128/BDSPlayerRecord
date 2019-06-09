// LogParser.cpp: LogParser class loads log into LogDB.

#if _MSC_VER
#pragma warning(disable:4996) // MSVC++ _CRT_SECURE_NO_WARNINGS
#endif

#include "LogParser.h"

LogParser::LogParser()
{
	readLogDB();
}

LogParser::~LogParser()
{

}

int LogParser::readLogDB()
{
	std::ifstream LDBfile("LogDB", std::ios::binary);
	char gamertag[16];
	LogDBEntry entry;

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

	for (auto& LDBEntry : LogDB)
	{
		LDBfile.write((char*)& LDBEntry.timestamp, sizeof(tm));
		LDBfile.write((char*)LDBEntry.gamertag.c_str(), 15);
		LDBfile.write((char*)& LDBEntry.xuid, sizeof(uint64_t));
		LDBfile.write((char*)& LDBEntry.event, sizeof(int8_t));

		// consider removing this:
		if (LDBfile.fail())
			return -1;
	}

	LDBfile.clear();
	LDBfile.close();
	return 0;
}

int LogParser::loadLogOnce(std::string filename)
{
	std::string logLine;
	std::stringstream ss_timestamp;
	LogDBEntry entry;
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

int LogParser::clearLogDB() // erase all data from LogDB
{
	LogDB.clear();
	return saveLogDB();
}

int LogParser::simpleLog() // make a simplified log file
{
	std::ofstream simLog("simlog.log");
	if (!simLog)
		return -1;

	for (auto& LDBEntry : LogDB)
	{
		simLog
			<< std::put_time(&LDBEntry.timestamp, "%Y-%m-%d %H:%M:%S") << ' '
			<< LDBEntry.gamertag << ' ';
		if (LDBEntry.event == 0)
			simLog << "Joined the game." << std::endl;
		else
			if (LDBEntry.event == 1)
				simLog << "Left the game." << std::endl;
			else
				simLog << std::endl << "Error: Undefined event" << std::endl;
	}
	simLog.close();

	return 0;
}

int LogParser::exportCSV_LogDB()
{
	std::string filename_CSV = "LogDB_";
	// get date time
	auto time = std::time(nullptr);
	std::ostringstream oss;
	oss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
	// append date time to filename
	filename_CSV.append(oss.str());
	// append extension .csv
	filename_CSV.append(".csv");

	std::ofstream CSV_LogDB(filename_CSV);
	if (!CSV_LogDB)
		return -1;

	CSV_LogDB << "Time,Player,xuid,event" << std::endl;

	for (auto& LDBEntry : LogDB)
	{
		CSV_LogDB
			<< std::put_time(&LDBEntry.timestamp, "%Y-%m-%d %H:%M:%S") << ','
			<< LDBEntry.gamertag << ','
			<< LDBEntry.xuid << ',';
		switch (LDBEntry.event)
		{
		case 0:
			CSV_LogDB << "connected" << std::endl;
			break;
		case 1:
			CSV_LogDB << "disconnected" << std::endl;
			break;
		default:
			break;
		}
	}

	CSV_LogDB.clear();
	CSV_LogDB.close();
	return 0;
}
