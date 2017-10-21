#include "sc2api/sc2_replay_observer.h"
#include "sc2api/sc2_control_interfaces.h"
#include "sc2api/sc2_proto_to_pods.h"
#include "sc2api/sc2_game_settings.h"

#include <iostream>

namespace sc2 {

//-------------------------------------------------------------------------------------------------
// ReplayObserver: An implementation of ReplayControlInterface.
//-------------------------------------------------------------------------------------------------

class ReplayControlImp : public ReplayControlInterface {
public:
    ReplayInfo replay_info_;
    ControlInterface* control_interface_;
    ReplayObserver* replay_observer_;

    ReplayControlImp(ControlInterface* control_interface, ReplayObserver* replay_observer);

    virtual bool GatherReplayInfo(const std::string& path, bool download_data) override;
    virtual bool LoadReplay(const std::string& replay_path, const InterfaceSettings& settings, uint32_t player_id) override;
    virtual bool WaitForReplay() override;
    virtual void UseGeneralizedAbility(bool value) override;

    virtual const ReplayInfo& GetReplayInfo() const override;
};

ReplayControlImp::ReplayControlImp(ControlInterface* control_interface, ReplayObserver* replay_observer) :
    control_interface_(control_interface),
    replay_observer_(replay_observer) {
}

bool ReplayControlImp::GatherReplayInfo(const std::string& path, bool download_data) {
    replay_info_.num_players = 0;

    // Request the replay info.
    GameRequestPtr request = control_interface_->Proto().MakeRequest();
    SC2APIProtocol::RequestReplayInfo* request_replay_info = request->mutable_replay_info();
    request_replay_info->set_replay_path(path);
    request_replay_info->set_download_data(download_data);
    if (!control_interface_->Proto().SendRequest(request)) {
        return false;
    }

    // Check that the response is properly filled out.
    GameResponsePtr response = control_interface_->WaitForResponse();
    if (!response) {
        std::cerr << "No response to replay info query!" << std::endl;
        return false;
    }

    if (!response->has_replay_info()) {
        for (auto e : response->error()) {
            std::cout << e << std::endl;
        }
        std::cerr << "Replay info expected and not returned: " << response->DebugString() << std::endl;
        return false;
    }
    const SC2APIProtocol::ResponseReplayInfo& proto_replay_info = response->replay_info();

    std::string map_name = proto_replay_info.map_name();
    std::string map_path = proto_replay_info.local_map_path();
    std::string version = proto_replay_info.game_version();
    std::string data_version = proto_replay_info.data_version();

    if (map_name.length() >= max_path_size) {
        std::cerr << "Map name is too long: " << map_name << std::endl;
        return false;
    }
    if (map_path.length() >= max_path_size) {
        std::cerr << "Map path is too long: " << map_path << std::endl;
        return false;
    }
    if (version.length() >= max_version_size) {
        std::cerr << "Version string is too long: " << version << std::endl;
        return false;
    }

    replay_info_.map_name = map_name.c_str();
    replay_info_.map_path = map_path.c_str();
    replay_info_.replay_path = path.c_str();
    replay_info_.version = version.c_str();
    replay_info_.data_version = data_version.c_str();

    replay_info_.duration = proto_replay_info.game_duration_seconds();
    replay_info_.duration_gameloops = proto_replay_info.game_duration_loops();

    replay_info_.data_build = proto_replay_info.data_build();
    replay_info_.base_build = proto_replay_info.base_build();

    for (int i = 0; i < proto_replay_info.player_info_size(); ++i) {
        const SC2APIProtocol::PlayerInfoExtra& player_info_extra_proto = proto_replay_info.player_info(i);
        ReplayPlayerInfo player_info;

        if (player_info_extra_proto.has_player_info()) {
            const SC2APIProtocol::PlayerInfo& player_info_proto = player_info_extra_proto.player_info();
            player_info.player_id = player_info_proto.player_id();
            if (player_info_proto.has_race_actual()) {
                player_info.race = ConvertRaceFromProto(player_info_proto.race_actual());
            }
            if (player_info_proto.has_race_requested()) {
                player_info.race_selected = ConvertRaceFromProto(player_info_proto.race_requested());
            }
        }

        player_info.mmr = player_info_extra_proto.player_mmr();
        player_info.apm = player_info_extra_proto.player_apm();

        if (player_info_extra_proto.has_player_result()) {
            const SC2APIProtocol::PlayerResult& player_result_proto = player_info_extra_proto.player_result();
            if (player_result_proto.has_result()) {
                player_info.game_result = ConvertGameResultFromProto(player_result_proto.result());
            }
        }

        if (replay_info_.num_players >= max_num_players) {
            // Something went wrong, too many players.
            return false;
        }

        replay_info_.players[replay_info_.num_players] = player_info;
        ++replay_info_.num_players;
    }

    return true;
}

bool ReplayControlImp::LoadReplay(const std::string& replay_path, const InterfaceSettings& settings, uint32_t player_id) {
    // Send the request.
    GameRequestPtr request = control_interface_->Proto().MakeRequest();
    SC2APIProtocol::RequestStartReplay* start_replay_request = request->mutable_start_replay();
    start_replay_request->set_replay_path(replay_path);
    start_replay_request->set_observed_player_id(player_id);

    SC2APIProtocol::InterfaceOptions* options = start_replay_request->mutable_options();
    options->set_raw(true);
    options->set_score(true);
    if (settings.use_feature_layers) {
        SC2APIProtocol::SpatialCameraSetup* setupProto = options->mutable_feature_layer();
        setupProto->set_width(settings.feature_layer_settings.camera_width);
        SC2APIProtocol::Size2DI* resolution = setupProto->mutable_resolution();
        resolution->set_x(settings.feature_layer_settings.map_x);
        resolution->set_y(settings.feature_layer_settings.map_y);
        SC2APIProtocol::Size2DI* minimap_resolution = setupProto->mutable_minimap_resolution();
        minimap_resolution->set_x(settings.feature_layer_settings.minimap_x);
        minimap_resolution->set_y(settings.feature_layer_settings.minimap_y);
    }
    if (settings.use_render) {
        SC2APIProtocol::SpatialCameraSetup* setupProto = options->mutable_render();
        SC2APIProtocol::Size2DI* resolution = setupProto->mutable_resolution();
        resolution->set_x(settings.render_settings.map_x);
        resolution->set_y(settings.render_settings.map_y);
        SC2APIProtocol::Size2DI* minimap_resolution = setupProto->mutable_minimap_resolution();
        minimap_resolution->set_x(settings.render_settings.minimap_x);
        minimap_resolution->set_y(settings.render_settings.minimap_y);
    }

    if (!control_interface_->Proto().SendRequest(request)) {
        std::cerr << "LoadReplay: load replay request failed." << std::endl;
        assert(0);
        return false;
    }

    return true;
}

bool ReplayControlImp::WaitForReplay() {
    // Wait for a response.
    GameResponsePtr response = control_interface_->WaitForResponse();
    if (!response.get()) {
        std::cerr << "WaitForReplay: timed out, did not receive any response." << std::endl;
        assert(0);
        return false;
    }

    if (!response->has_start_replay()) {
        std::cerr << "WaitForReplay: received the wrong type of response: " << std::to_string(int(response->response_case())) << std::endl;
        assert(0);
        return false;
    }

    const SC2APIProtocol::ResponseStartReplay& response_replay = response->start_replay();
    if (response_replay.has_error()) {
        SC2APIProtocol::ResponseStartReplay_Error err = response_replay.error();
        std::cerr << "WaitForReplay: start replay contains an error: " << std::to_string(int(err)) << std::endl;
        if (response_replay.has_error_details())
            std::cerr << "WaitForReplay: error details: " << response_replay.error_details() << std::endl;
        return false;
    }

    assert(control_interface_->IsInGame());
    if (!control_interface_->IsInGame()) {
        std::cerr << "WaitForReplay: not in a game." << std::endl;
        return false;
    }

    if (replay_info_.replay_path.empty()) {
        assert(0);
        std::cout << "WaitForReplay: new replay loaded, replay path unknown" << std::endl;
        return true;
    }

    control_interface_->GetObservation();
    replay_observer_->Control()->OnGameStart();
    replay_observer_->OnGameStart();

    std::cout << "Replaying: '" << replay_info_.replay_path << "'" << std::endl;
    return true;
}

void ReplayControlImp::UseGeneralizedAbility(bool value) {
    control_interface_->UseGeneralizedAbility(value);
}

const ReplayInfo& ReplayControlImp::GetReplayInfo() const {
    return replay_info_;
}

//-------------------------------------------------------------------------------------------------
// ReplayObserver.
//-------------------------------------------------------------------------------------------------

ReplayObserver::ReplayObserver() :
    replay_control_imp_(nullptr) {
    replay_control_imp_ = new ReplayControlImp(Control(), this);
}

ReplayObserver::~ReplayObserver() {
    delete replay_control_imp_;
}

ReplayControlInterface* ReplayObserver::ReplayControl() {
    return replay_control_imp_;
}

bool ReplayObserver::UndesirableReplay(const ReplayInfo& replay_info, int p0_mmr, int p1_mmr, int p0_apm, int p1_apm, 
                                   int winner, float duration, int r0, int r1) {
    if (replay_info.players[0].mmr < p0_mmr || replay_info.players[1].mmr < p1_mmr ||
        replay_info.players[0].apm < p0_apm || replay_info.players[1].apm < p1_apm || replay_info.duration < duration)
        return true;

    if (r0 != -1)
        if (replay_info.players[0].race != r0)
            return true;
    if (r1 != -1)
        if (replay_info.players[1].race != r1)
            return true;

    if (winner != -1)
    {
        if (winner == 0 && replay_info.players[0].game_result != GameResult(Win))
            return true;
        else if (replay_info.players[1].game_result != GameResult(Win))
            return true;
        else
            // Why would this happen...well, it wouldn't with the replays we currently have/use.
            return true;
    }

    return false;
}

void ReplayObserver::SetClassData(const ReplayInfo& replay_info)
{
    replay_observer_file_path_ = replay_info.replay_path;
    game_duration_ = replay_info.duration;
    game_loops_ = replay_info.duration_gameloops;
}

bool ReplayObserver::IgnoreReplay(const ReplayInfo& replay_info, uint32_t& /*player_id*/) {
    SetClassData(replay_info);

    // If we just want a count of viable replays based on our criteria uncomment this section.
    // TURN THIS INTO AN EXECUTABLE ONCE WE GET THE CONFIG FILE WORKING

    // static int count = 0;   
    // if (!UndesirableReplay(replay_info, 4000, 4000, 50, 50, -1, 60.0, Race(Protoss), Race(Zerg))) {
    //     std::cout << count++ << std::endl;
    // }
    // return true;

    return UndesirableReplay(replay_info, 4000, 4000, 50, 50, -1, 45.0, Race(Protoss), Race(Zerg));
}


float ReplayObserver::GetGameSecond(const int& step_no){
    return step_no / (float(game_loops_) / game_duration_);
}


void ReplayObserver::SetControl(ControlInterface* control) {
    replay_control_imp_->control_interface_ = control;
}

}
