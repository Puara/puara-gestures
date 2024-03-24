
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
// #include <ossia/network/common/debug.hpp>

#include <iostream>

#include <chrono>
#include <thread>

std::string client_ip = "127.0.0.1";
int client_port = 9000;
int local_port = 9001;
std::string device_name = "Puara_Gestures";

// Print unhandled incoming messages
auto cb = [](ossia::string_view v, const ossia::value& val) {
    std::cout << "Unhandled message: " << v << " => " << ossia::value_to_pretty_string(val) << std::endl;
};

int main(int argc, char* argv[]) {

    int i = 0;

    ossia::net::generic_device device {
        std::make_unique<ossia::net::osc_protocol>(
            client_ip, client_port, local_port), device_name
    };

    device.on_unhandled_message.connect(&cb);
    
    auto& node = find_or_create_node(device, "/foo/bar");
    auto param = node.create_parameter(ossia::val_type::INT);

    param->add_callback([&](const ossia::value& v) {
        std::cout << "New value received: " << ossia::value_to_pretty_string(v) << std::endl;
        i = ossia::convert<int>(v); 
    });

    while(true) {

        param->push_value(i); i += 1;

        std::this_thread::sleep_for(std::chrono::seconds(1));
  };
}
