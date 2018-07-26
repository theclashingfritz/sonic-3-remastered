#include "DisappearingPlatform.h"
#include <LevelScene.h>

void IDisappearingPlatform::Create() {
    Active = true;
    Priority = false;

    W = 64;
    H = 48;

    Sprite = App->Sprites["Disappearing Platform"];
    Sprite2 = App->Sprites["Disappearing Platform (Water)"];

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Timer = 0;
}
void IDisappearingPlatform::Update() {
    if (Timer < 200) {
        Frame = 4;
        Solid = true;
    }
    else if (Timer < 204) {
        Frame = 3;
        Solid = true;
    }
    else if (Timer < 208) {
        Frame = 2;
        Solid = false;
    }
    else if (Timer < 211) {
        Frame = 1;
        Solid = false;
    }
    else if (Timer < 213) {
        Frame = 0;
        Solid = false;
    }
    else if (Timer < 240) {
        Frame = -1;
        Solid = false;
    }
    else if (Timer < 242) {
        Frame = 0;
        Solid = false;
    }
    else if (Timer < 245) {
        Frame = 1;
        Solid = false;
    }
    else if (Timer < 248) {
        Frame = 2;
        Solid = false;
    }
    else if (Timer < 252) {
        Frame = 3;
        Solid = true;
    }
    else if (Timer < 256) {
        Frame = 4;
        Solid = true;
    }
    Timer = (Scene->frameAnim + (SubType & 0xF0)) & 0xFF;
}
void IDisappearingPlatform::Render(int CamX, int CamY) {
    G->PaletteShift(2);
    if (Frame >= 0)
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Frame, IE::CenterAlign | IE::MiddleAlign);

    G->PaletteShift(3);
    if (Frame == 4)
        G->DrawSprite(Sprite2, (int)X - CamX, (int)Y - CamY, 0.1f, (Timer / 4) % 3, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IDisappearingPlatform::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {

	return 1;
}
