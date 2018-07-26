#ifndef S3_RhinoBot_H
#define S3_RhinoBot_H

#include <S3/Object.h>

class IRhinoBot : public Enemy {
public:
    float Acceleration = 0;
    bool FlipSide = false;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_RhinoBot_H */
