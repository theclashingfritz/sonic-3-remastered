#include "Ridge.h"
#include <LevelScene.h>

void IRidge::Create() {
    Active = true;
    Priority = false;

    W = 64;
    H = 64;
}
void IRidge::Update() {

}
void IRidge::Render(int CamX, int CamY) {
    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IRidge::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (!Scene->MyPlayer->Ground)
        return 0;
    if (Scene->MyPlayer->X + 16 - X < 0)
        return 0;
    if (Scene->MyPlayer->X + 16 - X >= 32)
        return 0;
    if (Scene->MyPlayer->Y - Y < -20)
        return 0;
    if (Scene->MyPlayer->Y - Y > 32)
        return 0;

    if (FlipX) {
        if (Scene->MyPlayer->Speed >= -4)
            return 0;
        Scene->MyPlayer->Speed -= 4;
    }
    else {
        if (Scene->MyPlayer->Speed < 4)
            return 0;
        Scene->MyPlayer->Speed += 4;
    }
    Scene->MyPlayer->Gravity = -7;
    Scene->MyPlayer->Ground = false;
    Scene->MyPlayer->Angle = 0;
    Scene->MyPlayer->AngleMode = 0;
    Scene->MyPlayer->Action = ActionType::Twirl;

	return 1;
}
