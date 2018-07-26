#ifndef S3_RING_H
#define S3_RING_H

#include <S3/Object.h>

class IRing : public Object {
public:
    bool CanFall = false;
    bool CanBeCollected = true;
    int  Lifespan = -1;
    int  NonCollect = -1;

    int Glitter = 24;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int OnCollisionWithPlayer(int Data1, int Data2, int Data3);
};

#endif /* S3_RING_H */
