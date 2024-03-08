
//****************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level        //
//                             gestural descriptors                           //
// https://github.com/Puara/puara-gestures                                    //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2023) - https://www.edumeneses.com                            //
//****************************************************************************//

#include "../puara_gestures.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <vector>
#include "lo/lo.h"
#include "lo/lo_cpp.h"

int osc_server_port = 9000;
int update_interval = 33; //milliseconds

PuaraGestures gestures;

std::atomic<bool> keepRunning(true);
std::condition_variable cv;
std::mutex mtx;

std::vector<std::thread> threads;

lo::ServerThread osc_server(osc_server_port);

int binaryArray[30];

void floatToBinaryPositionArray(float inputValue, int binaryArray[30]) {
    if (inputValue < 0 || inputValue > 1) {
        std::cerr << "Error: Input value must be between 0 and 1." << std::endl;
        return;
    }

    // Convert position to array index
    int arrayIndex = static_cast<int>(inputValue * 30);

    // Ensure the index is within bounds
    arrayIndex = std::min(std::max(arrayIndex, 0), 29);

    // Set the corresponding position to 1
    for (int i = 0; i < 30; ++i) {
        binaryArray[i] = (i == arrayIndex) ? 1 : 0;
    }
}

int oscFunctions(){
    osc_server.add_method("/puaragestures/1DoF", "f",
    [&](lo_arg **argv, int) {
        float dof = argv[0]->f;
        floatToBinaryPositionArray(dof, binaryArray);
        gestures.updateTouchArray(binaryArray,30);
    });
    osc_server.add_method("/livepose/skeletons/0/0/keypoints/RIGHT_WRIST", "fff",
    [&](lo_arg **argv, int) {
        float dof = argv[0]->f;
            //std::cout << "data: " << dof << std::endl;
            floatToBinaryPositionArray(dof, binaryArray);
            gestures.updateTouchArray(binaryArray,30);
    });
    // osc_server.add_method("/AMIwrist_001/raw/accl", "fff",
    // [&](lo_arg **argv, int) {
    //     float dof = argv[0]->f;
    //     floatToBinaryPositionArray(dof, binaryArray);
    //     gestures.updateTouchArray(binaryArray,30);
    // });
    return 0;
}

void killlHandler(int signum) {
    std::cout << "\nClosing Puara Gestures..." << std::endl;
    keepRunning.store(false);
    cv.notify_all();
}

void updateSensorsThread() {
    while (keepRunning.load()) {
        gestures.updateInertialGestures();
    }
}

void printBrushThread() {
    while (keepRunning.load()) {
        if (gestures.brush > 0.5) std::cout << "Swipe: " << gestures.brush << std::endl;
    }
}

int main(int argc, char* argv[]) {

    oscFunctions();

    if (!osc_server.is_valid()) {
        std::cerr << "Error, liblo OSC server did not initialized correctly."
                << " This program cannot accept OSC messages" << std::endl;
    } else {
        osc_server.start();
    }

    std::signal(SIGINT, killlHandler);

    gestures.setGyroscopeValues(0.0, 0.0, 0.0);
    gestures.setAccelerometerValues(0.0, 0.0, 0.0);
    gestures.setMagnetometerValues(0.0, 0.0, 0.0);

    threads.emplace_back(updateSensorsThread);
    threads.emplace_back(printBrushThread);

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return !keepRunning; });
    }
    
    return 0;
}
