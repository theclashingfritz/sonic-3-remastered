#include "SpecialStageRing.h"
#include <LevelScene.h>

void ISpecialStageRing::Create() {
    W = 16;
    H = 32;
    Active = true;
    Priority = false;

    Sprite = App->Sprites["Big Ring"];

    SuperEmeraldRing = false;
    if (SubType >= 0x80)
        SuperEmeraldRing = true;

    if (Scene->act < 0x60)
        SuperEmeraldRing = false;
}
void ISpecialStageRing::Update() {
    if (!SuperEmeraldRing) {
        // if all ChaosEmeralds and Scene->act >= 0x60 (Mushroom Hill)
        // SuperEmeraldRing = true;
    }
    /*if (((CollectedRings >> ((SubType & 0x7F) - 1)) & 1) == 1) {
        Active = false;
        return;
    }*/

    Frame++;
    if (Frame / 7 >= 8) {
        Frame = (Frame % 28) + 8 * 7;
    }
}
void ISpecialStageRing::Render(int CamX, int CamY) {
    G->PaletteShift(-1);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, Frame / 7, IE::CenterAlign | IE::MiddleAlign);
    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2.f, (int)Y - CamY - H / 2.f, -2.9f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int ISpecialStageRing::OnCollisionWithPlayer(int PlayerID, int Data2, int Data3) {
    IPlayer* Player = Scene->GetPlayer(PlayerID);
    if (Frame < 8 * 7)
        return 0;
    if (Player->Action == ActionType::Dead)
        return 0;

    Scene->aud[0xB3]->Play(3);

    /*
    if (AllChaosEmeralds && Scene->act < 0x60) {
        Scene->getRing(50);
        return 0;
    }
    if (AllSuperEmeralds && Scene->act >= 0x60) {
        Scene->getRing(50);
        return 0;
    }*/

    Scene->getRing(50);
    Active = false;
    // set as collected
    return 0;

    // lock player movement
    // set them to object controlled
    // hide them
    // if carried by tails, do that to him too

    return 0;
}
