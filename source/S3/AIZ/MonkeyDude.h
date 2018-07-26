#ifndef S3_MonkeyDude_H
#define S3_MonkeyDude_H

#include <S3/Object.h>

class IMonkeyDudeSpore : public Enemy {
public:
    void Update();
    void Render(int CamX, int CamY);
};

class IMonkeyDude : public Enemy {
public:
    IMonkeyDudeSpore* Spore1 = NULL;

    int frame_face = 0;
    int frame_arm = 0;
    int frame_hand = 0;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_MonkeyDude_H */
