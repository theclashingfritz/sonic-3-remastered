#ifndef S3_FallingLog_H
#define S3_FallingLog_H

#include <S3/Object.h>

class IFallingLog : public Object {
public:
    int StartX;
    int StartY;
    int OffsetY;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_FallingLog_H */
