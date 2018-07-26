#ifndef SCENE_SSZ_H
#define SCENE_SSZ_H

#include "Standard.h"
#include "Application.h"
#include "LevelScene.h"
#include "TextureAudio.h"
#include <S3/Player.h>

class Scene_SSZ : public LevelScene {
public:
    Scene_SSZ(Application* app, int act, int checkpoint);
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

#endif /* SCENE_SSZ_H */

