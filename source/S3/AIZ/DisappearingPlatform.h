#ifndef S3_DisappearingPlatform_H
#define S3_DisappearingPlatform_H

#include <S3/Object.h>

class IDisappearingPlatform : public Object {
public:
    ISprite* Sprite2 = NULL;
    
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_DisappearingPlatform_H */
