#ifndef SCENE_DEZ_H
#define SCENE_DEZ_H

#include "Standard.h"
#include "Application.h"
#include "LevelScene.h"
#include "TextureAudio.h"
#include <S3/Player.h>

class Scene_DEZ : public LevelScene {
public:
    Scene_DEZ(Application* app, int act, int checkpoint);
    void DoSwitchActZone();

    void InitZone(bool resetTextures, int check, int specialRing, bool actTransition);
    void CreateUniqueTextures(Application* app, int act);
    void UnloadUniqueTextures(Application* app, int act);

    void Update();

    int CustomAnimTile(int ind, int i, int parRow);
    
    void HandleCamera();
    void DrawAboveEverythingNonHUD();
    void Render();
    void Free();
};

#endif /* SCENE_DEZ_H */

