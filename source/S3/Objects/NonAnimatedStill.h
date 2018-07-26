#ifndef S3_NONANIMATEDSTILL_H
#define S3_NONANIMATEDSTILL_H

#include <S3/Object.h>

class INonAnimatedStill : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_NONANIMATEDSTILL_H */
