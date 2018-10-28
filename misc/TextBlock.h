#ifndef __JJC__TEXT__BLOCK__H__
#define __JJC__TEXT__BLOCK__H__

// TextBlock defines a section of printable 
// space, data is inserted at specific 
// locations, taking into account newlines 
// and other unique text chars

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>

class TextBlock {
private:
    std::vector<char> __text;
    int x = 0, y = 0; // size of text block to allocate

public:
    TextBlock(int x, int y) {
        this->x = x;
        this->y = y;

        int sz = x*y;
        this->__text.resize(sz);
        std::cerr << "TextBlock size: " << __text.size() << std::endl;
        std::cerr << "  requested size: " << sz << std::endl;
    }

    void reset(void) {
        memset(&this->__text[0], ' ', x*y);
    }

    void insert(int x, int y, char c) {
        __text[y*this->x + x] = c;
    }

    // useful for inserting data of many types
    // ... std::vector<char>, const char*, std::string::c_str()
    void insert(int x, int y, char* str) {
        int sz = strlen(str);

        int current_y_offset = 0;
        int current_x_offset = 0;

        for(int i = 0; i < sz; i++) {
            if(str[i] == '\n') {
                current_y_offset++;
                current_x_offset = 0;
            } 
            else {
                this->insert(x + current_x_offset, y + current_y_offset, str[i]);
                current_x_offset++;
            }
        }
    }

    void insert(int x, int y, std::stringstream& ss) {
        this->insert(x, y, (char*)ss.str().c_str());
    }

    // only printing function needed
    friend std::ostream& operator<<(std::ostream& os, TextBlock& tb) {

        //for(int i = 0; i < tb.y; i++) {
        //    for(int j = 0; j < tb.x; j++) {
        //        os << tb.__text[i*tb.x + j];
        //    }
        //    os << std::endl;
        //}

        //for(int i = 0; i < tb.y; i++) {
        //    os.write((&tb.__text[0]) + (tb.x*i), tb.x);
        //    os << std::endl;
        //}

        os.write((const char*)&tb.__text[0], tb.x * tb.y);

        return os;
    }

};

#endif // __JJC__TEXT__BLOCK__H__