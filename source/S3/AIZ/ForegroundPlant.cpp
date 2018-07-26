#include "ForegroundPlant.h"
#include <LevelScene.h>

void IForegroundPlant::Create() {
    Active = true;
    Priority = true;

    W = 32;
    H = 32;

    Sprite = App->Sprites["Foreground Plant"];

    Frame = (SubType & 0xF);
    CamMult = (SubType >> 2) & 0x3C;

    StartX = (int)X;
    StartY = (int)Y;
}
void IForegroundPlant::Update() {

}
void IForegroundPlant::Render(int CamX, int CamY) {
    switch (CamMult) {
        case 0x04: // 1
            X = (StartX - 0xA0 - CamX) / 16 + StartX;
            Y = (StartY - 0x70 - CamY) / 16 + StartY;
            break;
        case 0x08: // 2
            X = (StartX - 0xA0 - CamX) / 8 + StartX;
            Y = (StartY - 0x70 - CamY) / 8 + StartY;
            break;
        case 0x0C: // 3
            X = (StartX - 0xA0 - CamX) / 4 + StartX;
            Y = (StartY - 0x70 - CamY) / 4 + StartY;
            break;
        case 0x10: // 4
            X = (StartX - 0xA0 - CamX) / 2 + StartX;
            Y = (StartY - 0x70 - CamY) / 2 + StartY;
            break;
        case 0x14: // 5
            X = (StartX - 0xA0 - CamX) + StartX;
            Y = (StartY - 0x70 - CamY) + StartY;
            break;
        case 0x18: // 6
            X = (StartX - 0xA0 - CamX) * 2 + StartX;
            Y = (StartY - 0x70 - CamY) + StartY;
            break;
        default:
            break;
    }


    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -2.8f, Frame, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IForegroundPlant::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {

	return 1;
}
