// PlayerDB.cpp: PlayerDB class manages PlayerDB

#if _MSC_VER
#pragma warning(disable:4996) // MSVC++ _CRT_SECURE_NO_WARNINGS
#endif

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
		PDBfile.read((char*)& entry.playcount, sizeof(uint32_t));
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
		PDBfile.write((char*)& PDB[i].playcount, sizeof(uint32_t));
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
	return appendPDB(LogDB.begin(), LogDB.end());
}

int PlayerDB::appendPDB(std::vector<LogDBEntry>::iterator start, std::vector<LogDBEntry>::iterator end)
{
	PDBEntry PEntry;
	time_t time_connect = 0, time_disconnect = 0;
	
	for (auto LDBEntry = start; LDBEntry != end; LDBEntry++)
	{
		// search gamertag in PDB
		auto index = find_if(PDB.begin(), PDB.end(), [&gamertag = LDBEntry->gamertag](const PDBEntry& currentEntry) { return gamertag == currentEntry.gamertag; });

		if (index == PDB.end()) // player not in PDB
		{
			PEntry.gamertag = LDBEntry->gamertag;
			PEntry.xuid = LDBEntry->xuid;
			PEntry.playcount = 0;
			PEntry.timeplayed_seconds = 0;
			PEntry.lastevent = 1;
			PDB.push_back(PEntry);
			index = PDB.end() - 1; // reposition index
		}

		if (LDBEntry->event == 0) // player connect
		{
			index->playcount++;
			index->lastevent = 0; // save last event
			index->lastonline = LDBEntry->timestamp;
		}
		else
			if (index->lastevent == 0) // player disconnect
			{
				index->lastevent = 1;
				// calculate time played
				time_connect = mktime(&index->lastonline);
				time_disconnect = mktime(&LDBEntry->timestamp);
				index->timeplayed_seconds += llround(difftime(time_disconnect, time_connect));
				// update last online time
				index->lastonline = LDBEntry->timestamp;
			}
			else // player failed to connect
			{
				index->lastevent = 1;
				index->lastonline = LDBEntry->timestamp;
			}
	}
	
	return savePDB();
}

int PlayerDB::clearPDB()
{
	PDB.clear();
	return savePDB();
}

int PlayerDB::simplePlayerReport(uint8_t type)
{
	auto PDBsortbygamertag = [](PDBEntry x, PDBEntry y)
	{
		std::string a = x.gamertag, b = y.gamertag;
		std::transform(a.begin(), a.end(), a.begin(), ::toupper);
		std::transform(b.begin(), b.end(), b.begin(), ::toupper);
		return a < b;
	};

	auto PDBsortbytimeplayed = [](PDBEntry x, PDBEntry y)
	{
		return x.timeplayed_seconds > y.timeplayed_seconds;
	};

	auto PDBsortbyplaycount = [](PDBEntry x, PDBEntry y)
	{
		return x.playcount > y.playcount;
	};

	auto PDBsortbylastonline = [](PDBEntry x, PDBEntry y)
	{
		return mktime(&x.lastonline) > mktime(&y.lastonline);
	};
	
	std::ofstream simPReport("simPReport.log");
	if (!simPReport)
		return -1;

	// copy std::vector
	std::vector<PDBEntry> sortedPDB(PDB);
	// sort
	switch (type)
	{
	case 1:
		sort(sortedPDB.begin(), sortedPDB.end(), PDBsortbygamertag);
		break;
	case 2:
		sort(sortedPDB.begin(), sortedPDB.end(), PDBsortbytimeplayed);
		break;
	case 3:
		sort(sortedPDB.begin(), sortedPDB.end(), PDBsortbyplaycount);
		break;
	case 4:
		sort(sortedPDB.begin(), sortedPDB.end(), PDBsortbylastonline);
		break;
	case 0:
	default:
		break;
	}

	simPReport << "Player\txuid\tNumber of Login\tTime Played\tLast Online" << std::endl;
	for (auto& PEntry : sortedPDB)
	{
		simPReport
			<< PEntry.gamertag << '\t'
			<< PEntry.xuid << '\t'
			<< PEntry.playcount << '\t'
			<< PEntry.timeplayed_seconds / 86400 << 'd'
			<< (PEntry.timeplayed_seconds % 86400) / 3600 << 'h'
			<< (PEntry.timeplayed_seconds % 86400 % 3600) / 60 << 'm'
			<< PEntry.timeplayed_seconds % 86400 % 3600 % 60 << 's'
			<< '\t'
			<< std::put_time(&PEntry.lastonline, "%Y-%m-%d %H:%M:%S") << std::endl;
	}

	simPReport.close();
	return 0;
}

int PlayerDB::exportCSV_PDB()
{
	std::string filename_CSV = "PDB_";
	// get date time
	auto time = std::time(nullptr);
	std::ostringstream oss;
	oss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
	// append date time to filename
	filename_CSV.append(oss.str());
	// append extension .csv
	filename_CSV.append(".csv");

	std::ofstream CSV_PDB(filename_CSV);
	if (!CSV_PDB)
		return -1;

	CSV_PDB << "Player,xuid,Number of Login,Time Played,Last Online" << std::endl;

	for (auto& PEntry : PDB)
	{
		CSV_PDB
			<< PEntry.gamertag << ','
			<< PEntry.xuid << ','
			<< PEntry.playcount << ','
			<< PEntry.timeplayed_seconds / 86400 << 'd'
			<< (PEntry.timeplayed_seconds % 86400) / 3600 << 'h'
			<< (PEntry.timeplayed_seconds % 86400 % 3600) / 60 << 'm'
			<< PEntry.timeplayed_seconds % 86400 % 3600 % 60 << 's'
			<< ','
			<< std::put_time(&PEntry.lastonline, "%Y-%m-%d %H:%M:%S") << std::endl;
	}

	CSV_PDB.clear();
	CSV_PDB.close();
	return 0;
}
