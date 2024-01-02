#ifndef BITOPERATIONS_HPP
#define BITOPERATIONS_HPP

#include <cstdint>

typedef uint64_t U64;

// Function to first the index of the least significant bit
int findLSBIndex (U64 value) {
    
    // No set bits exist
    if (value == 0) return -1; 

    int pos = 0;
    while (!(value & 1)) {
        value >>= 1; 
        pos++;
    }
    
    return pos;
}


#endif //BITOPERATIONS_HPP