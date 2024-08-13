
//********************************************************************************//
// Puara Gestures standalone - Receive OSC data to generate high-level            //
//                             gestural descriptors                               //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
//********************************************************************************//

#include "../puara_gestures.h"

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/osc.hpp>

#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>

#include <iostream>
#include <sstream>

#include <vector>
#include <chrono>
#include <thread>
#include "../descriptors/IMU_Sensor_Fusion/imu_orientation.h"

std::string client_ip = "127.0.0.1";
int client_port = 9000;
int local_port = 9001;
std::string device_name = "Puara_Gestures";

ossia::net::generic_device device {
    std::make_unique<ossia::net::osc_protocol>(
        client_ip, client_port, local_port),
        device_name
};

puara_gestures::Shake3D shake;
puara_gestures::Jab3D jab;
puara_gestures::utils::LeakyIntegrator leakyintegrator;
IMU_Orientation orientation;
puara_gestures::testing_roll rollTest;
puara_gestures::testing_tilt tiltTest;

auto cb = [](ossia::string_view unhandled_node, const ossia::value& unhandled_value) {
    std::cout
        << "Received unhandled message ("
        << unhandled_node << " => "
        << ossia::value_to_pretty_string(unhandled_value) <<  ")."
        << std::endl;
    };

int main(int argc, char* argv[]) {

    // creating response for unhandled messages
    device.on_unhandled_message.connect(&cb);

    // creating accelerometer 3D node
    auto& accelNode = find_or_create_node(device, "/puaragestures/accel3D");
    auto accelParam = accelNode.create_parameter(ossia::val_type::LIST);
    accelParam->add_callback([&](const ossia::value& v) {
        std::cout << "New accelerometer value received: " << ossia::value_to_pretty_string(v) << std::endl;
        ossia::vec3f accelerometer = ossia::convert<ossia::vec3f>(v);
        // shake.update(accelerometer[0], accelerometer[1], accelerometer[2]);
        // jab.update(accelerometer[0], accelerometer[1], accelerometer[2]);
        leakyintegrator.integrate(accelerometer[0]);
    });

    rollTest.test();
    // tiltTest.test();

//     while(true) {
//         puara_gestures::Coord3D shakeout = shake.current_value();
//         puara_gestures::Coord3D jabout = jab.current_value();
//         std::cout << "Shake X: " << shakeout.x << ", Jab X: " << jabout.x << ", Integrator: " << leakyintegrator.current_value << std::endl;
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//   };
}