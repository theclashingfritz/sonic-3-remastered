#ifndef SCENE_MHZ_H
#define SCENE_MHZ_H

#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"
#include "LevelScene.h"
#include "Player.h"

class Scene_MHZ : public LevelScene {
public:
    Scene_MHZ(Application* app, int act, int checkpoint);
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

#endif /* SCENE_MHZ_H */

