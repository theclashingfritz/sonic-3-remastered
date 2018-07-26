#include "Standard.h"
#include "Application.h"
#include "Scene.h"
#include "TextureAudio.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) app->Textures[name]
#define S(name) app->Sprites[name]

void Scene::FreeAudio() {
    /*
    if (aud_BadnikDestroyed != NULL) aud_BadnikDestroyed->Free(); aud_BadnikDestroyed = NULL;
    if (aud_BGM_Boss1 != NULL) aud_BGM_Boss1->Free(); aud_BGM_Boss1 = NULL;
    if (aud_BGM_Boss2 != NULL) aud_BGM_Boss2->Free(); aud_BGM_Boss2 = NULL;
    if (aud_BGM_DataSelect != NULL) aud_BGM_DataSelect->Free(); aud_BGM_DataSelect = NULL;
    if (aud_BGM_Drowning != NULL) aud_BGM_Drowning->Free(); aud_BGM_Drowning = NULL;
    if (aud_BGM_HCZ1 != NULL) aud_BGM_HCZ1->Free(); aud_BGM_HCZ1 = NULL;
    if (aud_BGM_HCZ2 != NULL) aud_BGM_HCZ2->Free(); aud_BGM_HCZ2 = NULL;
    if (aud_BGM_SpecialStage != NULL) aud_BGM_SpecialStage->Free(); aud_BGM_SpecialStage = NULL;
    if (aud_BGM_TitleScreen != NULL) aud_BGM_TitleScreen->Free(); aud_BGM_TitleScreen = NULL;
    if (aud_BIGLaser != NULL) aud_BIGLaser->Free(); aud_BIGLaser = NULL;
    if (aud_BombDrop != NULL) aud_BombDrop->Free(); aud_BombDrop = NULL;
    if (aud_BossHit != NULL) aud_BossHit->Free(); aud_BossHit = NULL;
    if (aud_Break != NULL) aud_Break->Free(); aud_Break = NULL;
    if (aud_BreakDeeper != NULL) aud_BreakDeeper->Free(); aud_BreakDeeper = NULL;
    if (aud_BubbleGet != NULL) aud_BubbleGet->Free(); aud_BubbleGet = NULL;
    if (aud_Checkpoint != NULL) aud_Checkpoint->Free(); aud_Checkpoint = NULL;
    if (aud_C_ChaosEmerald != NULL) aud_C_ChaosEmerald->Free(); aud_C_ChaosEmerald = NULL;
    if (aud_C_ChaosEmeraldContinue != NULL) aud_C_ChaosEmeraldContinue->Free(); aud_C_ChaosEmeraldContinue = NULL;
    if (aud_C_Drown != NULL) aud_C_Drown->Free(); aud_C_Drown = NULL;
    if (aud_C_EnemyShot != NULL) aud_C_EnemyShot->Free(); aud_C_EnemyShot = NULL;
    if (aud_C_IntoLargeRing != NULL) aud_C_IntoLargeRing->Free(); aud_C_IntoLargeRing = NULL;
    if (aud_C_ResultsRings != NULL) aud_C_ResultsRings->Free(); aud_C_ResultsRings = NULL;
    if (aud_C_RingLoss != NULL) aud_C_RingLoss->Free(); aud_C_RingLoss = NULL;
    if (aud_C_ShortBeep != NULL) aud_C_ShortBeep->Free(); aud_C_ShortBeep = NULL;
    if (aud_C_TailsFly != NULL) aud_C_TailsFly->Free(); aud_C_TailsFly = NULL;
    if (aud_C_ToBonusStage != NULL) aud_C_ToBonusStage->Free(); aud_C_ToBonusStage = NULL;
    if (aud_C_ToSpecialStage != NULL) aud_C_ToSpecialStage->Free(); aud_C_ToSpecialStage = NULL;
    if (aud_Die != NULL) aud_Die->Free(); aud_Die = NULL;
    if (aud_Jump != NULL) aud_Jump->Free(); aud_Jump = NULL;
    if (aud_Ring != NULL) aud_Ring->Free(); aud_Ring = NULL;
    if (aud_Roll != NULL) aud_Roll->Free(); aud_Roll = NULL;
    if (aud_ShieldBounce != NULL) aud_ShieldBounce->Free(); aud_ShieldBounce = NULL;
    if (aud_ShieldBubble != NULL) aud_ShieldBubble->Free(); aud_ShieldBubble = NULL;
    if (aud_ShieldEJump != NULL) aud_ShieldEJump->Free(); aud_ShieldEJump = NULL;
    if (aud_ShieldElectric != NULL) aud_ShieldElectric->Free(); aud_ShieldElectric = NULL;
    if (aud_ShieldFire != NULL) aud_ShieldFire->Free(); aud_ShieldFire = NULL;
    if (aud_Skid != NULL) aud_Skid->Free(); aud_Skid = NULL;
    if (aud_Spiked != NULL) aud_Spiked->Free(); aud_Spiked = NULL;
    if (aud_SpikeHurt != NULL) aud_SpikeHurt->Free(); aud_SpikeHurt = NULL;
    if (aud_SpinRev != NULL) aud_SpinRev->Free(); aud_SpinRev = NULL;
    if (aud_Spring != NULL) aud_Spring->Free(); aud_Spring = NULL;
   */
}

void Scene::drawText(Texture* texture, const char* text, float x, float y, int xa, int ya, int type) {
    drawText(texture, text, x, y, xa, ya, type, 0x00);
}
void Scene::drawText(Texture* texture, const char* text, float x, float y, int xa, int ya, int type, char offset) {
    for (unsigned char p = 0; p < strlen(text); p++) {
        if (text[p] >= 0x41 && text[p] <= 0x5A) {
            // Uppercase letters
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x41, type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] >= 0x61 && text[p] <= 0x7A) {
            // Lowercase letters
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x61, 1 - type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] >= 0x30 && text[p] <= 0x39) {
            // Numbers
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x30 + 26, type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] == 0x3A) {
            // Colon (:)
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x3A + 36, type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] == 0x3B) {
            // Semi-Colon (;), which maps to colon
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x3B + 36, 1 - type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] == 0x2D) {
            // Dash (-)
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x2D + 37, type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] == 0x27) {
            // Apostrophe (')
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x27 + 38, type, 0, ya, 1.f, 1.f, 23);
        }
        else if (text[p] == 0x22) {
            // Quote (")
            app->drawSpriteOPTeasy(texture, x - strlen(text) * 4 * xa + p * 8, y, text[p] - offset - 0x22 + 39, type, 0, ya, 1.f, 1.f, 23);
        }
        //0xDEADFACE
    }
    app->renderSpriteOPT(texture, 23);
}
void Scene::drawLongFont(Texture* fnt, const char* text, float x, float y, float align) {
    return;
}
void Scene::DrawTextDebug(const char* text, float x, float y, int xa, int ya) {
    ITexture* fnt = App->Textures["DebugFont"];
    for (unsigned char p = 0; p < strlen(text); p++) {
        G->DrawTexture(fnt, (text[p] >> 4) * 8, (text[p] & 0xF) * 8, 8, 8, x - strlen(text) * 4 * xa + p * 8, y - 4 * ya, -4.999999f, 8, 8);
        //0xDEADFACE
    }
}


void Scene::DrawLongFont(ISprite* fnt, const char* text, float X, float Y, float Align) {
    int spacing = 2;
    int x_offset = 0;
    int text_width = 0;

    for (int p = 0; p < strlen(text); p++) {
        int frame = text[p] - 65;
        if (text[p] == ' ')
            frame = 26;
        else if (frame < 0 || frame > 26)
            frame = 26;
        if (frame < 26)
            text_width += ((fnt->Frame[frame].U_right - fnt->Frame[frame].U_left) * fnt->Texture->Width) + spacing;
        else
            text_width += 8 + spacing;

        if (text[p] == 'M' || text[p] == 'W')
            text_width -= 2;
    }
    text_width -= spacing;

    for (int p = 0; p < strlen(text); p++) {
        int frame = text[p] - 65;
        if (text[p] != ' ') {
            if (frame < 0 || frame > 26) {
                frame = 26;
            }
            G->DrawSprite(fnt, X - text_width * Align / 2 + x_offset, Y, Z_FRONT * 5, frame, IE::LeftAlign | IE::MiddleAlign);
            x_offset += ((fnt->Frame[frame].U_right - fnt->Frame[frame].U_left) * fnt->Texture->Width) + spacing;
        }
        else {
            x_offset += 8 + spacing;
        }
        if (text[p] == 'M' || text[p] == 'W')
            x_offset -= 2;
    }
}
void Scene::DrawSmallFont(ISprite* fnt, const char* text, float x, float y, float Align) {
    //int spacing = 0;

    char offset = 0x00;
    int xa = (int)Align;
    /*
    if ((Align & IE::LeftAlign) == IE::LeftAlign)
        xa = 0;
    if ((Align & IE::RightAlign) == IE::RightAlign)
        xa = 2;
    //*/

    x += 4 * xa;

    for (unsigned char p = 0; p < strlen(text); p++) {
        if (text[p] >= 0x41 && text[p] <= 0x5A) {
            // Uppercase letters
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x41, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] >= 0x61 && text[p] <= 0x7A) {
            // Lowercase letters
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x61, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] >= 0x30 && text[p] <= 0x39) {
            // Numbers
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x30 + 26, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] == 0x3A) {
            // Colon (:)
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x3A + 36, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] == 0x3B) {
            // Semi-Colon (;), which maps to colon
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x3B + 36, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] == 0x2D) {
            // Dash (-)
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x2D + 37, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] == 0x27) {
            // Apostrophe (')
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x27 + 38, IE::CenterAlign | IE::MiddleAlign);
        }
        else if (text[p] == 0x22) {
            // Quote (")
            G->DrawSprite(fnt, x - strlen(text) * 4 * xa + p * 8, y, Z_FRONT * 5, text[p] - offset - 0x22 + 39, IE::CenterAlign | IE::MiddleAlign);
        }
    }
}

void Scene::setPixel(unsigned char* pixels, int x, int y, short rgb) {
    setPixel(pixels, x, y, rgb & 0xE, (rgb & 0xE0) >> 4, (rgb & 0xE00) >> 8);
}
void Scene::setPixel(unsigned char* pixels, int x, int y, char r, char g, char b) {
	char i = y * 4 + x;

	pixels[i * 4 + 0] = r * 0x10 + r;
	pixels[i * 4 + 1] = g * 0x10 + g;
	pixels[i * 4 + 2] = b * 0x10 + b;
	pixels[i * 4 + 3] = 0xFF;
}
void Scene::setPixelFull(unsigned char* pixels, int x, int y, char r, char g, char b) {
	char i = y * 4 + x;

	pixels[i * 4 + 0] = r;
	pixels[i * 4 + 1] = g;
	pixels[i * 4 + 2] = b;
	pixels[i * 4 + 3] = 0xFF;
}
