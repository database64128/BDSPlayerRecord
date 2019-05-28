#pragma once

#include <string>
#include <fstream>
#include <filesystem>

class PlayerDB
{
private:
	struct LogEntry
	{
		std::string gamertag; // 16
		uint64_t xuid = 0;
		uint8_t playcount = 0;
		uint8_t playtime = 0;
		// TODO: total play time
	} entry;
	std::filesystem::path players = "players/";
public:
	PlayerDB();
	~PlayerDB();
	int refreshDB();
	int refreshAll();
	int simplePlayerReport();
};

