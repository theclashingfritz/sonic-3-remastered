#ifndef S3_Rock_H
#define S3_Rock_H

#include <S3/Object.h>

class IRock : public Object {
public:
    float MiniRockX[8];
    float MiniRockY[8];
    float MiniRockXS[8];
    float MiniRockYS[8];
    float MiniRockF[8];
    int MiniRocks = 0;

    int StartX;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnPush(int PlayerID, int Direction);
    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    int  OnBreakHorizontal(int PlayerID, int HitFrom);
    int  OnBreakVertical(int PlayerID, int HitFrom);
};

#endif /* S3_Rock_H */
