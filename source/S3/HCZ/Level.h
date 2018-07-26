#ifndef S3_HCZ_LEVEL_H
#define S3_HCZ_LEVEL_H

#include "../../Standard.h"
#include "../../Application.h"
#include "../../TextureAudio.h"

#include "../../Engine/ITexture.h"

#include "../../S3/Player.h"

class Scene_S3HCZ : public Scene {
public:
    Scene_S3HCZ(Application*, int, int);
    void Update();
    void Render();
    void Free();
};

#endif /* S3_HCZ_LEVEL_H */
