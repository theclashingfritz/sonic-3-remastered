#ifndef S3_Points_H
#define S3_Points_H

#include <S3/Object.h>

class IPoints : public Object {
public:
    float ghostY = 0;
    int ghostSp = 0;
    int ghostTim = 60;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};
class IEnemyExplosion : public Object {
public:
    int HSpeed = 0;
    bool CanFall = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_Points_H */
