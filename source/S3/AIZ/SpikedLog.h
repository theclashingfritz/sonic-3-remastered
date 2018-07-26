#ifndef S3_SpikedLog_H
#define S3_SpikedLog_H

#include <S3/Object.h>

class ISpikedLog : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
};

#endif /* S3_SpikedLog_H */
