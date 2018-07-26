#include "Starpole.h"
#include <LevelScene.h>

void IStarpole::Create() {
    Active = true;
    Priority = false;

    W = 16;
    H = 64;
    Rotation = 0;
    Y -= 6;
}
void IStarpole::Update() {
    if (Scene->checkpoint >= SubType)
        Frame = (Frame + 1) % 8;

    if (Animation > 0) {
        Animation--;
        Priority = true;
    }
}
void IStarpole::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    if (Animation == -1 || Animation == 0) {
        int fr = (Frame / 4) * 4;
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, fr, IE::CenterAlign | IE::MiddleAlign);
    }
    else {
        int fr = (Frame / 4) + 2;
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 1, IE::CenterAlign | IE::MiddleAlign);
        G->DrawSprite(Sprite,
            (int)X - CamX - std::sin(toRadians(Animation * 18)) * 14,
            (int)Y - CamY - 18 - std::cos(toRadians(Animation * 18)) * 14, 0.09f, fr, IE::CenterAlign | IE::MiddleAlign);
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, 0.089f, W, H, IColor(0, 1, 1, 0.5f));
    }
}

int  IStarpole::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->checkpoint < SubType) {
        Scene->checkpoint = SubType;
        Scene->aud_Checkpoint->Play(3);
        Scene->MyPlayer->Vibrate(0, 0.35, 50);
        Animation = 60;
    }
	return 1;
}
