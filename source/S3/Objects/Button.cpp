#include "Button.h"
#include <LevelScene.h>

void IButton::Create() {
    Active = true;
    Priority = false;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    W = 32;
    H = 26;
    Rotation = 0;

    Y += 4;

    Sprite = App->Sprites["Button"];
    StartY = (int)Y;
}
void IButton::Update() {
    Down = CollidingWithPlayer;

    if (Down) {
        Y = StartY + 8;
        H = 24;
    }
    else {
        Y = StartY;
        H = 26;
    }
}
void IButton::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - (Down ? 8 : 0), -1.01f, 0, Down ? 1 : 0, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.2f, W, H, IColor(0, 1, 1, 0.5f));
    }
}

int  IButton::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom != 1) {
        CollidingWithPlayer = false;
        return 0;
    }

    IPlayer* Player = Scene->GetPlayer(PlayerID);
    if (Player->Y + 20 > Y - H / 2 + 4) {
        CollidingWithPlayer = false;
        return 0;
    }

    if (!Pressed) {
        Pressed = true;
        Scene->aud_C_ShortBeep->Play(2);
        Scene->ButtonCount = SubType;
    }

	return 1;
}
