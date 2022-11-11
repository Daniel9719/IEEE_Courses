#ifndef QUEUE_MAIN_H
#define QUEUE_MAIN_H

#include <stdint.h>

typedef char* ID_t;

enum SigLvl{
    LOW = 0,
    MEDIUM = 10,
    HIGH = 30
};

struct Data_t
{
    ID_t ID_Val;
    uint8_t Data_Val;
};

// struct Data_t
// {
//     ID_t ID_Val;
//     enum SigLvl Data_Val;
// };

#endif
