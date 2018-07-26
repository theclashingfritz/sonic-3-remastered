#ifndef SCENE_SOZ_H
#define SCENE_SOZ_H

#include "Standard.h"
#include "Application.h"
#include "LevelScene.h"
#include <S3/Player.h>

class Scene_SOZ : public LevelScene {
public:
    Scene_SOZ(Application* app, int act, int checkpoint);
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

#endif /* SCENE_SOZ_H */

