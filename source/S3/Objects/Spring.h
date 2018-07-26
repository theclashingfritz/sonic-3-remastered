#ifndef S3_SPRING_H
#define S3_SPRING_H

#include <S3/Object.h>

class ISpring : public Object {
public:
    int Rotation = 0;
    int Animation = -1;
    int SpringPower = 16;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_SPRING_H */
