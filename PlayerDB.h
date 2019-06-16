#pragma once

#include "LogParser.h"

class PlayerDB : public LogParser
{
protected:
	struct PDBEntry
	{
		std::string gamertag; // 16
		uint64_t xuid = 0;
		uint32_t playcount = 0; // add on connecting
		uint64_t timeplayed_seconds = 0; // add on disconnecting
		uint8_t lastevent = 1; // 0 for connect, 1 for disconnect
		tm lastonline = { 0 }; // last time online
	};
	std::vector<PDBEntry> PDB;
	std::filesystem::path players = "players/";
	int readPDB();
	int savePDB();
	std::vector<PDBEntry> copySortPDB(int sortBy = 1); // copy and sort PDB, 0: no sorting, 1: by gamertag, 2: by time played, 3: by play count, 4: by last online
public:
	PlayerDB();
	~PlayerDB();
	int buildPDB(); // clear and build PDB from *all entries* of LogDB
	int appendPDB(std::vector<LogDBEntry>::iterator start, std::vector<LogDBEntry>::iterator end); // select LogDB entries to append to PlayerDB
	int clearPDB(); // clear PDB
	int simplePlayerReport(int sortBy = 1);
	int exportCSV_PDB(int sortBy = 1);
};
