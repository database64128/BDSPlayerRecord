#pragma once

#include "LogParser.h"

class PlayerDB : public LogParser
{
protected:
	struct PDBEntry
	{
		std::string gamertag; // 16
		uint64_t xuid = 0;
		uint8_t playcount = 0; // add on connecting
		uint64_t timeplayed_seconds = 0; // add on disconnecting
		uint8_t lastevent = 1; // 0 for connect, 1 for disconnect
		tm lastonline; // last time online
	};
	std::vector<PDBEntry> PDB;
	std::filesystem::path players = "players/";
	int readPDB();
	int savePDB();
public:
	PlayerDB();
	~PlayerDB();
	int buildPDB(); // clear and build PDB from the ground up
	int appendPDB(std::vector<PDBEntry>::iterator start, std::vector<PDBEntry>::iterator end); // read new LogDB entries and append to PDB
	int clearPDB(); // clear PDB
	int simplePlayerReport(uint8_t type); // 0: no sorting, 1: sort by gamertag, 2: sort by time played, 3: sort by play count, 4: sort by last online
};

