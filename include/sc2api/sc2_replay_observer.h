/*! \file sc2_replay_observer.h
    \brief A client running a replay.
*/
#pragma once

#include "sc2_client.h"
// #include "sc2_gametypes.h"

#include <string>

namespace sc2 {

class ReplayControlImp;
class ReplayControlInterface;
class ControlInterface;
struct ReplayInfo;

//! A client for running a replay.
class ReplayObserver : public Client {
public:
    ReplayObserver();
    ~ReplayObserver() override;

    //! Obtains the replay control interface.
    //!< \return The replay control interface.
    ReplayControlInterface* ReplayControl();

    //! Determines if the replay should be filtered out.
    //! \param replay_info Replay information used to decide if the replay should be filtered.
    //!< \return If 'true', the replay will be rejected and not analyzed.
    virtual bool IgnoreReplay(const ReplayInfo& replay_info, uint32_t& player_id);

    void SetControl(ControlInterface* control);
    bool UndesirableReplay(const ReplayInfo& replay_info);//, int p0_mmr = 0, int p1_mmr = 0, int p0_apm = 10, int p1_apm = 10, 
                                   // int winner = -1, float duration = 0.0f, int r0 = -1, int r1 = -1);
    void SetClassData(const ReplayInfo& replay_info);
    float GetGameSecond(const int& step_no);

    std::string replay_observer_file_path_;
    float game_duration_ = 0.0f;
    unsigned int game_loops_ = 0;
    
private:
    ReplayControlImp* replay_control_imp_;
};

}
