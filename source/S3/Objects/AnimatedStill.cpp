#include "AnimatedStill.h"
#include <LevelScene.h>

void IAnimatedStill::Create() {
    Active = true;
    Priority = false;

    W = 64;
    H = 64;
}
void IAnimatedStill::Update() {
    if (SubType == 0)
        Frame = ((Scene->frameAnim / 4) % 5);
    if (SubType == 1)
        Frame = ((Scene->frameAnim / 4) % 4) + 5;
}
void IAnimatedStill::Render(int CamX, int CamY) {
    if (Y + 16 < Scene->visualWaterHeight) {
        G->PaletteShift(3);
        G->Scale(FlipX ? -1.f : 1.f, FlipY ? -1.f : 1.f);
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, Frame, IE::CenterAlign | IE::MiddleAlign);
        G->Scale(1.f, 1.f);
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, 0.09f, W, H, IColor(0, 1, 1, 0.5f));
    }
}
