#ifndef S3_ForegroundPlant_H
#define S3_ForegroundPlant_H

#include <S3/Object.h>

class IForegroundPlant : public Object {
public:
    int CamMult;
    int StartX;
    int StartY;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_ForegroundPlant_H */
