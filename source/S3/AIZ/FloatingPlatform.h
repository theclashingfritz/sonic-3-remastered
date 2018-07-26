#ifndef S3_FloatingPlatform_H
#define S3_FloatingPlatform_H

#include <S3/Object.h>

class IFloatingPlatform : public Object {
public:
    int t = 0;
    float mult = 0.0f;

    float myX = -1;
    float myY = -1;
    float LastSpeed = 0;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_FloatingPlatform_H */
