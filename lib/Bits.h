#ifndef __JJC__BITS__H__
#define __JJC__BITS__H__

#include <stdint.h>
#include <string>
#include <vector>

namespace Bits {

template<typename T = uint32_t>
auto chunk(int bits_on, int bit_offset) -> T {
    T ret = 0;
    for(int i = 0; i < bits_on; i++) {
        ret |= (1 << i);
    }

    ret <<= bit_offset;
    return ret;
}

template<typename T = uint32_t>
auto bitPattern(T patn, int bit_size) -> std::string {
    std::vector<char> bit_str;
    T mask = 1;

    for(int i = 0; i < bit_size; i++) {
        bit_str.push_back((patn >> (bit_size-i-1)) & mask ? '1' : '0');
    }

    bit_str.push_back('\0');

    std::string s = &bit_str[0];
    return s;
}

} // end of namespace Bits

#endif // __JJC__BITS__H__
