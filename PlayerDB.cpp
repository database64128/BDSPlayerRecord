#include "PlayerDB.h"

PlayerDB::PlayerDB()
{
	readPDB();
}

PlayerDB::~PlayerDB()
{
	
}

int PlayerDB::readPDB()
{
	std::ifstream PDBfile("PlayerDB", std::ios::binary);
	char gamertag[16];
	PDBEntry entry;

	if (!PDBfile)
	{
		return savePDB();
	}

	while (!PDBfile.eof())
	{
		PDBfile.read((char*)gamertag, 15);
		gamertag[15] = '\0';
		entry.gamertag.assign(gamertag);

		PDBfile.read((char*)& entry.xuid, sizeof(uint64_t));
		PDBfile.read((char*)& entry.playcount, sizeof(uint8_t));
		PDBfile.read((char*)& entry.timeplayed_seconds, sizeof(uint64_t));
		PDBfile.read((char*)& entry.lastevent, sizeof(uint8_t));
		PDBfile.read((char*)& entry.lastonline, sizeof(tm));
		if (PDBfile.fail())
			break;

		PDB.push_back(entry);
	}

	PDBfile.clear();
	PDBfile.close();
	return 0;
}

int PlayerDB::savePDB()
{
	std::ofstream PDBfile("PlayerDB", std::ios::binary);

	if (!PDBfile)
		return -1;

	for (int i = 0; i < PDB.size(); i++)
	{
		PDBfile.write((char*)PDB[i].gamertag.c_str(), 15);
		PDBfile.write((char*)& PDB[i].xuid, sizeof(uint64_t));
		PDBfile.write((char*)& PDB[i].playcount, sizeof(uint8_t));
		PDBfile.write((char*)& PDB[i].timeplayed_seconds, sizeof(uint64_t));
		PDBfile.write((char*)& PDB[i].lastevent, sizeof(uint8_t));
		PDBfile.write((char*)& PDB[i].lastonline, sizeof(tm));

		// consider removing this:
		if (PDBfile.fail())
			return -1;
	}

	PDBfile.clear();
	PDBfile.close();
	return 0;
}

int PlayerDB::buildPDB()
{
	PDB.clear();
	PDBEntry PEntry;
	time_t time_connect = 0, time_disconnect = 0;
	for (auto& LDBEntry : LogDB) // iterate in LogDB
	{
		// search gamertag in PDB
		auto index = find_if(PDB.begin(), PDB.end(), [&gamertag = LDBEntry.gamertag](const PDBEntry& currentEntry) { return gamertag == currentEntry.gamertag; });

		if (index == PDB.end()) // player not in PDB
		{
			PEntry.gamertag = LDBEntry.gamertag;
			PEntry.xuid = LDBEntry.xuid;
			PDB.push_back(PEntry);
			index--; // reposition index
		}

		if (LDBEntry.event == 0) // player connect
		{
			index->playcount++;
			index->lastevent = 0; // save last event
			index->lastonline = LDBEntry.timestamp;
		}
		else
			if (index->lastevent == 0) // player disconnect
			{
				index->lastevent = 1;
				// calculate time played
				time_connect = mktime(&index->lastonline);
				time_disconnect = mktime(&LDBEntry.timestamp);
				index->timeplayed_seconds += llround(difftime(time_disconnect, time_connect));
				// update last online time
				index->lastonline = LDBEntry.timestamp;
			}
			else // player failed to connect
			{
				index->lastevent = 1;
				index->lastonline = LDBEntry.timestamp;
			}
	}
	
	return savePDB();
}

int PlayerDB::appendPDB(std::vector<PDBEntry>::iterator start, std::vector<PDBEntry>::iterator end)
{
	for (auto LDBEntry = start; LDBEntry != end; LDBEntry++)
	{
		// copy savePDB over would be bad
	}
	
	return -1;
}

int PlayerDB::clearPDB()
{
	PDB.clear();
	savePDB();
}

int PlayerDB::simplePlayerReport(uint8_t type)
{
	std::ofstream simPReport("simPReport.log");
	if (!simPReport)
		return -1;

	simPReport << "Player\txuid\tPlay Count\tTime Played\tLast Online" << std::endl;
	for (auto& PEntry : PDB)
	{
		simPReport << PEntry.gamertag << '\t'
			<< PEntry.xuid << '\t'
			<< PEntry.playcount << '\t'
			<< PEntry.timeplayed_seconds / 86400 << 'd'
			<< (PEntry.timeplayed_seconds % 86400) / 3600 << 'h'
			<< (PEntry.timeplayed_seconds % 86400 % 3600) / 60 << 'm'
			<< (PEntry.timeplayed_seconds % 86400 % 3600 % 60) / 60 << 's'
			<< 't'
			<< std::put_time(&PEntry.lastonline, "%Y-%m-%d %H:%M:%S") << std::endl;
	}

	simPReport.close();
	return -1;
}