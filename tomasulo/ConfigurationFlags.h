#pragma once

#include <string>
#include <map>

class ConfigurationFlags {
private:
    std::map<std::string, std::string> M_flag_map = {
        {"broadcastpriority",             "UNDEFINED"},
        {"simultaneousdispatchcommit",    "UNDEFINED"},
        {"simultaneousbroadcastdispatch", "UNDEFINED"},
        {"broadcastnextcycle",            "UNDEFINED"}
    };

    bool entryIsOneOf(std::string entry_name, std::initializer_list<std::string> one_of) {
        std::string value = this->M_flag_map.at(entry_name);
        for(auto& str : one_of)
            if(str == value)
                return true;
        return false;
    }

public:


};