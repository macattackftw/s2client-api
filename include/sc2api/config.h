#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <linux/limits.h>
#include <stdlib.h>
#include "sc2utils/sc2_manage_process.h"

using namespace std;

const string replay_config_file = "/home/kyle/IntergalacticLifelineI/s2client-api/config.txt";
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

struct ConfigReplayFilter
{
    int p0_mmr = 0, 
        p1_mmr = 0, 
        p0_apm = 10, 
        p1_apm = 10,
        winner = -1, 
        p0_race = -1, 
        p1_race = -1;
    float min_duration = 0.0f;
    float max_duration = numeric_limits<float>::max();
};

    class Config
    {
        public:
        ReplayInfo rp_filter;
        string exe_path;
        string replay_path;
        ConfigReplayFilter filter;
        Config(string file)
        {
            ifstream fin;
            fin.open(file.c_str());
            if (!fin)
            {
                std::cout << "Config file failed to open." << std::endl;
                return;
            }
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
                                rp_filter.players[0].player_id = stoi(line_s);
                            break;
                        case 3:
                            if(line_s.size()>0)
                                filter.p0_mmr = stoi(line_s);
                            break;
                        case 4:
                            if(line_s.size()>0)
                                filter.p0_apm = stoi(line_s);
                            break;
                        case 5:
                            if(line_s.size()>0)
                                filter.p0_race = line_s == "Terran" ? Race(Terran) : (line_s == "Zerg" ? Race(Zerg) : Race(Protoss));
                            break;
                        case 6:
                            if(line_s.size()>0)
                                rp_filter.players[0].race_selected = static_cast<Race>(stoi(line_s));
                            break;
                        case 7:
                            if(line_s.size()>0)
                            {
                                // Can't use this because we need an "I don't care" option. TODO
                                rp_filter.players[0].game_result = line_s == "Win" ? GameResult(Win) : GameResult(Loss);    // Not used
                                if (rp_filter.players[0].game_result == GameResult(Win))
                                    filter.winner = 0;
                            }
                            break;
                        case 9:
                            if(line_s.size()>0)
                                rp_filter.players[1].player_id = stoi(line_s);
                            break;
                        case 10:
                            if(line_s.size()>0)
                                filter.p1_mmr = stoi(line_s);
                            break;
                        case 11:
                            if(line_s.size()>0)
                                filter.p1_apm = stoi(line_s);
                            break;
                        case 12:
                            if(line_s.size()>0)
                                filter.p1_race = line_s == "Terran" ? Race(Terran) : (line_s == "Zerg" ? Race(Zerg) : Race(Protoss));
                            break;
                        case 13:
                            if(line_s.size()>0)
                                // rp_filter.players[1].race_selected = static_cast<Race>(stoi(line_s));
                                rp_filter.players[1].race_selected = static_cast<Race>(stoi(line_s));
                            break;
                        case 14:
                            if(line_s.size()>0)
                            {
                                rp_filter.players[1].game_result = line_s == "Win" ? GameResult(Win) : GameResult(Loss);    // Not used
                                if (rp_filter.players[1].game_result == GameResult(Win))
                                    filter.winner = 1;
                            }
                            break;
                        case 16:
                            if(line_s.size()>0)
                                filter.min_duration = stof(line_s);
                            break;
                        case 17:
                            if(line_s.size()>0)
                                filter.max_duration = stof(line_s);
                            break;
                            
                        case 18:
                            if(line_s.size()>0)
                                rp_filter.duration_gameloops = (unsigned int)(stoi(line_s));
                            break;
                        case 19:
                            if(line_s.size()>0)
                                rp_filter.num_players = stoi(line_s);
                            break;
                        case 20:
                            if(line_s.size()>0)
                                rp_filter.data_build = stoi(line_s);
                            break;
                        case 21:
                            if(line_s.size()>0)
                                rp_filter.base_build = stoi(line_s);
                            break;
                        case 22:
                            rp_filter.map_name = line_s;
                            break;
                        case 23:
                            rp_filter.map_path = line_s;
                            break;
                        case 24:
                            rp_filter.replay_path = line_s;
                            break;
                        case 25:
                            rp_filter.version = line_s;
                            break;
                        case 26:
                            rp_filter.data_version = line_s;
                            break;
                        case 28:
                            exe_path = line_s;
                            break;
                        case 30:
                            replay_path = line_s;
                            break;
                    
                    }
                }        
            }
            fin.close();
            
            #ifndef __WIN32__
            if (replay_path[0] == '.')
            {
                char resolved_path[PATH_MAX]; 
                realpath(replay_path.c_str(), resolved_path); 
                replay_path = resolved_path;
                replay_path += "/"; // This guy...realpath() takes it off
            }
            else if (replay_path[0] == '~')
                replay_path = sc2::GetUserDirectory() + replay_path.substr(1);
            #endif
        }
    };