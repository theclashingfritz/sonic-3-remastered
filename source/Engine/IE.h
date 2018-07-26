#ifndef IE_H
#define IE_H

#include "IStandardHeaders.h"

class IE {
public:
    enum Flag {
        LeftAlign   = 0x10,
        CenterAlign = 0x20,
        RightAlign  = 0x40,
        TopAlign    = 0x100,
        MiddleAlign = 0x200,
        BottomAlign = 0x400,
    };
};

struct IPoint {
    float X;
    float Y;
};

#endif // IE_H
