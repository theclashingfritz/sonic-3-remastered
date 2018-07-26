#ifndef SCENE_SPECIALSTAGE_H
#define SCENE_SPECIALSTAGE_H

#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"

class Scene_SpecialStage : public Scene {
public:
    Texture* tex_BG;
    Texture* tex_Floor;

    Texture* tex_Arrow;
    Texture* tex_Ball;
    Texture* tex_ChaosEmerald;
    Texture* tex_HUDPieceCenter;
    Texture* tex_HUDPieceEnds;
    Texture* tex_HUDPieceNumbers;
    Texture* tex_Ring;
    Texture* tex_Sonic;
    Texture* tex_HUD_Buttons;

    Texture* paletteDry;
    Texture* paletteWet;

    int BallsLeft = 123;
    int RingsLeft = 45;

    int frame = 0;
    int frameOff = 0;
    int frameNon = 0;
    int frameSonic = 0;
    int frameTurn = 0;
    int sceneOut = -2;
    int turn = 0;

    int nextMove = 0;

    Scene_SpecialStage(Application* app, int data1, int data2);
    void Update();
    void Render();
    void Free();
};

#endif
