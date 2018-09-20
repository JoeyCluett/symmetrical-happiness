#ifndef __JJC__MACROS__H__
#define __JJC__MACROS__H__

// tired of writing out the loops
#define LOOP(var, start, end) for(int var = start; var < end; var++)

#define PAUSE getch

#include <string>

std::string SPACES(int n) {
    std::string sp = "";
    LOOP(i, 0, n)
        sp += ' ';
    return sp;
}

#endif // __JJC__MACROS__H__