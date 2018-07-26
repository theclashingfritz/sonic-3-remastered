#include "Standard.h"
#include "Scene_SpecialStage.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_MainMenu.h"

int x = 0;
int y = 0;
int dir = 0;

// 32 balls wide
int lvlWidth = 8;

Scene_SpecialStage::Scene_SpecialStage(Application* app, int data1, int data2) {
    this->app = app;

    tex_BG = newTexture(res_BlueBalls_BG_png, 1, 1, "res/BlueBalls/BG.png");
    tex_Floor = newTexture(res_BlueBalls_SpecialStage4x4_png, 4, 4, "res/BlueBalls/SpecialStage4x4.png");

    tex_Arrow = newTexture(res_BlueBalls_Arrow_png, 1, 1, "res/BlueBalls/Arrow.png");
    tex_Ball = newTexture(res_BlueBalls_Ball8x3_png, 8, 3, "res/BlueBalls/Ball8x3.png");
    tex_ChaosEmerald = newTexture(res_BlueBalls_ChaosEmerald8x2_png, 8, 2, "res/BlueBalls/ChaosEmerald8x2.png");
    tex_HUDPieceCenter = newTexture(res_BlueBalls_HUDPieceCenter_png, 1, 1, "res/BlueBalls/HUDPieceCenter.png");
    tex_HUDPieceEnds = newTexture(res_BlueBalls_HUDPieceEnds8_png, 8, 1, "res/BlueBalls/HUDPieceEnds8.png");
    tex_HUDPieceNumbers = newTexture(res_BlueBalls_HUDPieceNumbers10_png, 10, 1, "res/BlueBalls/HUDPieceNumbers10.png");

    tex_Ring = newTexture(res_BlueBalls_Ring8x7_png, 8, 7, "res/BlueBalls/Ring8x7.png");
    tex_Sonic = newTexture(res_BlueBalls_Sonic8_png, 8, 1, "res/BlueBalls/Sonic8.png");

    tex_HUD_Buttons = newTexture(res_HUD_Buttons6_png, 6, 1, "res/HUD_Buttons6.png");

    paletteDry = newPalTexture(res_AIZ_PAL1_DRY_png, 1, 1, "res/AIZ/PAL1_DRY.png");
    paletteWet = newPalTexture(res_BlueBalls_PAL_MAIN_png, 1, 1, "res/BlueBalls/PAL_MAIN.png");

    aud_BGM_SpecialStage = newMusic(snd_BGM_SpecialStage_ogg, "snd/BGM_SpecialStage.ogg");
    aud_BGM_SpecialStage->Play(-1);

    aud_C_ShortBeep = newSound(snd_C_ShortBeep_ogg, "snd/C_ShortBeep.ogg");
}
void Scene_SpecialStage::Update() {
    bool Controllable = true;
    bool left, right, up, down, jump, jumpHold;
	
	up = false;

    if (Controllable && app->Mobile) {
        for (int i = 0; i < 4; i++) {
            if (app->input->touchDown[i]) {
                int X2 = 48;
                int Y2 = app->gameHeight - 48;

                int X3 = app->input->touchX[i];
                int Y3 = app->input->touchY[i];

                float angle = modulo(toDegrees(std::atan2(Y2 - Y3, X3 - X2)) + 360, 360);
                float len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

                //printf("Angle: %d Distance: %d\n", (int)angle, (int)len);

                if (len < 100) {
                    if (angle >= 45 && angle < 135) {
                        up = true;
                    }
                    if (angle < 45 || angle >= 315) {
                        right = true;
                    }
                    if (angle >= 135 && angle < 225) {
                        left = true;
                    }
                    if (angle >= 225 && angle < 315) {
                        down = true;
                    }
                }

                X2 = app->renderWidth - 48 + 8;
                Y2 = app->renderHeight - 48 + 8;
                len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

                if (len < 75) {
                    jump = app->input->touchPressed[i];
                    jumpHold = app->input->touchDown[i];
                }
            }
        }

        app->input->left[0] = left;
        app->input->right[0] = right;
        app->input->jump[0] = jump;
    }

    if (!app->realPaused) {
        if (frameTurn == 1) {
            nextMove = 0;
            frameOff += 16;
            frameOff %= 64;
        }

        if (frameTurn > 0) {
            turn += nextMove;
            frameTurn--;
        }

        if (frameTurn == 0) {
            frame++;
            x++;

            x %= lvlWidth * 16;
        }
        frame = (frame) % (16 * 16 * 2 * 3);
        frameSonic++;
        frameSonic %= 24;
        frameNon++;
    }

    if (up || down) {

    }

    if (app->input->left[0] && frameTurn == 0)
        nextMove = -1;
    if (app->input->right[0] && frameTurn == 0)
        nextMove = 1;

    if (frame % 16 == 0 && nextMove != 0 && frameTurn == 0) {
        turn += nextMove * 2;
        frameTurn = 15;
    }

    turn = (turn + 64) % 64;

    if (frameNon > 180) {
        frameNon = 180;
    }

    if (sceneOut > -2) {
        sceneOut--;
        //aud_BGM_TitleScreen->SetVolume(sceneOut / 40.0f);
    }
    if (sceneOut == -2) {
        if (app->input->jumpPressed[0]) {
            sceneOut = 40;
            aud_C_ShortBeep->Play(0);
            app->input->Vibrate(0, 0.75, 250);
        }
    }
    if (sceneOut == -1) {
        //aud_BGM_TitleScreen->Stop();
        app->nextScene = new Scene_MainMenu(app, -1, -1);
    }
}

float CalcZ(int index) {
    int i = index;
    //return 0.03906351847 * index * index - 0.1143319376 * index + 38.90707146;
    return 0.00000147291861 * i * i * i * i + 0.0002500554694 * i * i * i - 0.01076487469 * i * i + 0.9593884187 * i + 38.15772043;
}
float CalcColumn1(float y) {
    return 8.70422 * TenTo(-15) * std::sqrt(6.39754 * TenTo(29) * y - 1.68493 * TenTo(31)) + 15.6361;
}
float CalcColumn(int col, float y) {
    int sign = signum(col);
    col = std::abs(col);
    if (col == 1) {
        y = std::max(y, 26.5f);
        return (8.70422 * TenTo(-15) * std::sqrt(6.39754 * TenTo(29) * y - 1.68493 * TenTo(31)) + 15.6361) * sign;
    }
    else if (col == 2) {
        y = std::max(y, 28.8f);
        return (4.96853 * TenTo(-11) * std::sqrt(7.58575 * TenTo(22) * y - 2.16242 * TenTo(24)) + 25.6445) * sign;
    }
    else if (col == 3) {
        y = std::max(y, 46.4f);
        return (2.0428 * TenTo(-14) * std::sqrt(7.33608 * TenTo(29) * y - 3.29445 * TenTo(31)) + 62.0009) * sign;
    }
    else if (col == 4) {
        y = std::max(y, 51.1f);
        return (1.68632 * TenTo(-14) * std::sqrt(1.81907 * TenTo(30) * y - 8.93642 * TenTo(31)) + 59.3594) * sign;
    }
    return 0;
}
float CalcCurvatureY(int col, float h) {
    float x = CalcColumn(std::abs(col), h);
    float r = 3000 + (80 - (h - 38) * (30.0 / 132.0)) * (h - 38.0);
    return -7 * (std::sqrt(r * r - x * x) - r);
}

void Scene_SpecialStage::Render() {
    // Converting from Palette distance index to function Y (Table)

    // For Desmos:
    // Column 1: 8.70422\cdot10^{-15}\sqrt{6.39754\cdot10^{29}x-1.68493\cdot10^{31}}+15.6361
    // Column 2: 4.96853\cdot10^{-11}\sqrt{7.58575\cdot10^{22}x-2.16242\cdot10^{24}}+25.6445
    // Column 3: 2.0428\cdot10^{-14}\sqrt{7.33608\cdot10^{29}x-3.29445\cdot10^{31}}+62.0009
    // Column 4: 1.68632\cdot10^{-14}\sqrt{1.81907\cdot10^{30}x-8.93642\cdot10^{31}}+59.3594 (slightly offscreen)

    app->ChangeShader(app->shader_test);
    GLint locPalette1 = glGetUniformLocation(app->current_shader, "u_palette1");
    GLint locPalette2 = glGetUniformLocation(app->current_shader, "u_palette2");
    GLint locWaterline = glGetUniformLocation(app->current_shader, "u_waterline");
    GLint locWaterlineStop = glGetUniformLocation(app->current_shader, "u_waterlineStop");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paletteDry->tex);
    glUniform1i(locPalette1, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
    glUniform1i(locPalette2, 2);

    glUniform1f(locWaterline, 0.f);
    glUniform1f(locWaterlineStop, 0.f);





    // 1st Speed:
    // 1 block per 16 frames

    int f = (int)(frame + frameOff);
    if (nextMove == 1 && frameTurn != 0) {
        f += 16;
    }
    for (int n = 0; n < 16; n++) {
        int switchhg = 15 - f % 16;
        int flip = 1;
        if (f % 32 >= 16) {
            flip = 0;
        }
        if (n > switchhg) {
            setPixel(paletteDry->pixels, flip, n, 0xE, 0x8, 0x00);
            setPixel(paletteDry->pixels, 1 - flip, n, 0x6, 0x2, 0x00);
        }
        else {
            setPixel(paletteDry->pixels, flip, n, 0x6, 0x2, 0x00);
            setPixel(paletteDry->pixels, 1 - flip, n, 0xE, 0x8, 0x00);
        }
    }

    setPixel(paletteDry->pixels, 2, 1, 0x0, 0x4, 0xE);
    setPixel(paletteDry->pixels, 2, 2, 0x0, 0xA, 0xE);
    setPixel(paletteDry->pixels, 2, 3, 0xA, 0xC, 0xE);


    glBindTexture(GL_TEXTURE_2D, paletteDry->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, paletteDry->w,
        paletteDry->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        paletteDry->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    int BGoffsetX = (-turn * 8 + 512) % 512;
    if (turn > 0 && turn < 16) // Hotfix
        BGoffsetX -= 512;

    app->drawZ = 0.6f;
    app->drawSpriteH(tex_BG, BGoffsetX + app->renderWidth / 2 + 96, -((frame * 4) % 256), 0 /* frame */, 0 /* frameH */, 1, 0);
    app->drawSpriteH(tex_BG, BGoffsetX + app->renderWidth / 2 + 96, -((frame * 4) % 256) + 256, 0 /* frame */, 0 /* frameH */, 1, 0);
    app->drawSpriteH(tex_BG, BGoffsetX + app->renderWidth / 2 + 96 - tex_BG->w, -((frame * 4) % 256), 0 /* frame */, 0 /* frameH */, 1, 0);
    app->drawSpriteH(tex_BG, BGoffsetX + app->renderWidth / 2 + 96 - tex_BG->w, -((frame * 4) % 256) + 256, 0 /* frame */, 0 /* frameH */, 1, 0);

    app->drawZ = 0.0f;
    if (frameTurn == 0)
        app->drawSpriteH(tex_Floor, app->renderWidth / 2, 0, 0 /* frame */, 0 /* frameH */, 1, 0);
    else
        app->drawSpriteR(tex_Floor, app->renderWidth / 2, 0, (15 - frameTurn) % 4 /* frame */, (15 - frameTurn) / 4 /* frameH */, 1, 0, nextMove == 1 ? -1 : 1, 1, 0);


    glUniform1f(locWaterline, 2.f);
    glUniform1f(locWaterlineStop, 0.f);

    app->drawZ = -0.1f;
	/*
    int ZTable[] = {
        38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40,
        41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 62, 63, 64, 66, 67, 69, 70,
        72, 74, 76, 77, 79, 81, 83, 86, 88, 90, 92, 95, 97, 100, 103, 105,
        108, 111, 114, 117, 121, 124, 127, 131, 135, 139, 143, 147, 151, 155, 160, 165,
        170, 175, 180, 185, 191, 197, 202, 208, 215, 221, 228, 235
    };

	
    for (int n = 0; n < 8; n++) {
        uint index = 80 + 16 - ( (n * 16 - x) + (lvlWidth * 16) ) % (lvlWidth * 16);
        if (index >= 0 && index < sizeof(ZTable) / sizeof(*ZTable)) {
            float Y = ZTable[index];
            float CurvedY = Y + CalcCurvatureY(-1, Y);
            //int ballFrame = (int)std::max(0.f, (112 - CurvedY) / 6);
            float dZ = app->drawZ;
            if (index < 0)
                app->drawZ = 0.4f;
            app->drawSpriteH(tex_Ball, app->renderWidth / 2 + CalcColumn(-1, CurvedY), CurvedY, 0, 2, 1, 1);
            app->drawZ = dZ;
        }
        if (index >= 0 && index < sizeof(ZTable) / sizeof(*ZTable)) {
            float Y = ZTable[index];
            float CurvedY = Y + CalcCurvatureY(1, Y);
            //int ballFrame = (int)std::max(0.f, (112 - CurvedY) / 6);
            float dZ = app->drawZ;
            if (index < 0)
                app->drawZ = 0.4f;
            app->drawSpriteH(tex_Ball, app->renderWidth / 2 + CalcColumn(1, CurvedY), CurvedY, 0, 2, 1, 1);
            app->drawZ = dZ;
        }
        if (index >= 0 && index < sizeof(ZTable) / sizeof(*ZTable)) {
            float Y = ZTable[index];
            float CurvedY = Y + CalcCurvatureY(2, Y);
            //int ballFrame = (int)std::max(0.f, (112 - CurvedY) / 6);
            float dZ = app->drawZ;
            if (index < 0)
                app->drawZ = 0.4f;
            app->drawSpriteH(tex_Ball, app->renderWidth / 2 + CalcColumn(2, CurvedY), CurvedY, 0, 2, 1, 1);
            app->drawZ = dZ;
        }
        if (index >= 0 && index < sizeof(ZTable) / sizeof(*ZTable)) {
            float Y = ZTable[index];
            float CurvedY = Y + CalcCurvatureY(-2, Y);
                //CurvedY = Y + CalcCurvatureY(3, CurvedY);
            //int ballFrame = (int)std::max(0.f, (112 - CurvedY) / 6);
            float dZ = app->drawZ;
            if (index < 0)
                app->drawZ = 0.4f;
            app->drawSpriteH(tex_Ball, app->renderWidth / 2 + CalcColumn(-2, CurvedY), CurvedY, 0, 2, 1, 1);
            app->drawZ = dZ;
        }
    }
	//*/


    app->ChangeShader(app->shader_basic);

    app->drawZ = -0.4f;
    app->drawSpriteR(paletteDry, 0, app->renderHeight, 0 /* frame */, 0 /* frameH */, 0, 2, 3, 3, 0);
    app->drawSpriteR(paletteWet, 12, app->renderHeight, 0 /* frame */, 0 /* frameH */, 0, 2, 3, 3, 0);

    /*Texture* tex_Arrow;
    Texture* tex_Ball;
    Texture* tex_ChaosEmerald;
    Texture* tex_HUDPieceCenter;
    Texture* tex_Ring;
    Texture* tex_Sonic;*/

    //app->drawSpriteR(tex_Arrow, app->renderWidth / 2 - 64, app->renderHeight / 2, 0 /* frame */, 0 /* frameH */, 1, 1, 1, 1, 0);
    //app->drawSpriteR(tex_Arrow, app->renderWidth / 2 + 64, app->renderHeight / 2, 0 /* frame */, 0 /* frameH */, 1, 1, 1, 1, 0);

    int frames[6] = {
        1, 2, 3, 4, 3, 2,
    };

    app->drawSpriteR(tex_Sonic, app->renderWidth / 2, app->renderHeight * 0.75, frames[(frameSonic / 2) % 6] /* frame */, 0 /* frameH */, 1, 1, (frameSonic / 2) % 12 >= 6 ? -1 : 1, 1, 0);

    app->drawSpriteH(tex_HUDPieceCenter, app->renderWidth / 2, 9, 0 /* frame */, 0 /* frameH */, 1, 0);
    app->drawSpriteH(tex_HUDPieceEnds, app->renderWidth / 2 - 136, 9, 0, 0, 2, 0);
    app->drawSpriteH(tex_HUDPieceEnds, app->renderWidth / 2 + 136, 9, 7, 0, 0, 0);

    for (int i = 0; i < 3; i++) {
        int n = HUD_PlaceOfNum(BallsLeft, i, 3);
        app->drawSpriteH(tex_HUDPieceNumbers, app->renderWidth / 2 - 136 + i * 16, 9, n, 0, 0, 0);

        n = HUD_PlaceOfNum(RingsLeft, i, 3);
        app->drawSpriteH(tex_HUDPieceNumbers, app->renderWidth / 2 + 88 + i * 16, 9, n, 0, 0, 0);
    }


    if (app->Mobile) {
        int buttFrame = 0;
        int flip = 0;
        if (app->input->up[0]) {
            buttFrame = 1;
        }
        if (app->input->down[0]) {
            buttFrame = 2;
        }
        if (app->input->right[0]) {
            buttFrame = 3;
            flip = 0;
        }
        if (app->input->left[0]) {
            buttFrame = 3;
            flip = 420;
        }

        app->SetBlend(1.f, 1.f, 1.f, 0.75f);
        app->drawSprite(tex_HUD_Buttons, 48, app->gameHeight - 48, flip, buttFrame, 1, 1);

        app->drawSprite(tex_HUD_Buttons, app->renderWidth - 40, app->gameHeight - 40, 0, 4, 1, 1);

        app->drawSprite(tex_HUD_Buttons, app->renderWidth / 2, 21, 0, 5, 1, 1);
        app->SetBlend(1.f, 1.f, 1.f, 1.f);
    }
}
void Scene_SpecialStage::Free() {
    tex_BG->Free();

    tex_BG = NULL;

    FreeAudio();
}
