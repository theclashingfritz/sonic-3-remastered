#ifndef S3_CollapsingPlatform_H
#define S3_CollapsingPlatform_H

#include <S3/Object.h>

class ICollapsingPlatform : public Object {
public:
    int f = -1;

    float tim[268]; // allotted - horizontal sections * vertical sections * frames of separation (allotted - 32)
    int allotted = 60 * 5;
    int separation = 2;
    int sections = 6;
    int vsect = 4;
    bool Reset = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);

    int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    bool CustomSolidityCheck(int, int, int, bool);
};

#endif /* S3_CollapsingPlatform_H */
