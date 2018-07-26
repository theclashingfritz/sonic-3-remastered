#include "InvisibleSolid.h"
#include <LevelScene.h>

void IInvisibleSolid::Create() {
    Active = true;
    Priority = false;

    if (!Deadly)
        Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Rotation = 0;
    Size = 1 + (SubType & 0xF0) / 0x10;
    W = (SubType & 0xF0) + 0x10;
    H = ((SubType & 0x0F) << 4) + 0x10;

    if (Deadly) {
        W -= 2;
        H -= 2;
    }
}
void IInvisibleSolid::Update() {

}
void IInvisibleSolid::Render(int CamX, int CamY) {
    if (Scene->ShowHitboxes || Painful || Deadly) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, Painful || Deadly ? IColor(1, 0, 0, 0.5f) : IColor(0, 1, 1, 0.5f));
    }
}

int  IInvisibleSolid::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Painful) {
        if (HitFrom == 1 && Rotation == 0 && Scene->MyPlayer->Gravity >= 0) {
            Scene->MyPlayer->Hurt(X, true);
        }
        else if (HitFrom == 3 && Rotation == 180 && Scene->MyPlayer->Gravity <= 0) {
            Scene->MyPlayer->Hurt(X, true);
        }
        else if ((Rotation == 90 && HitFrom == 2) || (Rotation == 270 && HitFrom == 0)) {
            if (Scene->MyPlayer->Y + 20 - 4 - 4 <= Y - 16)
                return 0;

            Scene->MyPlayer->Hurt(X, true);
        }
    }
    else if (Deadly) {
        if (Scene->MyPlayer->Y - 20 > Y + H / 2)
            return 0;
        Scene->MyPlayer->Die();
    }
	return 1;
}
