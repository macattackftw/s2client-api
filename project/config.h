#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

using namespace std;
//namespace sc2{
//***TODO***remove the structs and enums and use the sc2 namespace instead
enum Race {
    Terran,
    Zerg,
    Protoss,
    Random
};
enum GameResult {
    Win,
    Loss,
    Tie,
    Undecided
};
struct ReplayPlayerInfo {
    //! Player ID.
    int player_id;
    //! Player ranking.
    int mmr;
    //! Player actions per minute.
    int apm;
    //! Actual player race.
    Race race;
    //! Selected player race. If the race is "Random", the race data member may be different.
    Race race_selected;
    //! If the player won or lost.
    GameResult game_result;

    ReplayPlayerInfo() :
        player_id(0),
        mmr(-10000),
        apm(0),
        race(Random),
        race_selected(Random) {
    }
};
struct ReplayInfo {
    float duration;
    unsigned int duration_gameloops;
    int32_t num_players;
    uint32_t data_build;
    uint32_t base_build;
    string map_name;
    string map_path;
    string replay_path;
    string version;
    string data_version;
    ReplayPlayerInfo players[2];
};



	class config
	{
	    public:
		ReplayInfo filter;
		string exe_path;
		string replay_path;
		config(string file);
		~config();
	};

	config::config(string file)
	{

	    ifstream fin;
	    fin.open(file.c_str());

	    string line_s;
	    int count = 0;
	   
	    while(getline(fin, line_s))
	    {
		count++;

		if(line_s.front() != '-')
                {
                    line_s.erase(0, line_s.find(':')+1);
		    switch(count)
		    {   

		        case 2:
			    if(line_s.size()>0)
		                filter.players[0].player_id = stoi(line_s);
		        case 3:
			    if(line_s.size()>0)
		                filter.players[0].mmr = stoi(line_s);
		        case 4:
			    if(line_s.size()>0)
		               filter.players[0].apm = stoi(line_s);
		        case 5:
			    if(line_s.size()>0)
		               filter.players[0].race = static_cast<Race>(stoi(line_s));
			case 6:
			    if(line_s.size()>0)
		               filter.players[0].race_selected = static_cast<Race>(stoi(line_s));
			case 7:
			    if(line_s.size()>0)
		               filter.players[0].game_result = static_cast<GameResult>(stoi(line_s));
		        case 9:
			    if(line_s.size()>0)
		               filter.players[1].player_id = stoi(line_s);
		        case 10:
			    if(line_s.size()>0)
		               filter.players[1].mmr = stoi(line_s);
		        case 11:
			    if(line_s.size()>0)
		                filter.players[1].apm = stoi(line_s);
		        case 12:
			    if(line_s.size()>0)
		                filter.players[1].race = static_cast<Race>(stoi(line_s));
			case 13:
			    if(line_s.size()>0)
		                filter.players[1].race_selected = static_cast<Race>(stoi(line_s));
			case 14:
			    if(line_s.size()>0)
		                filter.players[1].game_result = static_cast<GameResult>(stoi(line_s));
		        case 16:
			    if(line_s.size()>0)
		                filter.duration = stof(line_s);
		        case 17:
			    if(line_s.size()>0)
		                filter.duration_gameloops = (unsigned int)(stoi(line_s));
		        case 18:
			    if(line_s.size()>0)
		                filter.num_players = stoi(line_s);
		        case 19:
                            if(line_s.size()>0)
		                filter.data_build = stoi(line_s);
			case 20:
                            if(line_s.size()>0)
		                filter.base_build = stoi(line_s);
			case 21:
		            filter.map_name = line_s;
		        case 22:
		            filter.map_path = line_s;
		        case 23:
		            filter.replay_path = line_s;
		        case 24:
		            filter.version = line_s;
		        case 25:
		            filter.data_version = line_s;
			case 27:
		            exe_path = line_s;
			case 29:
		            replay_path = line_s;
			
		    }
                }
	
	    }
	    fin.close();
	}

	config::~config()
	{
	    
	}

//}
































