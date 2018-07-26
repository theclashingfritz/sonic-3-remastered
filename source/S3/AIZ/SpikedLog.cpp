#include "SpikedLog.h"
#include <LevelScene.h>

void ISpikedLog::Create() {
    Active = true;
    Priority = false;

    W = 48;
    H = 16;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Sprite = App->Sprites["Spiked Log"];
}
void ISpikedLog::Update() {
    Frame = (Scene->frameAnim / 4) % 16;
}
void ISpikedLog::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Frame, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  ISpikedLog::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
	return 1;
}
