#ifndef SCENE_CNZ_H
#define SCENE_CNZ_H

#include "Standard.h"
#include "Resources.h"
#include "Application.h"
#include "LevelScene.h"
#include <S3/Player.h>

class Scene_CNZ : public LevelScene {
public:
    void DoSwitchActZone();

    void InitZone(bool resetTextures, int check, int specialRing, bool actTransition);
    void CreateUniqueTextures(Application* app, int act);
    void UnloadUniqueTextures(Application* app, int act);


    void Update();
    void Render();
    void Free();
    int  CustomAnimTile(int ind, int i, int parRow);
    Scene_CNZ(Application* app, int act, int checkpoint);
};


#endif /* Scene_CNZ_H */
