#ifndef S3_MONITOR_H
#define S3_MONITOR_H

#include <S3/Object.h>

class IMonitor : public Object {
public:
    float GhostY = 0;
    bool  CanFall = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnBreakHorizontal(int PlayerID, int HitFrom);
    int  OnBreakVertical(int PlayerID, int HitFrom);
};

#endif /* S3_MONITOR_H */