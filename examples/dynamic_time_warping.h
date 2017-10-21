/*
    Gather data necessary for multidemensional dynamic time warping (DTW).
*/

#ifndef DYNAMIC_TIME_WARPING_H
#define DYNAMIC_TIME_WARPING_H

#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>


const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class DynamicTimeWarping : public sc2::ReplayObserver {
public:
    int step_num = 0;
    std::vector<std::string> fout_strings;
    std::ofstream fout;
    bool halt_data = false;
    int probes = 0; // initial scout
    const char* kReplayFolder_;

    DynamicTimeWarping(const char* ReplayFolder) :
        sc2::ReplayObserver() {
        kReplayFolder_ = ReplayFolder;
    }

    void OpenFile(std::string headers)
    {
        // std::string output_path = replay_observer_file_path_.substr(0,replay_observer_file_path_.size() - 10) + "_dtw.csv";
        // fout.open(output_path.c_str(), std::ofstream::out);
        // fout_strings.push_back(headers);
    }

    void CloseFile()
    {
        // for (std::string x : fout_strings)
            // fout << x;
        // fout.close();
    }


    // Inherited methods
    
    void OnGameStart() 
    {
        std::cout << "Replay: " << replay_count++ << std::endl;
        step_num = 0;
        halt_data = false;
        // fout_strings.clear();
        // OpenFile("Seconds,Item,Upgrade\n");
    }
    
    void OnUnitCreated(const sc2::Unit* unit) 
    {
        // if (unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
            // if (unit->unit_type != 801) // Lets ignore phase shifting for strategizer?
                // fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(unit->unit_type) + ",0\n");
    }

    void OnUpgradeCompleted(sc2::UpgradeID upgrade) 
    {
        // fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(upgrade) + ",1\n");
    }


    void OnStep() 
    {
        if (!halt_data && step_num < 4000)  // Stop at roughly 177 seconds or first unit death
        {
            std::cout << std::to_string(GetGameSecond(step_num)) << "\tarmy value: " << GetArmyValue() 
                      << std::endl;
        }
        step_num += STEP_SIZE;
    }

    void OnUnitDestroyed(const sc2::Unit* unit)
    {
        if (!halt_data && unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
        {
            // std::cout << unit->owner << "\t" << unit->unit_type << std::endl;
            if (unit->unit_type == 84)
            {
                if (probes == 0)
                    ++probes;
                else
                    halt_data = true;
            }
            else if (DesiredUnit(unit)) // Lets ignore phase shifting for strategizer?
            {
                halt_data = true;
            }

        }
    }    

    void OnGameEnd() 
    {
        // CloseFile();
    }

    unsigned int GetArmyValue()
    {
        // This may be built into the scoreboard if I can figure that out...
        unsigned int army_value = 0;
        const sc2::ObservationInterface* obs = Observation();
        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        sc2::Units units = obs->GetUnits();
        for (auto u : units)
        {
            if (DesiredUnit(u))
            {
                army_value += unit_types.at(u->unit_type).mineral_cost;
                army_value += unit_types.at(u->unit_type).vespene_cost;
                // std::cout << std::to_string(u->unit_type) + ",";
            }
        }
        // std::cout << std::endl;
        return army_value;
    }

    bool DesiredUnit(const sc2::Unit*& u)
    {
        // Fix this. Utilize enum class UNIT_TYPEID
        return u->owner == PLAYER_ID && u->unit_type != 801 && u->unit_type != 341 && u->unit_type != 665 && u->unit_type != 666 
                && u->unit_type != 146 && u->unit_type != 147
                && u->unit_type != 483 && u->unit_type != 342 && u->unit_type != 608
                && u->unit_type != 343 && u->unit_type != 84
                && u->unit_type != 60  && u->unit_type != 61  && u->unit_type != 59  && u->unit_type != 72
                && u->unit_type != 62  && u->unit_type != 67;
    }

};

#endif