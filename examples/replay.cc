#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_manage_process.h"
#include "time_unit_created.h"

#include <iostream>
#include <fstream>

const char* kReplayFolder = "/home/kyle/Downloads/StarCraftII/PvZ/";


int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    // kReplayFolder will be replaced by input from a config file replay_folder.c_str()
    TimeUnitCreated replay_observer(kReplayFolder);

    if (!coordinator.SetReplayPath(replay_observer.kReplayFolder_)) {
        std::cout << "Unable to find replays." << std::endl;
        return 1;
    }

    coordinator.SetStepSize(STEP_SIZE);
    coordinator.AddReplayObserver(&replay_observer);

    while (coordinator.Update());
    while (!sc2::PollKeyPress());
}