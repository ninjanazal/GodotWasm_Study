#include "main.h"
#include <string>
#include "OS.hpp"

using namespace godot;

void Main::_register_methods() {
    register_method("_ready", &Main::_ready);
    register_method("call_test", &Main::call_test);
}

void Main::_ready() {
    Godot::print("Hello CPipi");
}

void Main::call_test(String val){
    Godot::print( "From GD script " + val);
    std::string test = val.alloc_c_string();

    Godot::print("----> " + String(test.c_str()));

    // C++ is 65x~ faster
    int init_time = OS::get_singleton()->get_ticks_msec();
    int accum = 0;
    for (int i = 0; i < 20000000; i++) {
        accum += i % 2 ;
    }
    Godot::print(std::to_string(OS::get_singleton()->get_ticks_msec() - init_time).c_str());
    
}