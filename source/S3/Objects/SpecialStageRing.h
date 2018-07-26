#ifndef S3_SpecialStageRing_H
#define S3_SpecialStageRing_H

#include <S3/Object.h>

class ISpecialStageRing : public Object {
public:
    int  Anim = 0;
    bool SuperEmeraldRing = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int Data1, int Data2, int Data3);
};

#endif /* S3_SpecialStageRing_H */
