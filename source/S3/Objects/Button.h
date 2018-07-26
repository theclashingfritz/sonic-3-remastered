#ifndef S3_BUTTON_H
#define S3_BUTTON_H

#include <S3/Object.h>

class IButton : public Object {
public:
    bool Down = false;
    bool Pressed = false;
    int  StartY;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_BUTTON_H */
