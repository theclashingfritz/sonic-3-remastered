#ifndef S3_SwingVine_H
#define S3_SwingVine_H

#include <S3/Object.h>

class ISwingVine : public Object {

    public:
        float StartX;
        float StartY;
        float BaseX;
        float BaseY;
        float Angle;
        float PotentialEnergy;

        void Create();
        void Update();
        void Render(int CamX, int CamY);

        int OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_SwingVine_H */