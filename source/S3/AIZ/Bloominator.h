#ifndef S3_Bloominator_H
#define S3_Bloominator_H

#include <S3/Object.h>

class IBloominatorSpore : public Enemy {
public:
    void Update();
    void Render(int CamX, int CamY);
};

class IBloominator : public Enemy {
public:
    IBloominatorSpore* Spore1 = NULL;
    IBloominatorSpore* Spore2 = NULL;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_Bloominator_H */
