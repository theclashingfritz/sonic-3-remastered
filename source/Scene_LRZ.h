#ifndef SCENE_LRZ_H
#define SCENE_LRZ_H

#include "Standard.h"
#include "Application.h"
#include "LevelScene.h"
#include <S3/Player.h>

class Scene_LRZ : public LevelScene {
public:
    Scene_LRZ(Application* app, int act, int checkpoint);
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

#endif /* SCENE_LRZ_H */

