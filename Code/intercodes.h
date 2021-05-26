#ifndef INTERCODES_H_
#define INTERCODES_H_
typedef struct InterCodes_* InterCodes;
#include "type.h"

struct InterCodes_
{
    InterCode code;
    InterCodes* prev, next;
};

InterCodes code_insert(InterCodes cur);
InterCodes code_print(InterCodes root);

#endif