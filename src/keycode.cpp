// MIT License
//
// Copyright (c) 2022 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//
//

#include "../include/keycode.hpp"
#include "../libs/command/include/command.hpp"

#include <unistd.h>
#include <pwd.h>
#include <fstream>

#ifdef __has_include
#   if __has_include(<X11/Xlib.h>) || __has_include(<X11/xlib.h>)
#       include "../include/backends/xlib.hpp"
#       undef Status
#   endif
#endif

#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    if(argc >= 2) {
        std::cout << "keycode for " << __KEYCODE_IMPL__ << '\n';
        return 1;
    }

    device* data = new device();
    std::string home_path(getpwuid(getuid())->pw_dir);
    
    std::string file_data;

    if(home_path.back() != '/')
        home_path.push_back('/');

    if(std::filesystem::exists(home_path + ".keycode")) {
        std::ifstream file(home_path + ".keycode");
        for(std::string script_data; 
            std::getline(file, script_data); file_data.append(script_data + "\n"))
            ; file.close();

    } else {
        std::ofstream file(home_path + ".keycode");
        file << "# keycode script - variant set to 'keycode_data'\n";
        file.close();
    }

    while(true) {
        value opcode = data->get_opcode();
        std::string val = std::string(__CONVERT_TO_STRING__(opcode.opcode));
        
        if(opcode.is_release) {
            val.append("_Release"); 
            
            if(process::New("enigne")
                .Arg("-keycode_data=" + val)
                .Arg(home_path + ".keycode")
                .Status().error)
                break;
        }
    }

    delete data;

    return 0;
}