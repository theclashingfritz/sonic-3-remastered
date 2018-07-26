#ifndef S3_ANIMATEDSTILL_H
#define S3_ANIMATEDSTILL_H

#include <S3/Object.h>

class IAnimatedStill : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_ANIMATEDSTILL_H */
