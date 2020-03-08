#ifndef CONSTANT_H
#define CONSTANT_H

const long BILLION=1000000000l;

inline long timespecToNano(timespec t) {
    return BILLION * t.tv_sec + t.tv_nsec;
}

#endif
