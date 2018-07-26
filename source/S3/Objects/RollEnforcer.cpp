#include "RollEnforcer.h"
#include <LevelScene.h>

void IRollEnforcer::Create() {
    Active = true;
    Priority = false;

    W = 28;
    H = 28;
}
void IRollEnforcer::Update() {

}
void IRollEnforcer::Render(int CamX, int CamY) {
    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IRollEnforcer::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->MyPlayer->Speed == 0) {
        if ((SubType & 0xF) != 0x4) {
            if (signum(Scene->MyPlayer->Flip) == (FlipX ? -1 : 1)) {
                Scene->MyPlayer->ForceRoll = true;
                Scene->MyPlayer->X = std::floor(Scene->MyPlayer->X);
                Scene->MyPlayer->Y = std::floor(Scene->MyPlayer->Y);
                Scene->MyPlayer->Speed = signum(Scene->MyPlayer->Speed) * 5.5f;
            }
            else {
                Scene->MyPlayer->ForceRoll = false;
            }
        }
    }
    else {
        if ((SubType & 0xF) != 0x4) {
            if (signum(Scene->MyPlayer->Speed) == (FlipX ? -1 : 1)) {
                Scene->MyPlayer->ForceRoll = true;
                Scene->MyPlayer->X = std::floor(Scene->MyPlayer->X);
                Scene->MyPlayer->Y = std::floor(Scene->MyPlayer->Y);
                Scene->MyPlayer->Speed = signum(Scene->MyPlayer->Speed) * 5.5f;
            }
            else {
                Scene->MyPlayer->ForceRoll = false;
            }
        }
        else {
            Scene->MyPlayer->ForceRoll = false;
            Scene->MyPlayer->X = std::floor(Scene->MyPlayer->X);
            Scene->MyPlayer->Y = std::floor(Scene->MyPlayer->Y);
        }
    }
	return 1;
}
