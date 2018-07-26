#ifndef S3_Ridge_H
#define S3_Ridge_H

#include <S3/Object.h>

class IRidge : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_Ridge_H */
