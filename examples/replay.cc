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

    if (!coordinator.SetReplayPath(kReplayFolder)) {
        std::cout << "Unable to find replays." << std::endl;
        return 1;
    }

    TimeUnitCreated replay_observer;
    coordinator.SetStepSize(STEP_SIZE);
    coordinator.AddReplayObserver(&replay_observer);

    while (coordinator.Update());
    while (!sc2::PollKeyPress());
}