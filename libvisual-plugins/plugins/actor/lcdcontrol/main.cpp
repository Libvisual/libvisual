
#include <string>
#include <iostream>

#include <luascript/luascript.h>
#include "PluginUptime.h"

using namespace LCD;

class Main {
    Main()
    {

    }
};

extern "C" {

/*
int main(int argc, char **argv)
{
    lua *state = new lua();
    PluginUptime *uptime = new PluginUptime(state); 
    
    std::string str = "return LCD.uptime('%d d %H:%M:%S')";
    std::string val = "error";
    try {
        state->exec("function __wrap__() " + str + " end; __out__ = __wrap__()");
        val = state->get_variable<lua::string_arg_t>("__out__").value();
    } catch (lua::exception &e)
    {
        std::cout << "Lua error: " << e.error() << " line: " << e.line() << "\n";
    }

    std::cout << val << "\n";

    return 0;

}
*/
}
