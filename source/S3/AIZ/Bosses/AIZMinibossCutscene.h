#ifndef S3_AIZMinibossCutscene_H
#define S3_AIZMinibossCutscene_H

#include <S3/Object.h>

class IAIZMinibossCutscene : public Object {
public:
    int FirstRun = -1;

    float Gravity = 0;

    int Flip = 1;

    int State = 0;
    int TimerAction = -1;
    int HitTimer = -1;

    int HitCount = 600;

    int TimerForFire = 0;

    ISprite* SpriteFlame = NULL;
    ISprite* SpriteSmall = NULL;

    ISprite* SpriteTransition = NULL;

    ITexture* Palette = NULL;
    ITexture* pal = new ITexture();

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_AIZMinibossCutscene_H */
