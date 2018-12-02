#pragma once

#include <utility>
#include <iostream>

class register_file_t {
public:
    // <value, tag>
    std::pair<int, int> r[8];

    register_file_t(void) {
        this->reset();
    }

    friend std::ostream& operator<<(std::ostream& os, register_file_t& rf) {
        auto pad_int = [](int i, int len) -> std::string {
            auto str = std::to_string(i);
            while(str.size() < len)
                str.push_back(' ');
            return str;
        };

        std::cout << "+----+-------------+-----+\n";
        std::cout << "|  v |    value    | tag |\n";
        std::cout << "+----+-------------+-----+\n";

        for(int i = 0; i < 8; i++) {
            std::cout << "|  " << i << " | " << pad_int(rf.value(i), 11) << " |";
            if(rf.tag(i) == -1) {
                std::cout << "  X  |\n";
            } else {
                std::cout << " RS" << rf.tag(i) << " |\n";
            }
        }

        return os;
    }

    void reset(void) {
        for(int i = 0; i < 8; i++) {
            r[i].first = 0;
            r[i].second = -1; // not claimed
        }
    }

    int& operator[](int index) {
        return this->r[index].first;
    }

    // can modify value through this anyway
    int& value(int index) {
        return this->r[index].first;
    }

    // reference because each issue will modify it anyway
    int& tag(int index) {
        return this->r[index].second;
    }
};