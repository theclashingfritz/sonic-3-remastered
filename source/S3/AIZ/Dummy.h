#ifndef S3_Dummy_H
#define S3_Dummy_H

#include <S3/Object.h>

class IDummy : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_Dummy_H */
