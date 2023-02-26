#ifndef MAIN_H
#define MAIN_H

#include "Godot.hpp"
#include "Node.hpp"

namespace godot {
    class Main : public Node {
        GODOT_CLASS(Main, Node)
    private:

    public:
        static void _register_methods();

        Main() = default;
        ~Main() = default;

        void _init() { };
        void _ready();

        void call_test(String val);
    };

}

#endif