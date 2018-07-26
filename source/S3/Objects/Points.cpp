#include "Points.h"
#include <LevelScene.h>

void IPoints::Create() {
    Active = true;
    Priority = true;

    W = 32;
    H = 32;

    Sprite = App->Sprites["Points"];
}
void IPoints::Update() {
    if (ghostSp == 0 && ghostTim > 0) {
        ghostY += (24 - ghostY) / 8;
        ghostTim -= 1;
    }
    if (ghostTim <= 0)
        Active = false;
}
void IPoints::Render(int CamX, int CamY) {
    if (ghostY >= 0 && ghostTim > 0) {
        if (ghostTim < 15 && ghostTim % 6 < 3)
            return;

        G->PaletteShift(-1);
        G->DrawSprite(Sprite, (int)X - CamX, (int)(Y - ghostY) - CamY, -2.85f, Frame, IE::CenterAlign | IE::MiddleAlign);
    }
}

void IEnemyExplosion::Create() {
    Active = true;
    Priority = true;

    W = 32;
    H = 32;

    Sprite = App->Sprites["FX (Enemy Explosion)"];
    Frame = 3;
    Speed = 8;

    CanFall = false;
}
void IEnemyExplosion::Update() {
    X += HSpeed;
    Y += Gravity;
    if (CanFall)
        Gravity += 0.21875f;

    Frame++;
    if (Frame >= Sprite->FrameCount * Speed) {
        Active = false;
    }
}
void IEnemyExplosion::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->Scale(FlipX ? -1.f : 1.f, 1.f);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.85f, Frame / (int)Speed, IE::CenterAlign | IE::MiddleAlign);
    G->Scale(1.f, 1.f);
}
