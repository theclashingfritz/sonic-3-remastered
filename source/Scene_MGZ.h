#ifndef SCENE_MGZ_H
#define SCENE_MGZ_H

#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"
#include "LevelScene.h"
#include "Player.h"

class Scene_MGZ : public LevelScene {
public:
    Scene_MGZ(Application* app, int act, int checkpoint);
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

#endif /* SCENE_MGZ_H */

