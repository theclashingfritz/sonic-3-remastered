#ifndef S3_PATHSWITCHER_H
#define S3_PATHSWITCHER_H

#include <S3/Object.h>

class IPathSwitcher : public Object {
public:
    int Size = 1;

    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_PATHSWITCHER_H */
