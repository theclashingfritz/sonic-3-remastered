#ifndef S3_STARPOLE_H
#define S3_STARPOLE_H

#include <S3/Object.h>

class IStarpole : public Object {
public:
    int Rotation = 0;
    int Animation = -1;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_STARPOLE_H */
