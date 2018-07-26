#ifndef S3_SPIKES_H
#define S3_SPIKES_H

#include <S3/Object.h>

class ISpikes : public Object {
public:
    int Rotation = 0;
    int Animation = 0;
    int Size = 1;
    bool Vertical = false;
    bool In = false;
    
    int Cos[4];
    int Sin[4];

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_SPIKES_H */
