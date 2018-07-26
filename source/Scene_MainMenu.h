#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"

class Menu {
public:
    Application* app;
    Scene_MainMenu* sc;
    int     SelectedItem = 0;
    float   ScrollY = 0.0f;
    float   ToScrollY = 0.0f;
    bool    ScrollSnap = true;
    int     BeginScrollTouchY = -1;
    float   BeginScrollY = -1.0f;
    Menu*   Parent = NULL;
    Menu*   SubMenu[16] = { NULL };
    int     SubMenuCount = 0;
    bool    Custom = false;
    bool    GameStart = false;
    const char* text[16];
    Menu(Application* app = NULL, Scene_MainMenu* sce = NULL) { SubMenu[0] = NULL; this->app = app; sc = sce; }
    virtual bool ButtonSelected();
    virtual void Render(int x, float Cos, float Sin, Texture* tex_Buttons, Texture* tex_Font);
};

class Menu_Main : public Menu {
public:
    Menu_Main(Application* app = NULL, Scene_MainMenu* sce = NULL) { SubMenu[0] = NULL; this->app = app; sc = sce; }
    bool ButtonSelected();
};

class Menu_StartGame : public Menu {
public:
    Menu_StartGame(Application* app = NULL, Scene_MainMenu* sce = NULL) { GameStart = true; SubMenu[0] = NULL; this->app = app; sc = sce; }
    bool ButtonSelected();
    void Render(int x, float Cos, float Sin, Texture* tex_Buttons, Texture* tex_Font);
    void drawFont(int x, int y, const char* text, float align, Texture* font);
};

class Scene_MainMenu : public Scene {
public:
    Texture* tex_BG;
    Texture* tex_Side;
    Texture* tex_Buttons;
    Texture* tex_Font;
    Texture* tex_HUD_ZoneNameFont;
    Texture* tex_ZoneNameFont;
    Texture* tex_Menu_LevelPreviews;
    Texture* tex_Menu_Back;

    Audio* aud_BG;
    Audio* aud_ShortBeep;

    int frame = 0;
    int sceneIn = 80;
    int sceneOut = -2;
    int transitionFramesMax = 45;

    float Sin;
    float Cos;

    Menu* nextMenu = NULL;
    int blinkyItemAnim = -1;
    int transitionFrames = -1;
    int transitionDirection = -1;

    Menu_Main* menu_Main;
        Menu_StartGame* menu_StartGame;
        Menu* menu_Multiplayer;
            Menu* menu_Local;
                //Menu* menu_HostGame;
                //Menu* menu_JoinGame;
            Menu* menu_Online;
                //Menu* menu_HostGame;
                //Menu* menu_JoinGame;
        Menu* menu_Options;
            Menu* menu_MusicSFX;
            Menu* menu_Display;
                //Menu* menu_CRTEffect;
                //Menu* menu_NoHUD;
            Menu* menu_Controls;
        Menu* menu_Extras;
            Menu* menu_ChallengeMode;

    Menu* currentMenu = NULL;

    Scene_MainMenu(Application* app, int data1, int data2);
    void Load();
    void Update();
    void Render();
    void Free();
};

#endif
