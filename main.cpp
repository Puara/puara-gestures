
#include "puara_gestures.h"
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

PuaraGestures gestures;
int update_interval = 33; //milliseconds

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

int libloServerMethods(){
    // osc_server.add_method("/puaragestures/pos", "ff",
    //     [&](lo_arg **argv, int) {
    //         float x = argv[0]->f;
    //         float y = argv[1]->f;
    //         std::cout << "X: " << x << " Y: " << y << std::endl;
    //     });
    // osc_server.add_method("/livepose/skeletons/0/0/keypoints/LEFT_WRIST", "iif",
    //     [&](lo_arg **argv, int) {
    //         int x = argv[0]->i;
    //         int y = argv[1]->i;
    //         float conf = argv[0]->f;
    //         x = gestures.mapRange(x, 0, 640, -41, 41);
    //         y = gestures.mapRange(y, 0, 480, -41, 41);
    //         // std::cout 
    //         // << "X: "  << x 
    //         // << " Y: " << y
    //         // << " C: " << conf 
    //         // << std::endl;
    //         gestures.setAccelerometerValues(x, y, 0);
    //     });
        osc_server.add_method("/livepose/skeletons/0/0/keypoints/LEFT_WRIST", "iif",
        [&](lo_arg **argv, int) {
            int x = argv[0]->i;
            int y = argv[1]->i;
            float conf = argv[0]->f;
            float xf = gestures.mapRange(static_cast<float>(x), 0.0, 640.0, 0.0, 1.0);
            float yf = gestures.mapRange(static_cast<float>(y), 0.0, 480.0, 0.0, 1.0);
            // std::cout 
            // << "X: "  << xf 
            // << " Y: " << yf
            // << " C: " << conf 
            // << std::endl;
            floatToBinaryPositionArray(xf, binaryArray);
            gestures.updateTouchArray(binaryArray,30);
        });
        return 0;
}

void killlHandler(int signum) {
    std::cout << "\nClosing Puara Gestures..." << std::endl;
    keepRunning.store(false);
    cv.notify_all();
}

// void updateSensor() {
//     while (keepRunning) {
//         gestures.updateInertialGestures();
//         std::cout << gestures.getShakeX() << std::endl;
//         std::this_thread::sleep_for(std::chrono::milliseconds(update_interval));
//     }
// }

int main(int argc, char* argv[]) {

    libloServerMethods();

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

    while (1) {
        gestures.updateInertialGestures();
        // std::cout 
        //     // << " Acc: " << gestures.getAccelX()
        //     // << " Y: " << gestures.getGyroY() 
        //     // << " Z: " << gestures.getGyroZ() 
        //     // << " shake: " << gestures.getShakeX()
        //     // << " Y: " << gestures.getShakeY() 
        //     // << " Z: " << gestures.getShakeZ() 
        //     // << " X: " << gestures.getJabX() 
        //     // << " Y: " << gestures.getJabY() 
        //     // << " Z: " << gestures.getJabZ() 
        //     << std::endl;

        std::cout << "Binary Array: ";
        for (int i = 0; i < 30; ++i) {
            std::cout << binaryArray[i] << " ";
        }
        std::cout << "brush: " << gestures.brush
                  << " rub: " << gestures.rub << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(update_interval)); 
    }

    // {
    //     std::unique_lock<std::mutex> lock(mtx);
    //     cv.wait(lock, []{ return !keepRunning; });
    // }
    
    return 0;
}
