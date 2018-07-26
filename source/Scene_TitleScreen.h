#ifndef SCENE_TITLESCREEN_H
#define SCENE_TITLESCREEN_H

#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"
#include "Engine/ITexture.h"

class Scene_TitleScreen : public Scene {
public:
    Texture* tex_BG;
    Texture* tex_Logo;
    Texture* tex_SonicBase;
    Texture* tex_SonicEye;
    Texture* tex_SonicFinger;
    Texture* tex_Tails;
    Texture* tex_TouchToStart;

    ITexture* tex_test;

    ISprite*  spr_signs;
    ISprite*  spr_menu_BG;

    int frame = 0;
    int frameNon = 0;
    int tailsX = 0;
    int sceneOut = -2;

    IVideo* IntroMovie;

    Scene_TitleScreen(Application* app, int data1, int data2);
    void Update();
    void Render();
    void Free();
};

#endif
