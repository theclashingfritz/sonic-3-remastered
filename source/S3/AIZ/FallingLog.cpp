#include "FallingLog.h"
#include <LevelScene.h>

void IFallingLog::Create() {
    Active = true;
    Priority = false;

    W = 48;
    H = 14;

    StartX = (int)X;
    StartY = (int)Y;

    SolidTop = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Sprite = App->Sprites["Falling Log"];

    Frame = OffsetY;
}
void IFallingLog::Update() {
    Priority = true;
    
    int Length = (int)Scene->waterLevel - StartY;
    Y = StartY + std::min(Frame, Length);

    Length = (Length / 0x80) * 0x80 + 0x80;

    Frame = (Frame + 1) % (Length);
}
void IFallingLog::Render(int CamX, int CamY) {
    int Length = (int)Scene->waterLevel - StartY;
    if (Frame < Length || Frame % 8 < 4) {
        G->PaletteShift(0);
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);
    }

    G->PaletteShift(1);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.09f, 1 + (Frame % 16) / 4, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IFallingLog::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
	return 1;
}
