#ifndef S3_INVISIBLESOLID_H
#define S3_INVISIBLESOLID_H

#include <S3/Object.h>

class IInvisibleSolid : public Object {
public:
    int Size = 1;
    int Rotation = 0;
    int Animation = 0;
    bool Painful = false;
    bool Deadly = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_INVISIBLESOLID_H */
