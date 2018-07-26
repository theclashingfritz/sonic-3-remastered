#include "MonkeyDude.h"
#include <LevelScene.h>

void IMonkeyDudeSpore::Update() {
    Gravity += 0.1875f;
    X += Speed;
    Y += Gravity;
}
void IMonkeyDudeSpore::Render(int CamX, int CamY) {
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.12f, 4, IE::CenterAlign | IE::MiddleAlign);
}

void IMonkeyDude::Create() {
    Active = true;
    Priority = false;

    W = 32;
    H = 32;

    Sprite = App->Sprites["MonkeyDude"];

    Spore1 = new IMonkeyDudeSpore();
    Spore1->Sprite = Sprite;
    Spore1->X = X;
    Spore1->Y = Y;
    Spore1->W = 16;
    Spore1->H = 16;
    Spore1->G = G;
    Spore1->App = App;
    Spore1->Scene = Scene;
    Spore1->Invincible = true;

    Scene->ObjectsEnemies[Scene->ObjectEnemiesCount++] = this;

    Frame = 0;
}
void IMonkeyDude::Update() {
    Frame = 0;
    frame_face = (frame_face + 1) % 16;
    frame_hand = (frame_hand + 1) % (112 * 2);
    if (frame_hand % (64 + 48) >= 64) {
        if (frame_hand % 224 < 112 && frame_hand % 16 == 8)
            Y += 8;
        else if (frame_hand % 224 >= 112 && frame_hand % 16 == 8)
            Y -= 8;
    }
    frame_arm = (frame_arm + 1) % 128;

    for (int p = 0; p < 1; p++) {
        IPlayer* Player = Scene->GetPlayer(p);
        if (Spore1->X + Spore1->W / 2 >= Player->X - Player->W / 2 &&
            Spore1->Y + Spore1->H / 2 >= Player->Y - Player->H / 2 &&
            Spore1->X - Spore1->W / 2 <  Player->X + Player->W / 2 &&
            Spore1->Y - Spore1->H / 2 <  Player->Y + Player->H / 2) {
            //Spore1->OnCollisionWithPlayer(p, 1, 0);
        }
    }

    Spore1->Update();
}
void IMonkeyDude::Render(int CamX, int CamY) {
    float Flip = FlipX ? -1.f : 1.f;

    G->PaletteShift(0);

    int offset = 1;
    if (frame_hand % (64 + 48) >= 64)
        offset = 0;

    for (int i = 0; i < 5; i++) {
        int final_x = (int)X - CamX + Flip * -14;
        int final_y = (int)Y - CamY - 2 - (frame_face / 8) * 2 * offset;

        float angle = std::sin((frame_arm - i * 4) / 128.f * Math_PI * 2) * (Math_PI / 3) - Math_PI / 6;
        final_x -= Flip * (int)(std::cos(angle) * i * 8);
        final_y += (int)(std::sin(angle) * i * 8);

        G->DrawSprite(Sprite, final_x, final_y, 0.1f, 3 + (i == 4 ? 1 : 0), IE::CenterAlign | IE::MiddleAlign);

        if (true && i == 4) {
            G->DrawSprite(Sprite, final_x, final_y, 0.09f, 6, IE::CenterAlign | IE::MiddleAlign);
        }
    }

    if (frame_hand % (64 + 48) >= 64) {
        int f = ((frame_hand / 8) % 2) * 2;
        if (frame_hand % 224 < 112) {
            f = 2 - f;
        }
        G->Scale(Flip, 1.0f);
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -2.9f, f, IE::CenterAlign | IE::MiddleAlign);
        G->Scale(1.0f, 1.0f);
    }
    else {
        G->Scale(Flip, 1.0f);
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -2.9f, frame_face / 8, IE::CenterAlign | IE::MiddleAlign);
        G->Scale(1.0f, 1.0f);
    }

    //Spore1->Render(CamX, CamY);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 0, 0, 0.5f));
    }
}
