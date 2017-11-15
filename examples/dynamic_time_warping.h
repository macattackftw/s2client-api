/*
Gather data necessary for multidemensional dynamic time warping (DTW).
*/

#ifndef DYNAMIC_TIME_WARPING_H
#define DYNAMIC_TIME_WARPING_H

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_score.h"
#include "convex_hull_slow.h"
#include "unit_centroid.h"
#include "parse_csv.h"
#include "md_dtw.h"

#include "sc2utils/sc2_manage_process.h"

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>


const int STEP_SIZE = 22;
const int PLAYER_ID = 1;
static int replay_count = 0;


class DynamicTimeWarping : public sc2::ReplayObserver {
  public:
    int step_num = 0;
    std::vector<std::string> fout_strings;
    std::vector<std::vector<float> > all_in_;
    std::vector<std::vector<float> > cheese_;
    std::vector<std::vector<float> > economic_;
    std::vector<std::vector<float> > timing_;
    std::vector<std::vector<float> > time_series;

    const unsigned int cols = 9;
    std::ofstream fout;
    bool halt_data = false;
    int probes = 0, adepts = 0; // initial scout
    bool file_write_flag = false;


    DynamicTimeWarping() :
        sc2::ReplayObserver() {
        std::vector<std::string> lines = GetLines("/home/kyle/IntergalacticLifelineI/s2client-api/baselines/all_in_baseline.csv");
        for (unsigned int i = 0; i < cols; ++i) {
            all_in_.emplace_back(std::vector<float>(lines.size(), 0.0f));
            cheese_.emplace_back(std::vector<float>(lines.size(), 0.0f));
            economic_.emplace_back(std::vector<float>(lines.size(), 0.0f));
            timing_.emplace_back(std::vector<float>(lines.size(), 0.0f));
            time_series.emplace_back(std::vector<float>());
        }
        for (unsigned int i = 0; i < lines.size(); ++i) {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            for (int j = cols - 1; j > 0; --j) {
                pos = lines[i].rfind(",");
                all_in_[j][i] = std::stof(lines[i].substr(pos + 1));
                lines[i].resize(pos);
            }
            // Deal with the first column
            all_in_[0][i] = std::stof(lines[i]);
        }

        lines.clear();
        lines = GetLines("/home/kyle/IntergalacticLifelineI/s2client-api/baselines/cheese_baseline.csv");
        std::cout << "Cheeselines: " << lines.size() << std::endl;
        for (unsigned int i = 0; i < lines.size(); ++i) {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            for (int j = cols - 1; j > 0; --j) {
                pos = lines[i].rfind(",");
                cheese_[j][i] = std::stof(lines[i].substr(pos + 1));
                lines[i].resize(pos);
            }
            // Deal with the first column
            cheese_[0][i] = std::stof(lines[i]);
        }

        lines.clear();
        lines = GetLines("/home/kyle/IntergalacticLifelineI/s2client-api/baselines/economic_baseline.csv");
        std::cout << "economiclines: " << lines.size() << std::endl;
        for (unsigned int i = 0; i < lines.size(); ++i) {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            for (int j = cols - 1; j > 0; --j) {
                pos = lines[i].rfind(",");
                economic_[j][i] = std::stof(lines[i].substr(pos + 1));
                lines[i].resize(pos);
            }
            // Deal with the first column
            economic_[0][i] = std::stof(lines[i]);
        }

        lines.clear();
        lines = GetLines("/home/kyle/IntergalacticLifelineI/s2client-api/baselines/timing_baseline.csv");
        for (unsigned int i = 0; i < lines.size(); ++i) {
            // From the end of the line walk backwards looking for commas
            // Split that bit off and add to the appropriate vector.
            // Skips first column
            size_t pos = 0;
            for (int j = cols - 1; j > 0; --j) {
                pos = lines[i].rfind(",");
                timing_[j][i] = std::stof(lines[i].substr(pos + 1));
                lines[i].resize(pos);
            }
            // Deal with the first column
            timing_[0][i] = std::stof(lines[i]);
        }
    }

    void OpenFile(std::string headers) {
        std::string output_path = replay_observer_file_path_.substr(0, replay_observer_file_path_.size() - 10) + "_dtw_guess.txt";
        fout.open(output_path.c_str(), std::ofstream::out);
        fout_strings.push_back(headers);
    }

    void CloseFile() {
        for (std::string x : fout_strings)
            fout << x;
        fout.close();
    }


    // Inherited methods

    void OnGameStart() {
        std::cout << "Replay: " << replay_count++ << std::endl;
        step_num = 0;
        probes = 0;
        adepts = 0;
        halt_data = false;
        if (file_write_flag) {
            fout_strings.clear();
            // OpenFile("Seconds,ArmyVal,MinRate,GasRate,StructVal,UpgMin,UpgGas,StructPerim,ArmyDist\n");
            OpenFile("");
        }
    }

    void OnUnitCreated(const sc2::Unit* unit) {
        // if (unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
        // if (unit->unit_type != 801) // Lets ignore phase shifting for strategizer?
        // fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(unit->unit_type) + ",0\n");
    }

    void OnUpgradeCompleted(sc2::UpgradeID upgrade) {
        // fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," + std::to_string(upgrade) + ",1\n");
    }


    void OnStep() {

        // sc2::Units structures = Observation()->GetUnits(sc2::Unit::Self, IsStructure(Observation()));
        // if (0)
        if (!halt_data && step_num < 7400) { // Stop at roughly 177 seconds or first unit death
            const sc2::ObservationInterface* obs = Observation();
            const sc2::Score& score = obs->GetScore();
            // sc2::Score score;
            float min_rate = score.score_details.collection_rate_minerals;
            float gas_rate = score.score_details.collection_rate_vespene;
            float upg_min = score.score_details.used_minerals.upgrade;
            float upg_vesp = score.score_details.used_vespene.upgrade;
            float struct_val = score.score_details.total_value_structures;
            sc2::Units units = Observation()->GetUnits();
            sc2::Units army = GetArmyUnits(units);
            sc2::Units bases = GetBases(units);
            float army_val = GetArmyValue(obs, army);
            float struct_area = convhull::Area(convhull::ConvexHull(GetStructures(obs, units)));
            float army_dist = army_val == 0 ? 0.0f : GetDistance(GetUnitCentroid(units), GetUnitCentroid(army));

            if (file_write_flag) {
                // fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "," +
                //                           std::to_string(army_val) + "," +
                //                           std::to_string(min_rate) + "," +
                //                           std::to_string(gas_rate) + "," +
                //                           std::to_string(GetStructuresValue(obs, units)) + "," +
                //                           std::to_string(upg_min) + "," +
                //                           std::to_string(upg_vesp) + "," +
                //                           std::to_string(struct_area) + "," +
                //                           std::to_string(army_dist) + "\n");

                time_series[0].push_back(GetGameSecond(step_num));
                time_series[1].push_back(army_val);
                time_series[2].push_back(min_rate);
                time_series[3].push_back(gas_rate);
                time_series[4].push_back(GetStructuresValue(obs, units));
                time_series[5].push_back(upg_min);
                time_series[6].push_back(upg_vesp);
                time_series[7].push_back(struct_area);
                time_series[8].push_back(army_dist);

                MultiDimensionalTimeWarping md_dtw(time_series, all_in_, cheese_, economic_, timing_);

                float all_in = md_dtw.GetAllIn();
                float cheese = md_dtw.GetCheese();
                float economic = md_dtw.GetEconomic();
                float timing = md_dtw.GetTiming();

                // Sometimes the data goes nuts???
                if (all_in < 9999999 && cheese < 9999999 && economic < 9999999 && timing < 9999999 )
                {
                    fout_strings.emplace_back(std::to_string(GetGameSecond(step_num)) + "\t" + md_dtw.MostLikely() + "\t\tAll_In  : " + std::to_string(all_in) + "\tCheese: " + std::to_string(cheese)
                          + "\tEconomic: " + std::to_string(economic) + "\tTiming: " + std::to_string(timing) + "\n");
                }


            } else {
                // std::string output = (std::to_string(GetGameSecond(step_num)) + "," +
                //                           std::to_string(army_val) + "," +
                //                           std::to_string(min_rate) + "," +
                //                           std::to_string(gas_rate) + "," +
                //                           std::to_string(GetStructuresValue(obs, units)) + "," +
                //                           std::to_string(upg_min) + "," +
                //                           std::to_string(upg_vesp) + "," +
                //                           std::to_string(struct_area) + "," +
                //                           std::to_string(army_dist) + "\n");
                // std::cout << "OUTPUT: " << output;
                time_series[0].push_back(GetGameSecond(step_num));
                time_series[1].push_back(army_val);
                time_series[2].push_back(min_rate);
                time_series[3].push_back(gas_rate);
                time_series[4].push_back(GetStructuresValue(obs, units));
                time_series[5].push_back(upg_min);
                time_series[6].push_back(upg_vesp);
                time_series[7].push_back(struct_area);
                time_series[8].push_back(army_dist);

                std::string output = std::to_string(time_series[1].back()) + "\t" + 
                                     std::to_string(time_series[2].back()) + "\t" + 
                                     std::to_string(time_series[3].back()) + "\t" + 
                                     std::to_string(time_series[4].back()) + "\t" + 
                                     std::to_string(time_series[5].back()) + "\t" + 
                                     std::to_string(time_series[6].back()) + "\t" + 
                                     std::to_string(time_series[7].back()) + "\t" + 
                                     std::to_string(time_series[8].back());

                std::cout << output << std::endl;

                // cout << endl;
                // cout << time_series[0].back() << endl;
                // cout << time_series[1].back() << endl;
                // cout << time_series[2].back() << endl;
                // cout << time_series[3].back() << endl;
                // cout << time_series[4].back() << endl;
                // cout << time_series[5].back() << endl;
                // cout << time_series[6].back() << endl;
                // cout << time_series[7].back() << endl;
                // cout << time_series[8].back() << endl;

                MultiDimensionalTimeWarping md_dtw(time_series, all_in_, cheese_, economic_, timing_);

                float all_in = md_dtw.GetAllIn();
                float cheese = md_dtw.GetCheese();
                float economic = md_dtw.GetEconomic();  // 4.357142857142857
                float timing = md_dtw.GetTiming();

                // Sometimes the data goes nuts???
                if (all_in < 9999999 && cheese < 9999999 && economic < 9999999 && timing < 9999999 )
                {
                    std::cout << GetGameSecond(step_num) << "\t" << md_dtw.MostLikely() << "\t\tAll_In  : " << all_in <<   "\tCheese: " << cheese
                              << "\tEconomic: " << economic << "\tTiming: " << timing << std::endl;
                }
            }
        }
        step_num += STEP_SIZE;
    }

    void OnUnitDestroyed(const sc2::Unit* unit) {
        // if (!halt_data && unit->owner == PLAYER_ID)   // Units are "created" when seen, so it "creates" minerals, neutral map mobs, etc.
        // {
        //  // std::cout << unit->owner << "\t" << unit->unit_type << std::endl;
        //  if (unit->unit_type == 84)
        //  {
        //      if (probes < 7) // Lets ignore worker-line harass
        //          ++probes;
        //      // else
        //      // halt_data = true;
        //  }
        //  else if (unit->unit_type == 311)
        //  {
        //      if (adepts < 3) // Lets ignore scouts dying
        //          ++adepts;
        //      // else
        //      // halt_data = true;
        //  }
        //  // else if (DesiredArmyUnit(unit)) // Lets ignore phase shifting for strategizer?
        //  // {
        //  // halt_data = true;
        //  // }
        // }

        // if (!halt_data && IsStructure(*unit))
        //  std::cout << unit->unit_type << std::endl;
    }

    void OnGameEnd() {
        if (file_write_flag) {
            CloseFile();
        }

        for (auto &vec : time_series) {
            vec.clear();
        }
    }

    float GetDistance(const sc2::Point2D &p1, const sc2::Point2D &p2) {
        return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    }


    sc2::Units GetBases(const sc2::Units &units) {
        sc2::Units ret_val;
        for (auto u : units) {
            if (u->unit_type == 59) // Nexus
                ret_val.push_back(u);
        }
        return ret_val;
    }

    sc2::Units GetArmyUnits(const sc2::Units &units) {
        sc2::Units ret_val;
        for (auto u : units) {
            if (DesiredArmyUnit(u))
                ret_val.push_back(u);
        }
        return ret_val;
    }

    unsigned int GetArmyValue(const sc2::ObservationInterface* obs, const sc2::Units &units) {
        // This may be built into the scoreboard if I can figure that out...
        unsigned int army_value = 0;
        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        for (auto u : units) {
            army_value += unit_types.at(u->unit_type).mineral_cost;
            army_value += unit_types.at(u->unit_type).vespene_cost;
        }
        return army_value;
    }

    unsigned int GetStructuresValue(const sc2::ObservationInterface* obs, const sc2::Units &units) {
        // This may be built into the scoreboard if I can figure that out...
        unsigned int struct_value = 0;
        const sc2::UnitTypes& unit_types = obs->GetUnitTypeData();
        for (auto u : units) {
            if (u->owner == PLAYER_ID && IsStructure(obs, *u)) {
                struct_value += unit_types.at(u->unit_type).mineral_cost;
                struct_value += unit_types.at(u->unit_type).vespene_cost;
            }
        }
        // std::cout << std::endl;
        return struct_value;
    }


    // Returns ANY army unit that is not one of the ones prohibited by the below list
    bool DesiredArmyUnit(const sc2::Unit*& u) {
        // Fix this. Utilize enum class UNIT_TYPEID
        return u->owner == PLAYER_ID && u->is_alive && u->unit_type != 801 && u->unit_type != 341 && u->unit_type != 665 && u->unit_type != 666
               && u->unit_type != 146 && u->unit_type != 147
               && u->unit_type != 483 && u->unit_type != 342 && u->unit_type != 608
               && u->unit_type != 343 && u->unit_type != 84
               && u->unit_type != 60 && u->unit_type != 61 && u->unit_type != 59 && u->unit_type != 72
               && u->unit_type != 62 && u->unit_type != 67;
    }

    // // I don't trust this filter function
    bool IsStructure (const sc2::ObservationInterface* obs, const sc2::Unit& unit) {
        auto& attributes = obs->GetUnitTypeData().at(unit.unit_type).attributes;
        bool is_structure = false;
        for (const auto& attribute : attributes) {
            if (attribute == sc2::Attribute::Structure) {
                is_structure = true;
            }
        }
        return is_structure;
    }


    std::vector<const sc2::Unit*> GetStructures(const sc2::ObservationInterface* obs, const sc2::Units &units) {
        std::vector<const sc2::Unit*> ret_val;
        ret_val.reserve(units.size());
        for (const sc2::Unit *u : units)
            if (u->owner == PLAYER_ID && IsStructure(obs, *u))
                ret_val.push_back(u);
        return ret_val;
    }

    // bool IsStructure(const sc2::Unit &unit)
    // {
    //  return (unit.owner == PLAYER_ID && unit.is_alive && (
    //          strcmp(UnitTypeToName(unit.unit_type), "PROTOSS_NEXUS") ||
    //          unit.unit_type == 60 || unit.unit_type == 62 || unit.unit_type == 63 ||
    //          unit.unit_type == 66 || unit.unit_type == 69 || unit.unit_type == 70 || unit.unit_type == 71 ||
    //          unit.unit_type == 67 || unit.unit_type == 68 || unit.unit_type == 65 || unit.unit_type == 133));

    // }

};

#endif