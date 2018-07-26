#include "AIZBoss.h"
#include <LevelScene.h>

void IAIZBoss::Create() {
    Active = true;
    Priority = true;

    W = 64;
    H = 64;

    Sprite = App->Sprites["Boss"];
    SpriteFlame = App->Sprites["Boss Flame"];
    //SpriteSmall = App->Sprites["Boss Small"];
    Palette = App->Textures["Palette (Boss)"];

    HitCount = 8;

    Speed = 0;
    Gravity = 0;
    Radius = 28;

    Timer = 240;
}
void IAIZBoss::Update() {
    if (Scene->LevelRoutine <= 7)
        return;

    if (FirstRun == -1) {
        App->Stack->Push("Boss", Scene->aud_BGM_Boss2, 10, 0, 3.0f, true, false);
        FirstRun = 0;
    }

    if (HitCount > 0) {
        X += Speed;
        Y += Gravity;
        Frame = (Frame + 1) % 64;

        if (HitTimer > -1)
            HitTimer--;
    }
    else {
        //Scene->doSwitch = true;
    }

    if (Scene->LevelRoutine == 8 && OnScreen) {
        X = 0x4900;
        Y = 0x01D0;
    }

    if (Scene->LevelRoutine == 8) {
        if (Timer == 0)
            Scene->LevelRoutine = 9;

        if (Timer >= 0)
            Timer--;
    }


    // to start the load: Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
    // to do the switch:  Scene->doSwitch = true;
}
void IAIZBoss::Render(int CamX, int CamY) {
    /*G->PaletteShift(0);
    G->Scale(Flip, 1.f);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY + 32, 0.09f, 1 + (Scene->frameAnim % 4) / 2, IE::CenterAlign | IE::MiddleAlign);

    if (HitTimer > -1)
        G->PaletteShift(HitTimer % 2);
    G->SetNextSpriteDrawPalette(Palette, Palette);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);

    // "Nose"
    G->DrawSprite(Sprite, (int)X - CamX - 36 * Flip, (int)Y - CamY + 8, 0.11f, 6, IE::CenterAlign | IE::MiddleAlign);

    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - 32, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);
    G->DrawSprite(Sprite, (int)X - CamX + 9 * Flip, (int)Y - CamY - 28, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);
    G->DrawSprite(Sprite, (int)X - CamX + 18 * Flip, (int)Y - CamY - 24, 0.11f, 3, IE::CenterAlign | IE::MiddleAlign);

    G->SetNextSpriteDrawPalette(NULL, NULL);
    Scene->RestorePalette();
    G->Scale(1.f, 1.f);*/

    if (Scene->ShowHitboxes || true) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - Radius, (int)Y - CamY - Radius, -1.2f, Radius * 2, Radius * 2, IColor(0.5f, 0.5f, 0.5f, 0.5f));
    }
}

int  IAIZBoss::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->LevelRoutine <= 7)
        return 0;

    if (HitTimer != -1)
        return 0;

    if (!Scene->MyPlayer->Ground) {
        if (Scene->MyPlayer->Action == ActionType::Jumping ||
            Scene->MyPlayer->Action == ActionType::Glide) {
            int dis2 = (X - Scene->MyPlayer->X) * (X - Scene->MyPlayer->X);
            dis2    += (Y - Scene->MyPlayer->Y) * (Y - Scene->MyPlayer->Y);

            int cut = 16 + Radius;
            if (dis2 < cut * cut) {
                HitTimer = 32;

                Scene->MyPlayer->Speed   = -Scene->MyPlayer->Speed;
                Scene->MyPlayer->Gravity = -Scene->MyPlayer->Gravity;

                App->print(1, "Hit");

                if (Scene->MyPlayer->Action == ActionType::Glide) {
                    Scene->MyPlayer->Action = ActionType::GlideFall;
                }
                Scene->aud_BossHit->Play(1);

                if (HitCount == 1) {
                    //Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
                    App->Stack->Remove("Boss");

                    Scene->givePoint(1000, -1, -1, -1);

                    Scene->levelComplete = true;
                    Scene->aud_BadnikDestroyed->Play(1);
                    App->Stack->Push("Act Clear!", Scene->aud_BGM_ActClear, 100, 0, 1.0f, false, false, true);
                    Scene->MyPlayer->Vibrate(0, 0.25, 150);

                    Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
                }
                HitCount--;

                Scene->MyPlayer->Vibrate(0, 0.5, 500);
            }
        }
        else {
            Scene->MyPlayer->Hurt(X, false);
        }
    }
    else {
        Scene->MyPlayer->Hurt(X, false);
    }
	return 1;
}
