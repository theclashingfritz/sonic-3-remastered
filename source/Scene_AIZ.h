#ifndef SCENE_AIZ_H
#define SCENE_AIZ_H

#include "Standard.h"
#include "Resources.h"
#include "Application.h"
#include "TextureAudio.h"
#include "LevelScene.h"
#include "Player.h"

class Scene_AIZ : public LevelScene {
public:

    void DrawAboveEverythingNonHUD();
    void InitZone(bool resetTextures, int check, int specialRing, bool actTransition);
    void CreateUniqueTextures(Application* app, int act);
    void UnloadUniqueTextures(Application* app, int act);
    void HandleCamera();
    void DoSwitchActZone();
    void RefreshRW();

    void Update();
    void Render();
    void Free();
    int  CustomAnimTile(int ind, int i, int parRow);
    Scene_AIZ(Application* app, int act, int checkpoint);
};


#endif /* Scene_AIZ_H */
