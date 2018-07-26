#ifndef S3_BREAKABLEWALL_H
#define S3_BREAKABLEWALL_H

#include <S3/Object.h>

class IBreakableWall : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    int  OnBreakHorizontal(int PlayerID, int HitFrom);
    int  OnBreakVertical(int PlayerID, int HitFrom);
};

#endif /* S3_BREAKABLEWALL_H */
