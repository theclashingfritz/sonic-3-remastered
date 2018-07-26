#ifndef S3_Cork_H
#define S3_Cork_H

#include <S3/Object.h>

class ICork : public Object {
public:
    float MiniCorkX[8];
    float MiniCorkY[8];
    float MiniCorkXS[8];
    float MiniCorkYS[8];
    int MiniCorks = 0;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    int  OnBreakHorizontal(int PlayerID, int HitFrom);
    int  OnBreakVertical(int PlayerID, int HitFrom);
};

#endif /* S3_Cork_H */
