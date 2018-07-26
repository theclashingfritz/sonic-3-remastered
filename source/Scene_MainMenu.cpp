#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Scene_AIZ.h"
#include "Scene_HCZ.h"
#include "Scene_MGZ.h"
#include "Scene_CNZ.h"
#include "Scene_ICZ.h"
#include "Scene_LBZ.h"
#include "Scene_MHZ.h"
#include "Scene_FBZ.h"
#include "Scene_SOZ.h"
#include "Scene_LRZ.h"
#include "Scene_SSZ.h"
#include "Scene_DEZ.h"
#include "Scene_TDZ.h"
#include "Scene_SpecialStage.h"
#include "Application.h"
#include "Resources.h"

#include "S3/HCZ/Level.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) app->Textures[name]
#define S(name) app->Sprites[name]

bool Menu::ButtonSelected() {
    if (SelectedItem == -1) return false;

    if (SubMenu[SelectedItem] != NULL) {
        sc->nextMenu = SubMenu[SelectedItem];
        sc->aud_C_ShortBeep->Play(0);
        app->input->Vibrate(0, 0.35, 200); // weaker menu change
        sc->blinkyItemAnim = 8 * 1;
        sc->transitionDirection = 1;
    }
    else {
        // "wrong" sfx
        app->input->Vibrate(0, 0.75, 400); // get aggressive
    }
    return false;
}
void Menu::Render(int x, float Cos, float Sin, Texture* tex_Buttons, Texture* tex_Font) {
    for (int i = 0; i < SubMenuCount; i++) {
        if (text[i] == NULL) break;

        app->drawSpriteH(tex_Buttons, x + 80 + 2 + (int)(Cos * (64 * i + ScrollY)), 50 + 2 + (int)(Sin * (64 * i + ScrollY)), 0, i % 4 + 1, 0, 1);
        app->drawSpriteH(tex_Buttons, x + 80 + (int)(Cos * (64 * i + ScrollY)), 50 + (int)(Sin * (64 * i + ScrollY)), 0, 0, 0, 1);

        sc->drawText(tex_Font, text[i], x + 80 + 18 + (int)(Cos * (64 * i + ScrollY)), 50 + (int)(Sin * (64 * i + ScrollY)), 0, 1, (i == SelectedItem ? 1 : 0));
    }
}
bool Menu_Main::ButtonSelected() {
    if (SelectedItem < 3)
        Menu::ButtonSelected();
    else if (SelectedItem == 3)
        app->SwapFullscreen();
    else
        app->running = false;
    return false;
}
bool Menu_StartGame::ButtonSelected() {
    app->Stack->Clear();
    Menu::ButtonSelected();

    return false;
}
void Menu_StartGame::Render(int x, float Cos, float Sin, Texture* tex_Buttons, Texture* tex_Font) {
    int initalY = 48;

    drawFont(x + app->gameWidth / 2, initalY / 2 + (int)(ScrollY), "LEVEL SELECT", 0.5f, sc->tex_ZoneNameFont);

    for (int i = 0; i < SubMenuCount; i++) {
        if (text[i] == NULL) break;


        if (i == SelectedItem && (sc->frame / 15) % 2 != 0) {
            app->drawRectangle(x + 40 - 2, initalY - 2 + (int)(64 * i + ScrollY), app->gameWidth - 40 * 2, 60, 1.0f, 0.0f, 0.0f, 1.0f);
        }
        else {
            app->drawRectangle(x + 40 - 2, initalY - 2 + (int)(64 * i + ScrollY), app->gameWidth - 40 * 2, 60, 0.145098f, 0.145098f, 0.145098f, 1.0f);
        }
        app->drawRectangle(x + 40, initalY + (int)(64 * i + ScrollY), app->gameWidth - 40 * 2 - 4, 56, 0.145098f, 0.145098f, 0.145098f, 1.0f);

        if (i != 1 && i != 3 && i != 0)
            app->ChangeShader(app->shader_blackwhite);
        app->drawSpriteH(sc->tex_Menu_LevelPreviews, x + 40, initalY + 56 / 2 + (int)(64 * i + ScrollY), (i + 4 + (i >= 10 ? 1 : 0)) % 5, (i + 4 + (i >= 10 ? 1 : 0)) / 5, 0, 1);
        if (i != 1 && i != 3 && i != 0)
            app->ChangeShader(app->shader_basic);

        drawFont(x + 40 + 80 + 16, initalY + 56 / 2 + (int)(64 * i + ScrollY) - 13, text[i], 0, sc->tex_HUD_ZoneNameFont);
        drawFont(x + 40 + 80 + 16 + 4, initalY + 56 / 2 + (int)(64 * i + ScrollY) + 13, "ZONE", 0, sc->tex_HUD_ZoneNameFont);
    }
}
void Menu_StartGame::drawFont(int x, int y, const char* text, float align, Texture* font) {
    int ww = 0;
    int addx = 0;
    // Calculate width of TEXT1
    for (unsigned char p = 0; p < strlen(text); p++) {
        int frame = text[p] - 65;
        if (text[p] == ' ') {
            frame = 26;
        }
        else if (frame < 0 || frame > 26) {
            frame = 26;
        }
        ww += getZoneNameLetterWidth(frame) + 2;
    }
    ww -= 8;
    for (unsigned char p = 0; p < strlen(text); p++) {
        int frame = text[p] - 65;
        if (text[p] != ' ') {
            if (frame < 0 || frame > 26) {
                frame = 26;
            }
            //app->drawSprite(font, x - ww * align + addx, y, 5, frame, 1, 1);
            //G->DrawSprite(S("Font"), x - ww * align + addx, y, Z_FRONT * 5, frame, IE::CenterAlign | IE::MiddleAlign);
            addx += getZoneNameLetterWidth(frame) + 2;
        }
        else {
            addx += getZoneNameLetterWidth(26) + 2;
        }
    }
}

int view_port = 0;
int view_port_fin = 0;
int current_menu_item = 0;
int tt_width = 80;

/*
const char* menuIt[5] = {
    "SOLO",
    "GROUP",
    "EXTRAS",
    "OPTIONS",
    "EXIT",
};
*/

int menuItems = 14;
const char* menuIt[14] = {
    "AIZ",
    "HCZ",
    "MGZ",
    "CNZ",
    "ICZ",
    "LBZ",

    "MHZ",
    "FBZ",
    "SOZ",
    "LRZ",
    "HPZ",
    "SSZ",
    "DEZ",
    "TDZ"
};

Scene_MainMenu::Scene_MainMenu(Application* app, int data1, int data2) {
    this->app = app;
    G = app->g;

    Sin = std::sin(toRadians(75));
    Cos = std::cos(toRadians(75));

    /*a[0] = "START GAME";
    a[1] = "MUTLIPLAYER";
    a[2] = "OPTIONS";
    a[3] = "EXTRAS";

    a[4] = "LOCAL";
    a[5] = "BLUETOOTH";
    a[6] = "ONLINE";

    a[7] = "MUSIC AND SOUND";
    a[8] = "CONTROLS";
    a[9] = "NEW FEATURES";
    a[10] = "YOUTUBER MODE";
    a[11] = "CREDITS";*/

    menu_Main = new Menu_Main(app, this);
    menu_StartGame = new Menu_StartGame(app, this);
    menu_Multiplayer = new Menu(app, this);
    menu_Local = new Menu(app, this);
    menu_Online = new Menu(app, this);
    menu_Options = new Menu(app, this);
    menu_MusicSFX = new Menu(app, this);
    menu_Display = new Menu(app, this);
    menu_Controls = new Menu(app, this);
    menu_Extras = new Menu(app, this);
    menu_ChallengeMode = new Menu(app, this);

    menu_Main->text[0] = "START GAME";
    menu_Main->text[1] = "MUTLIPLAYER";
    menu_Main->text[2] = "OPTIONS";
    menu_Main->text[3] = "EXTRAS";
    menu_Main->text[4] = "EXIT";
    menu_Main->SubMenu[0] = menu_StartGame;
    menu_Main->SubMenu[1] = menu_Multiplayer;
    menu_Main->SubMenu[2] = menu_Options;
    menu_Main->SubMenu[3] = menu_Extras;
    //menu_Main->SubMenu[4] = menu_Extras;
    menu_Main->SubMenuCount = 5;
    if (app->platform == Application::P_iOS) {
        menu_Main->SubMenuCount = 4;
    }

    menu_StartGame->Parent = menu_Main;
    menu_StartGame->Custom = true;
    menu_StartGame->text[0] = "ANGEL ISLAND";
    menu_StartGame->text[1] = "HYDRO CITY";
    menu_StartGame->text[2] = "MARBLE GARDEN";
    menu_StartGame->text[3] = "CARNIVAL NIGHT";
    menu_StartGame->text[4] = "ICE CAP";
    menu_StartGame->text[5] = "LAUNCH BASE";
    menu_StartGame->text[6] = "MUSHROOM HILL";
    menu_StartGame->text[7] = "FLYING BATTERY";
    menu_StartGame->text[8] = "SANDOPOLIS";
    menu_StartGame->text[9] = "LAVA REEF";
    menu_StartGame->text[10] = "SKY SANCTUARY";
    menu_StartGame->text[11] = "DEATH EGG";
    menu_StartGame->text[12] = "DOOMSDAY";
    menu_StartGame->SubMenuCount = 13;

    menu_Multiplayer->Parent = menu_Main;
    menu_Multiplayer->text[0] = "LOCAL";
    menu_Multiplayer->text[1] = "ONLINE";
    menu_Multiplayer->SubMenu[0] = menu_Local;
    menu_Multiplayer->SubMenu[1] = menu_Online;
    menu_Multiplayer->SubMenuCount = 2;

        menu_Local->Parent = menu_Multiplayer;
        menu_Local->Custom = true;
        menu_Local->text[0] = "HOST LOCAL ROOM";
        menu_Local->text[1] = "HOST WIRELESS ROOM";
        menu_Local->text[2] = "JOIN WIRELESS ROOM";
        menu_Local->SubMenuCount = 3;

        menu_Online->Parent = menu_Multiplayer;
        menu_Online->text[0] = "HOST GAME";
        menu_Online->text[1] = "JOIN WITH CODE";
        menu_Online->text[2] = "JOIN FROM FAVORITES";
        menu_Online->SubMenuCount = 3;

    menu_Options->Parent = menu_Main;
    menu_Options->text[0] = "MUSIC AND SOUND";
    menu_Options->text[1] = "DISPLAY SETTINGS";
    menu_Options->text[2] = "CONTROLS";
    menu_Options->SubMenuCount = 3;

    menu_Extras->Parent = menu_Main;
    menu_Extras->text[0] = "CHALLENGE MODE";
    menu_Extras->text[1] = "BLUE KNUCKLES";
    menu_Extras->SubMenuCount = 2;

    currentMenu = menu_Main;


    /*menu_Main.MenuParent = this;
    menu_StartGame.MenuParent = this;
    menu_Multiplayer.MenuParent = this;
    menu_Local.MenuParent = this;
    menu_Online.MenuParent = this;
    menu_Options.MenuParent = this;
    menu_MusicSFX.MenuParent = this;
    menu_Display.MenuParent = this;
    menu_Controls.MenuParent = this;
    menu_Extras.MenuParent = this;
    menu_ChallengeMode.MenuParent = this;*/

    /*Menu menu_StartGame;
    Menu menu_Multiplayer;
        Menu menu_Local;
        Menu menu_Online;
    Menu menu_Options;
        Menu menu_MusicSFX;
        Menu menu_Display;
        Menu menu_Controls;
    Menu menu_Extras;
        Menu menu_ChallengeMode;*/
}

void Scene_MainMenu::Load() {
    tex_BG = newTexture(res_Menu_BG_png, 1, 1, "res/Menu_BG.png");
    tex_Side = newTexture(res_Menu_Side_png, 1, 1, "res/Menu_Side.png");
    tex_Font = newTexture(res_Font40x2_png, 40, 2, "res/Font40x2.png");
    tex_Buttons = newTexture(res_Menu_Buttons1x5_png, 1, 5, "res/Menu_Buttons1x5.png");

    tex_HUD_ZoneNameFont = newTexture(res_Common_HUD_ZoneNameFont26_png, 26, 1, "res/Common/HUD_ZoneNameFont26.png");
    tex_ZoneNameFont = newTexture(res_Menu_ZoneNameFont26x2_png, 26, 2, "res/Menu_ZoneNameFont26x2.png");
    tex_Menu_LevelPreviews = newTexture(res_Menu_LevelPreviews5x5_png, 5, 5, "res/Menu_LevelPreviews5x5.png");
    tex_Menu_Back = newTexture(res_Menu_Back2_png, 2, 1, "res/Menu_Back2.png");

    aud_BGM_DataSelect = newMusic(snd_BGM_DataSelect_ogg, "snd/BGM_DataSelect.ogg");
    aud_BGM_DataSelect->Play(-1);

    aud_C_ShortBeep = newSound(snd_C_ShortBeep_ogg, "snd/C_ShortBeep.ogg");

    app->Stack->Push("Menu", aud_BGM_DataSelect, 20, 0, 0, true, false);

    //T("Common") = new ITexture(IResources::Load("Sprites/Common/Texture.png"), false);

    S("Background") = new ISprite(IResources::Load("Sprites/Common/Main Menu/Background.spr"), T("Common"));
    S("Font") = new ISprite(IResources::Load("Sprites/Common/Main Menu/Font (Tall).spr"), T("Common"));
    S("SmallFont") = new ISprite(IResources::Load("Sprites/Common/Main Menu/Font (Small).spr"), T("Common"));
    S("Level Previews") = new ISprite(IResources::Load("Sprites/Common/Main Menu/Level Previews.spr"), T("Common"));
}
void Scene_MainMenu::Update() {
    app->realPaused = false;
    app->paused = false;

    frame++;
    frame = (frame) % (320 * 8);

    int maxOnScreen = 3;

    if (sceneOut == -2) {
        if (app->input->leftPressed[0] ||
            (app->input->touchPressed[0] && app->input->mouseX < app->renderWidth / 2 && app->input->mouseY < app->renderHeight / 2)) {
            aud_C_ShortBeep->Play(0);
            current_menu_item--;
            if (current_menu_item < 0)
                current_menu_item = menuItems - 1;
            if (current_menu_item < view_port)
                view_port = current_menu_item;
            if (current_menu_item > view_port + 2)
                view_port = current_menu_item - 2;
        }
        if (app->input->rightPressed[0] ||
            (app->input->touchPressed[0] && app->input->mouseX > app->renderWidth / 2 && app->input->mouseY < app->renderHeight / 2)) {
            aud_C_ShortBeep->Play(0);
            current_menu_item++;
            if (current_menu_item > menuItems - 1)
                current_menu_item = 0;
            if (current_menu_item < view_port)
                view_port = current_menu_item;
            if (current_menu_item > view_port + 2)
                view_port = current_menu_item - 2;
        }
        if (app->input->jumpPressed[0] ||
            (app->input->touchPressed[0] && app->input->mouseY > app->renderHeight / 2)) {
            if (current_menu_item != -1) {
                app->Stack->Clear();
                //
                sceneOut = 40;
            }
            else {
                aud_C_ShortBeep->Play(3);
            }
        }
    }

    if (sceneOut == 0) {
        G->SetFade(0.f);
        //app->nextScene = new Scene_MSZ(app, 1 + app->input->right[0], 0);
        if (current_menu_item == 0)
            app->nextScene = new Scene_AIZ(app, 1 + app->input->down[0], 2);
        else if (current_menu_item == 1)
            app->nextScene = new Scene_HCZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 2)
            app->nextScene = new Scene_MGZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 3)
            app->nextScene = new Scene_CNZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 4)
            app->nextScene = new Scene_ICZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 5)
            app->nextScene = new Scene_LBZ(app, 1 + app->input->down[0], 1);
        else if (current_menu_item == 6)
            app->nextScene = new Scene_MHZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 7)
            app->nextScene = new Scene_FBZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 8)
            app->nextScene = new Scene_SOZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 9)
            app->nextScene = new Scene_LRZ(app, 1 + app->input->down[0], 0);
        //else if (current_menu_item == 10)
            //app->nextScene = new Scene_HPZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 11)
            app->nextScene = new Scene_SSZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 12)
            app->nextScene = new Scene_DEZ(app, 1 + app->input->down[0], 0);
        else if (current_menu_item == 13)
            app->nextScene = new Scene_TDZ(app, 1 + app->input->down[0], 0);

        else {
            sceneOut = -2;
            sceneIn = 40;
        }
    }

    view_port_fin += (view_port * (tt_width + 8) - view_port_fin) / 4;

    if (sceneIn > -2) {
        sceneIn -= 1;
    }
    if (sceneOut > -2) {
        sceneOut -= 1;
    }

	if (/* DISABLES CODE */ (true))
		return;

    if (currentMenu != NULL) {
        if (currentMenu->ScrollSnap && currentMenu->SelectedItem != -1) {
            if (50 + 2 + (int)(Sin * (64 * currentMenu->SelectedItem + currentMenu->ToScrollY)) < 0 || 50 + 2 + (int)(Sin * (64 * currentMenu->SelectedItem + currentMenu->ToScrollY)) > app->gameHeight - 64) {
                currentMenu->ToScrollY = std::max(-64.0f * currentMenu->SelectedItem, currentMenu->ToScrollY - 64);
            }
        }

        currentMenu->ScrollY += (currentMenu->ToScrollY - currentMenu->ScrollY) / 8.0f;

        if (transitionFrames == -1 && blinkyItemAnim == -1) {
            if (app->input->upPressed[0]) {
                currentMenu->SelectedItem = (currentMenu->SelectedItem - 1 + currentMenu->SubMenuCount) % currentMenu->SubMenuCount;
                aud_C_ShortBeep->Play(0);
                if (!currentMenu->ScrollSnap) {
                    if ((int)currentMenu->ToScrollY % 64 != 0) {
                        currentMenu->ToScrollY = std::max(-64 * currentMenu->SelectedItem, (currentMenu->SubMenuCount - maxOnScreen) * -64);
                    }
                }
                currentMenu->ScrollSnap = true;
            }
            if (app->input->downPressed[0]) {
                currentMenu->SelectedItem = (currentMenu->SelectedItem + 1 + currentMenu->SubMenuCount) % currentMenu->SubMenuCount;
                aud_C_ShortBeep->Play(0);
                if (!currentMenu->ScrollSnap) {
                    if ((int)currentMenu->ToScrollY % 64 != 0) {
                        currentMenu->ToScrollY = std::max(-64 * currentMenu->SelectedItem, (currentMenu->SubMenuCount - maxOnScreen) * -64);
                    }
                }
                currentMenu->ScrollSnap = true;
            }
            if (app->input->jumpPressed[0] || app->input->pausePressed[0] || (app->input->mouseReleased && std::abs(currentMenu->BeginScrollTouchY - app->input->mouseY) < 10)) {
                currentMenu->ButtonSelected();
                currentMenu->ScrollSnap = true;
            }
            if (app->input->jump2Pressed[0]) {
                nextMenu = currentMenu->Parent;
                if (nextMenu != NULL) {
                    aud_C_ShortBeep->Play(0);
                    blinkyItemAnim = 0;
                    transitionDirection = -1;
                }
                currentMenu->ScrollSnap = true;
            }
            if (app->input->mousePressed) {
                currentMenu->ScrollSnap = false;
                currentMenu->BeginScrollTouchY = app->input->mouseY;
                currentMenu->BeginScrollY = currentMenu->ScrollY;

                currentMenu->SelectedItem = -1;
                for (int i = 0; i < currentMenu->SubMenuCount; i++) {
                    int x = 80 + (int)(Cos * (64 * i + currentMenu->ScrollY));
                    int y = 50 + (int)(Sin * (64 * i + currentMenu->ScrollY));

                    if (currentMenu->GameStart) {
                        x = 40;
                        y = 48 + (int)(64 * i + currentMenu->ScrollY);
                        if (app->input->mouseX > x && app->input->mouseY >= y && app->input->mouseY <= y + (60) && app->input->mouseX < x + app->renderWidth - 80) {
                            currentMenu->SelectedItem = i;
                        }
                    }
                    else {
                        if (app->input->mouseX > x && app->input->mouseY >= y - (Sin * 20) && app->input->mouseY <= y + (Sin * 20) && app->input->mouseX < x + 192) {
                            currentMenu->SelectedItem = i;
                        }
                    }
                }
            }
            if (app->input->mouse) {
                currentMenu->ScrollSnap = false;
                currentMenu->ToScrollY = currentMenu->BeginScrollY + app->input->mouseY - currentMenu->BeginScrollTouchY;
                if (std::abs(currentMenu->BeginScrollTouchY - app->input->mouseY) > 10) {
                    currentMenu->SelectedItem = -1;
                }
            }
            if (!app->input->mouse && !currentMenu->ScrollSnap) {
                if (currentMenu->ToScrollY < (currentMenu->SubMenuCount - maxOnScreen) * -64.0f) {
                    currentMenu->ToScrollY += ((currentMenu->SubMenuCount - maxOnScreen) * -64.0f - currentMenu->ToScrollY) / 2.0f;
                }
                if (currentMenu->ToScrollY > (0) * -64.0f) {
                    currentMenu->ToScrollY += ((0) * -64.0f - currentMenu->ToScrollY) / 2.0f;
                }
            }
        }

        if (blinkyItemAnim > 0) {
            blinkyItemAnim--;
        }
        if (blinkyItemAnim == 0) {
            transitionFrames = transitionFramesMax;
            blinkyItemAnim = -1;
        }
        if (transitionFrames > 0) {
            transitionFrames--;
        }
        if (transitionFrames == 0) {
            currentMenu = nextMenu;
            transitionFrames = -1;
        }
    }

}
void Scene_MainMenu::Render() {
    if (sceneIn > -2) {
        G->SetFade(sceneIn / 40.f);
    }
    if (sceneOut > -2) {
        G->SetFade(1.f - sceneOut / 40.f);
    }

    app->drawZ = -0.0f;
    int x = 0;
    if (transitionFrames > 0) {
        x = (int)(-app->gameWidth * (1.0f - std::pow(transitionFrames / (float)transitionFramesMax, 3.0f))) * transitionDirection;
    }

    G->DrawSprite(S("Background"), app->renderWidth / 2, 1, Z_BACK, 0, IE::CenterAlign | IE::TopAlign);
    G->DrawSprite(S("Background"), app->renderWidth / 2, 0, Z_BACK, 0, IE::CenterAlign | IE::TopAlign);

    DrawLongFont(S("Font"), "MAIN MENU", app->renderWidth / 2, app->renderHeight - 20, 1);


    int top = 8;
    int ItemS[2] = { tt_width, app->renderHeight - 20 * 2 - top };
    for (int i = 0; i < menuItems; i++) {
        int left = app->renderWidth / 2 - (ItemS[0] * 1.5 + 8)     + (ItemS[0] + 8) * i - view_port_fin;

        G->DrawRectangle(left + 2, top + 2, Z_MID, 2, ItemS[1], IColor(0.0f, 0.0f, 0.0f, 0.5f));
        G->DrawRectangle(left + 2 + ItemS[0] - 2, top + 2, Z_MID, 2, ItemS[1], IColor(0.0f, 0.0f, 0.0f, 0.5f));
        G->DrawRectangle(left + 2, top + 2, Z_MID, ItemS[0], 2, IColor(0.0f, 0.0f, 0.0f, 0.5f));
        G->DrawRectangle(left + 2, top + 2 + ItemS[1] - 2, Z_MID, ItemS[0], 2, IColor(0.0f, 0.0f, 0.0f, 0.5f));

        float o = 0.0f;
        if (i == current_menu_item && frame % 40 < 20)
            o = 1.0f;
        G->DrawRectangle(left, top, Z_FRONT, 2, ItemS[1], IColor(o, o * .9f, 0.0f, 1.0f));
        G->DrawRectangle(left + ItemS[0] - 2, top, Z_FRONT, 2, ItemS[1], IColor(o, o * .9f, 0.0f, 1.0f));
        G->DrawRectangle(left, top, Z_FRONT, ItemS[0], 2, IColor(o, o * .9f, 0.0f, 1.0f));
        G->DrawRectangle(left, top + ItemS[1] - 2, Z_FRONT, ItemS[0], 2, IColor(o, o * .9f, 0.0f, 1.0f));

        //G->DrawRectangle(left + 2, top + 2 + ItemS[1] - 20 - 4, Z_MID, ItemS[0], 20, IColor(0.0f, 0.0f, 0.0f, 0.5f));
        G->DrawRectangle(left, top + ItemS[1] - 20 - 8, Z_FRONT, ItemS[0], 20, IColor(0.0f, 0.0f, 0.0f, 1.0f));

        G->DrawSprite(S("Level Previews"), left, top + 8, Z_FRONT * 0.5f, 4 + i, IE::LeftAlign | IE::TopAlign);

        DrawSmallFont(S("SmallFont"), menuIt[i], left + ItemS[0] / 2, top + ItemS[1] - 20 - 8 + 10, 1);
    }


    if (currentMenu != NULL) {
        currentMenu->Render(x, Cos, Sin, tex_Buttons, tex_Font);
    }
    if (nextMenu != NULL) {
        nextMenu->Render(x + app->gameWidth * transitionDirection, Cos, Sin, tex_Buttons, tex_Font);
    }

    if (nextMenu != NULL) {
        if (nextMenu->Parent != NULL) {
            int x = 4;
            int y = app->gameHeight - 4;
            app->drawSpriteR(tex_Menu_Back, x, y, 0, 0, 0, 2, 1.0f, 1.0f, 0);
            drawText(tex_Font, "BACK", x + 32, y - 3, 0, 2, 1);
        }
    }

    //G->DrawRectangle(app->input->mouseX, app->input->mouseY, Z_FRONT * 5, 2, 2, IColor(1.0f, 0.0f, 0.0f, 1.0f));
}
void Scene_MainMenu::Free() {
    tex_BG->Free();
    tex_BG = NULL;

    tex_Buttons->Free();
    tex_Buttons = NULL;

    FreeAudio();
}
