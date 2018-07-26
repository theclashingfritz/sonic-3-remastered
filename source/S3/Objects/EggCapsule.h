#ifndef S3_EGGCAPSULE_H
#define S3_EGGCAPSULE_H

#include <S3/Object.h>

class IEggCapsule : public Object {
public:
    int Rotation = 0;
    int Animation = 0;
    bool CanFall = true;

    ITexture* Texture = NULL;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_EGGCAPSULE_H */
