#include "Standard.h"
#include "Scene_TitleScreen.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_MainMenu.h"

#include "Engine/IO/IResources.h"
#include "Engine/ITexture.h"
#include "Engine/IGraphics.h"
#include "Engine/IE.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

ISprite* texLogo = NULL;
ISprite* texTails = NULL;
ISprite* texSonicEye = NULL;
ISprite* texSonicBase = NULL;
ISprite* texBackground = NULL;
ISprite* texSonicFinger = NULL;
ISprite* texTouchToStart = NULL;

Scene_TitleScreen::Scene_TitleScreen(Application* app, int data1, int data2) {
    App = app;
    G = app->g;

    IntroMovie = new IVideo();
    IntroMovie->Load();

    //
    #ifdef PLATFORM_WINDOWS
    aud_BGM_TitleScreen = newMusic(snd_BGM_TitleScreen_ogg, "snd/BGM_TitleScreen.ogg");
    #else
    aud_BGM_TitleScreen = newMusic(snd_BGM_TitleScreen_ogg, "Resource/Movies/IntroTee.ogg");
    #endif

    aud_C_ShortBeep = newSound(snd_C_ShortBeep_ogg, "snd/C_ShortBeep.ogg");

    App->Stack->Push("Title Screen", aud_BGM_TitleScreen, 20, 0, 0, false, true);

    T("Common") = new ITexture(App, IResources::Load("Sprites/Common/Texture.png"), false);

    texBackground = new ISprite(IResources::Load("Sprites/Common/Title Menu/Background.spr"), T("Common"));
    texLogo = new ISprite(IResources::Load("Sprites/Common/Title Menu/Logo.spr"), T("Common"));
    texSonicBase = new ISprite(IResources::Load("Sprites/Common/Title Menu/Sonic (Base).spr"), T("Common"));
    texSonicEye = new ISprite(IResources::Load("Sprites/Common/Title Menu/Sonic (Eye).spr"), T("Common"));
    texSonicFinger = new ISprite(IResources::Load("Sprites/Common/Title Menu/Sonic (Finger).spr"), T("Common"));
    texTails = new ISprite(IResources::Load("Sprites/Common/Title Menu/Tails.spr"), T("Common"));
    texTouchToStart = new ISprite(IResources::Load("Sprites/Common/Title Menu/Touch To Start.spr"), T("Common"));
}
void Scene_TitleScreen::Update() {
    if (App->realPaused) return;

    IntroMovie->NextFrame((int)std::floor(IntroMovie->CurrentFrame60 / 60.f * 24.27f));
    IntroMovie->CurrentFrame60++;

    frame++;
    frame = (frame) % 480;
    frameNon++;

    if (frameNon > 180) {
        frameNon = 180;
    }

    tailsX = (tailsX + 1) % (int)(App->gameWidth * 2);

    if (sceneOut > -2) {
        sceneOut--;
        aud_BGM_TitleScreen->SetVolume(sceneOut / 40.0f);
    }
    if (sceneOut == -2) {
        if (App->input->jumpPressed[0] ||
            App->input->jump2Pressed[0] ||
            //App->input->jump3Pressed[0] ||
            //App->input->pausePressed[0] ||
            App->input->mousePressed) {
            sceneOut = 40;
            aud_C_ShortBeep->Play(0);
            App->input->Vibrate(0, 0.75, 250);
        }
    }
    if (sceneOut == -1) {
        //aud_BGM_TitleScreen->Stop();
        App->nextScene = new Scene_MainMenu(App, -1, -1);
        App->Stack->Remove("Title Screen");
    }
}
void Scene_TitleScreen::Render() {
    if (sceneOut > -2) {
        G->SetFade((40.f - sceneOut) / 40.f);
    }

    G->PaletteShift(-1);

    if (!IntroMovie->Finished)
        G->DrawVideo(IntroMovie);
    else {
        // Draws the background normally.
        G->DrawSprite(texBackground, App->renderWidth / 2, 0, Z_MID, 0, IE::CenterAlign | IE::TopAlign);
        // Draws the bottom 24 pixels to make the wave pattern in the ocean.
        for (int i = 0; i < 24; i += 1)
            G->DrawSprite(texBackground, 0, App->gameHeight - 24 + i, 400, 1, App->renderWidth / 2 + (float)std::sin( ((int)(frame / 2.0f + i * 10.0f) % 60) / 60.0f * M_PI * 2.0f) * (i / 4.0f), 225 - 24 + i, Z_FRONT, 0.0f, 0, IE::CenterAlign | IE::TopAlign);

        // Draws Tails.
        G->DrawSprite(texTails, tailsX - 50.0f - (App->gameWidth - App->renderWidth) / 2, 224 / 2.0f - 48.0f, Z_FRONT, (frame % 4) / 2, IE::CenterAlign | IE::MiddleAlign);

        // Draws the Sonic In Ring sprite.
        G->DrawSprite(texSonicBase, App->renderWidth / 2.0f, 112 - 38, Z_FRONT * 2, 0, IE::CenterAlign | IE::MiddleAlign);

        // Sonic Finger Animation
        if (frame < 5)
            G->DrawSprite(texSonicFinger, App->renderWidth / 2 + 36, App->gameHeight / 2 - 38, Z_FRONT * 2.1f, 1, IE::CenterAlign | IE::MiddleAlign);
        else if (frame >= 10 && frame < 15)
            G->DrawSprite(texSonicFinger, App->renderWidth / 2 + 36, App->gameHeight / 2 - 38, Z_FRONT * 2.1f, 0, IE::CenterAlign | IE::MiddleAlign);

        // Sonic Eye
        if (frame >= 60) {
            if (frame - 60 < 0x02)
                G->DrawSprite(texSonicEye, App->renderWidth / 2 - 44, App->gameHeight / 2 - 56, Z_FRONT * 2.1f, 1, IE::CenterAlign | IE::MiddleAlign);
            else if (frame - 60 < 0x0A)
                G->DrawSprite(texSonicEye, App->renderWidth / 2 - 44, App->gameHeight / 2 - 56, Z_FRONT * 2.1f, 0, IE::CenterAlign | IE::MiddleAlign);
            else if (frame - 60 < 0x12)
                G->DrawSprite(texSonicEye, App->renderWidth / 2 - 44, App->gameHeight / 2 - 56, Z_FRONT * 2.1f, 1, IE::CenterAlign | IE::MiddleAlign);
        }

        // Draws the Touch To Start / Press Any Button To Start sprite
        if (frame % 60 < 30) {
            if (App->Mobile)
                G->DrawSprite(texTouchToStart, App->renderWidth / 2, App->gameHeight / 2 + 96, Z_FRONT * 2.6f, 0, IE::CenterAlign | IE::MiddleAlign);
            else
                G->DrawSprite(texTouchToStart, App->renderWidth / 2, App->gameHeight / 2 + 76, Z_FRONT * 2.6f, 1, IE::CenterAlign | IE::MiddleAlign);
        }

        // Draws the SEGA copyright in the bottom right.
        G->DrawSprite(texTouchToStart, App->renderWidth - 6, App->gameHeight - 9, Z_FRONT * 2.6f, 2, IE::RightAlign | IE::BottomAlign);

        // Equation for the fancy elastic ease.
        float offsetY = -(float)(std::sin((frameNon / 60.0f - 0.35f) * 2 * M_PI) / std::exp(3 * (frameNon / 60.0f - 0.35f)));
        if (frameNon > 160) {
            offsetY = 0;
        }
        // Sonic 3 Logo using that fancy elastic ease.
        G->DrawSprite(texLogo, App->renderWidth / 2, App->renderHeight / 2 + 25 + offsetY * 128, Z_FRONT * 2.2f, 0, IE::CenterAlign | IE::MiddleAlign);
    }
}
void Scene_TitleScreen::Free() {
    FreeAudio();
}
