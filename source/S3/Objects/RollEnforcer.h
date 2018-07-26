#ifndef S3_RollEnforcer_H
#define S3_RollEnforcer_H

#include <S3/Object.h>

class IRollEnforcer : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_RollEnforcer_H */
