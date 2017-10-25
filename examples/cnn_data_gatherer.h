#ifndef CNN_DATA_GATHERER_H
#define CNN_DATA_GATHERER_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_score.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>
#include <string>

const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class CnnDataGatherer : public sc2::ReplayObserver {
public:
    int step_num = 0;
    unsigned int base_count = 0;
    std::vector<std::string> fout_strings;
    std::ofstream fout;

    CnnDataGatherer() :
        sc2::ReplayObserver() {
    }

    void OpenFile(std::string headers)
    {
        std::string output_path = replay_observer_file_path_.substr(0,replay_observer_file_path_.size() - 10) + "_CNN.csv";
        fout.open(output_path.c_str(), std::ofstream::out);
        fout_strings.push_back(headers);
    }

    void CloseFile()
    {
        for (std::string x : fout_strings)
            fout << x;
        fout.close();
    }


    // Inherited methods
    
    void OnGameStart() 
    {
        std::cout << "Replay: " << replay_count++ << std::endl;
        step_num = 0;
        base_count = 0;
        fout_strings.clear();
        OpenFile("Bases,GasRate,Army,Race,Structures,Time\n");
    }


    void OnStep() 
    {
        const sc2::ObservationInterface* obs = Observation();
        const sc2::Score& score = obs->GetScore();
        base_count = 0;

        float min_rate = score.score_details.collection_rate_minerals;
        float gas_rate = score.score_details.collection_rate_vespene;
        float struct_val = score.score_details.total_value_structures;  // Left the first nexus cost so it can relate 400 with the 1 nexus and 2 with the 400 jump
        sc2::Units units = Observation()->GetUnits();
        unsigned int army_val = GetArmyValue(obs, units);

        fout_strings.push_back(std::to_string(base_count) + "," + 
                               std::to_string(gas_rate) + "," + 
                               std::to_string(army_val) + "," + 
                               std::to_string(3) + "," + // Protoss
                               std::to_string(struct_val) + "," + 
                               std::to_string(GetGameSecond(step_num)) + "\n");
        step_num += STEP_SIZE;
    }

    void OnGameEnd() 
    {
        CloseFile();
    }


    unsigned int GetArmyValue(const sc2::ObservationInterface* obs, sc2::Units units)
    {
        // This may be built into the scoreboard if I can figure that out...
        unsigned int army_value = 0;
        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        // sc2::Units units = obs->GetUnits();
        for (auto u : units)
        {
            if (u->owner == PLAYER_ID)
            {
                if (u->unit_type == 59) // Nexus
                    ++base_count;
                if (DesiredArmyUnit(u))
                {
                    army_value += unit_types.at(u->unit_type).mineral_cost;
                    army_value += unit_types.at(u->unit_type).vespene_cost;
                    // std::cout << std::to_string(u->unit_type) + ",";
                }
            }
        }
        // std::cout << std::endl;
        return army_value;
    }    

    bool DesiredArmyUnit(const sc2::Unit*& u)
    {
        // Fix this. Utilize enum class UNIT_TYPEID
        return u->is_alive && u->unit_type != 801 && u->unit_type != 341 && u->unit_type != 665 && u->unit_type != 666
            && u->unit_type != 146 && u->unit_type != 147
            && u->unit_type != 483 && u->unit_type != 342 && u->unit_type != 608
            && u->unit_type != 343 && u->unit_type != 84
            && u->unit_type != 60 && u->unit_type != 61 && u->unit_type != 59 && u->unit_type != 72
            && u->unit_type != 62 && u->unit_type != 67;
    }
};

#endif