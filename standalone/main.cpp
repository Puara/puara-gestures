
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

#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

std::string client_ip = "127.0.0.1";
int client_port = 9000;
int local_port = 9001;
std::string device_name = "Puara_Gestures";

ossia::net::generic_device device {
    std::make_unique<ossia::net::osc_protocol>(
        client_ip, client_port, local_port), 
        device_name
};

std::unordered_map<std::string, puara_gestures::Shake> shake;
std::unordered_map<std::string, puara_gestures::Jab> jab;


auto cb = [](ossia::string_view unhandled_node, const ossia::value& unhandled_value) {
    std::cout 
        << "Received new message. Creating node and associated gestures (" 
        << unhandled_node << " => " 
        << ossia::value_to_pretty_string(unhandled_value) <<  ")."
        << std::endl; 
    if (unhandled_value.get_type() != ossia::val_type::INT && unhandled_value.get_type() != ossia::val_type::FLOAT) {
        std::cout << "Not creating node. Wrong value type (expecting single float or integer)" << std::endl;
        return;
    }
    auto& node = create_node(device, unhandled_node);
    auto param = node.create_parameter(unhandled_value.get_type());
    switch (unhandled_value.get_type()) {
        case ossia::val_type::INT:
            // for now do nothing but need to add button stuff here
            break;
        case ossia::val_type::FLOAT:
            shake.emplace(unhandled_node, puara_gestures::Shake());
            jab.emplace(unhandled_node, puara_gestures::Jab());
            param->add_callback([=](const ossia::value& received_value) {
                std::cout << "New value received: " << ossia::value_to_pretty_string(received_value) << std::endl;
                std::cout << "index: " << param->get_node().get_name() << std::endl;
                // std::cout << "shake: " << 
                //     shake.at(param->get_node().get_name()).update(ossia::convert<float>(received_value))
                //     << std::endl;
            });
            break;
    }
};

int main(int argc, char* argv[]) {

    

    device.on_unhandled_message.connect(&cb);
    
    // auto& node = find_or_create_node(device, "/foo/bar");
    // auto param = node.create_parameter(ossia::val_type::INT);

    // param->add_callback([&](const ossia::value& v) {
    //     std::cout << "New value received: " << ossia::value_to_pretty_string(v) << std::endl;
    //     i = ossia::convert<int>(v); 
    // });

    while(true) {

        //param->push_value(i); i += 1;

        std::this_thread::sleep_for(std::chrono::seconds(1));
  };
}
