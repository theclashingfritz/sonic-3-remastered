#include "Ring.h"
#include <LevelScene.h>

void IRing::Create() {
    W = 16;
    H = 16;
    Active = true;
    Priority = false;
}
void IRing::Update() {
    if (!CanBeCollected) {
        if (Glitter >= 0)
            Glitter--;

        if (Glitter == -1)
            Active = false;
    }

    if (CanBeCollected) {
        if (CanFall) {
            Gravity += 0.09375f;
            for (float i = 0; i < std::abs(Gravity); i += 0.25f) {
                int angle = 0;
                if (Scene->CollisionAt(X, Y + 8, 0, &angle, false, NULL, true)) {
                    Gravity = Gravity * -0.75f;
                }
                else if (Scene->CollisionAt(X, Y - 8, 0, &angle, false, NULL, false)) {
                    Gravity = std::abs(Gravity) * 0.75f;
                }
                Y += signum(Gravity) * 0.25f;
            }
            for (float i = 0; i < std::abs(Speed); i += 1) {
                int angle = 0;
                if (Scene->CollisionAt(X + 8 * signum(Speed), Y, 0, &angle, false, NULL, false)) {
                    Speed = Speed * -0.25f;
                }
                X += signum(Speed);
            }
        }
        else if (!CanFall && Scene->MyPlayer->Shield == ShieldType::Electric) {
            float XL = Scene->MyPlayer->X - X;
            float YL = Scene->MyPlayer->Y - Y;
            if (XL * XL + YL * YL < 64 * 64) {
                X += signum(XL) * 8;
                Y += signum(YL) * 8;
            }
        }
    }

    if (NonCollect > 0)
        NonCollect--;

    if (Lifespan > 0)
        Lifespan--;

    if (Lifespan == 0)
        Active = false;
}
void IRing::Render(int CamX, int CamY) {
    if (Sprite != NULL) {
        G->PaletteShift(0);
        if (CanBeCollected) {
            if (!CanFall || Lifespan > 128 || (Lifespan > 64 && Lifespan % 8 < 4) || Lifespan % 4 < 2)
                G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, (Scene->frameAnim / 8) % 4, IE::CenterAlign | IE::MiddleAlign);
        }
        else {
            int Frame = (24 - 1 - Glitter) / 6;
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, 4 + Frame, IE::CenterAlign | IE::MiddleAlign);
        }
    }
    else {
        if (Scene->ShowHitboxes || true) {
            G->PaletteShift(-1);
            G->DrawRectangle((int)(std::round(X) - CamX - W / 2.f), (int)(std::round(Y) - CamY - H / 2.f), -3.9f, W, H, IColor(1, 1, 1, 0.5f));
        }
    }
}

int  IRing::OnCollisionWithPlayer(int Data1, int Data2, int Data3) {
    if (CanBeCollected && NonCollect <= 0 && (!CanFall || Scene->MyPlayer->InvincibilityTimer < 90)) {
        CanBeCollected = false;
        Priority = true;
        Speed = 0;
        Gravity = 0;

        Scene->getRing(1);
        Scene->aud_Ring->Play(3);
    }
    return 0;
}
