#ifndef S3_TreeBark_H
#define S3_TreeBark_H

#include <S3/Object.h>

class ITreeBark : public Object {
public:
    void Create();
    void Update();
    void Render(int CamX, int CamY);
};

#endif /* S3_TreeBark_H */
