#include "Bloominator.h"
#include <LevelScene.h>

void IBloominatorSpore::Update() {
    Gravity += 0.1875f;
    X += Speed;
    Y += Gravity;
}
void IBloominatorSpore::Render(int CamX, int CamY) {
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.12f, 4, IE::CenterAlign | IE::MiddleAlign);
}

void IBloominator::Create() {
    Active = true;
    Priority = false;

    W = 32;
    H = 48;

    Sprite = App->Sprites["Bloominator"];

    Spore1 = new IBloominatorSpore();
    Spore1->Sprite = Sprite;
    Spore1->X = X;
    Spore1->Y = Y;
    Spore1->W = 16;
    Spore1->H = 16;
    Spore1->G = G;
    Spore1->App = App;
    Spore1->Scene = Scene;
    Spore1->Invincible = true;
    Spore1->BounceOffShield = true;

    Spore2 = new IBloominatorSpore();
    Spore2->Sprite = Sprite;
    Spore2->X = X;
    Spore2->Y = Y;
    Spore2->W = 16;
    Spore2->H = 16;
    Spore2->G = G;
    Spore2->App = App;
    Spore2->Scene = Scene;
    Spore2->Invincible = true;
    Spore2->BounceOffShield = true;

    Scene->ObjectsEnemies[Scene->ObjectEnemiesCount++] = this;

    Frame = 0;
}
void IBloominator::Update() {
    if (Frame == 31) {
        Spore1->X = X;
        Spore1->Y = Y - 16;
        Spore1->Speed = -1;
        Spore1->Gravity = -5;
        Scene->aud[0x4D]->Play(2);
    }
    else if (Frame == 62) {
        Spore2->X = X;
        Spore2->Y = Y - 16;
        Spore2->Speed = 1;
        Spore2->Gravity = -5;
        Scene->aud[0x4D]->Play(2);
    }
    else if (Frame == 182) {
        Frame = 0;
    }
    Frame++;

    for (int p = 0; p < 1; p++) {
        IPlayer* Player = Scene->GetPlayer(p);
        if (Spore1->X + Spore1->W / 2 >= Player->X - Player->W / 2 &&
            Spore1->Y + Spore1->H / 2 >= Player->Y - Player->H / 2 &&
            Spore1->X - Spore1->W / 2 <  Player->X + Player->W / 2 &&
            Spore1->Y - Spore1->H / 2 <  Player->Y + Player->H / 2) {
            Spore1->OnCollisionWithPlayer(p, 1, 0);
        }
        if (Spore2->X + Spore2->W / 2 >= Player->X - Player->W / 2 &&
            Spore2->Y + Spore2->H / 2 >= Player->Y - Player->H / 2 &&
            Spore2->X - Spore2->W / 2 <  Player->X + Player->W / 2 &&
            Spore2->Y - Spore2->H / 2 <  Player->Y + Player->H / 2) {
            Spore2->OnCollisionWithPlayer(p, 1, 0);
        }
    }

    Spore1->Update();
    Spore2->Update();
}
void IBloominator::Render(int CamX, int CamY) {
    int Subframe = 0;
    int Keyframe = Frame - 1;
    if (Keyframe >= 14 && Keyframe < 62 + 6) {
        if (Keyframe >= 14 && Keyframe < 14 + 11) {
            Subframe = 1;
        }
        else if (Keyframe >= 25 && Keyframe < 25 + 6) {
            Subframe = 2;
        }
        else if (Keyframe >= 31 && Keyframe < 31 + 6) {
            Subframe = 3;
        }

        else if (Keyframe >= 45 && Keyframe < 45 + 11) {
            Subframe = 1;
        }
        else if (Keyframe >= 56 && Keyframe < 56 + 6) {
            Subframe = 2;
        }
        else if (Keyframe >= 62 && Keyframe < 62 + 6) {
            Subframe = 3;
        }
    }
    G->PaletteShift(0);

    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Subframe, IE::CenterAlign | IE::MiddleAlign);

    Spore1->Render(CamX, CamY);
    Spore2->Render(CamX, CamY);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 0, 0, 0.5f));
    }
}
