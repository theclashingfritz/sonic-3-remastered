#ifndef S3_LogBridge_H
#define S3_LogBridge_H

#include <S3/Object.h>

class ILogBridge : public Object {
public:
    int f = -1;

    float tim[268]; // allotted - horizontal sections * vertical sections * frames of separation (allotted - 32)
    int allotted = 60 * 5;
    int separation = 8;
    int sections = 6;
    int vsect = 1;
    bool Reset = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    bool CustomSolidityCheck(int, int, int, bool);
};

#endif /* S3_LogBridge_H */