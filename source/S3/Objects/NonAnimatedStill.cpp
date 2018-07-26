#include "NonAnimatedStill.h"
#include <LevelScene.h>

void INonAnimatedStill::Create() {
    Active = true;
    Priority = false;

    W = 128;
    H = 192;

    if (Sprite == NULL)
        Sprite = App->Sprites["Non Anim"];
}
void INonAnimatedStill::Update() {

}
void INonAnimatedStill::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    switch (SubType) {
        // AIZ Act 2
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
            G->Scale(FlipX ? -1.f : 1.f, FlipY ? -1.f : 1.f);
            G->PaletteShift(2);
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, SubType, IE::CenterAlign | IE::MiddleAlign);
            G->Scale(1.f, 1.f);
            break;
        case 0x4:
            G->PaletteShift(3);
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, SubType, IE::CenterAlign | IE::MiddleAlign);
            break;
        case 0x5:
            G->Scale(FlipX ? -1.f : 1.f, FlipY ? -1.f : 1.f);
            G->PaletteShift(2);
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -1.05f, 0, IE::CenterAlign | IE::MiddleAlign);
            G->Scale(1.f, 1.f);
            break;


        // HCZ Act 1
        case 0x6:
            for (int i = 0; i < 16; i++)
                G->DrawSprite(Sprite, (int)X - CamX - 48 + (i % 4) * 32, (int)Y - CamY - 48 + (i / 4) * 32, -1.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
            break;
        case 0x7:
            for (int i = 0; i < 12; i++)
                G->DrawSprite(Sprite, (int)X - CamX - 48 + (i % 4) * 32, (int)Y - CamY - 48 + (i / 4) * 32, -1.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
            break;
        case 0x8:
            for (int i = 0; i < 8; i++)
                G->DrawSprite(Sprite, (int)X - CamX - 48 + (i % 4) * 32, (int)Y - CamY - 32 + (i / 4) * 32, 1.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
            break;
        case 0x9:
            for (int i = 0; i < 16; i++)
                G->DrawSprite(Sprite, (int)X - CamX - 48 + (i % 4) * 32, (int)Y - CamY + 16 + (i / 4 - (i % 4)) * 32, -1.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
            break;
        case 0xA:
            for (int i = 0; i < 16; i++)
                G->DrawSprite(Sprite, (int)X - CamX - 48 + (i % 4) * 32, (int)Y - CamY - 80 + (i / 4 + (i % 4)) * 32, -1.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
            break;

        // MGZ
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
            G->Scale(FlipX ? -1.f : 1.f, FlipY ? -1.f : 1.f);
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -1.1f, SubType - 0xB, IE::CenterAlign | IE::MiddleAlign);
            G->Scale(1.f, 1.f);
            break;

        // HCZ Act 2
        case 0xF:
        case 0x10:
        case 0x11:
        case 0x12:
            G->Scale(FlipX ? -1.f : 1.f, FlipY ? -1.f : 1.f);
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY + 1, -1.1f, SubType - 0xF, IE::CenterAlign | IE::MiddleAlign);
            G->Scale(1.f, 1.f);
            break;
        case 0x13:
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY + 2, 1.1f, SubType - 0xF, IE::CenterAlign | IE::MiddleAlign);
            break;

        default:
            Frame = 0;
            break;
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, 0.09f, W, H, IColor(0, 1, 1, 0.5f));
    }
}
