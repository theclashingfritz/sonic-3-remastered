#include "EggCapsule.h"
#include <LevelScene.h>

void IEggCapsule::Create() {
    Active = true;
    Priority = true;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    W = 64;
    H = 56;
    Rotation = 0;

    Animation = -1;

    Texture = App->Textures["Capsule"];
}
void IEggCapsule::Update() {
    if (CanFall) {
        Gravity += 0.21875f;
        for (float i = 0; i < std::abs(Gravity); i += 0.5f) {
            int angle = 0;
            if (Scene->CollisionAt(X, Y + H / 2, 0, &angle, true, NULL, true)) {
                Gravity = 0;
                CanFall = false;
                Y = (int)Y;
                break;
            }
            Y += signum(Gravity) * 0.5f;
        }
    }

    if (Animation >= 0)
        Animation--;
    if (Animation == 0) {
        Scene->doSwitch = true;
    }
}
void IEggCapsule::Render(int CamX, int CamY) {
    G->PaletteShift(-1);
    G->DrawTexture(Texture, 0, 0, 64, 64, (int)X - CamX - 32, (int)Y - CamY - 32 - 4, -0.1f, 64, 64);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.1f, W, H, IColor(0, 1, 1, 0.5f));
    }
}

int  IEggCapsule::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom == 1 && Rotation == 0) {
        ///*
        if ((Scene->act & 0xF0) != 0x10) {
            if (Scene->MyPlayer->AngleMode == 0 && Scene->MyPlayer->Ground && !Scene->levelComplete) {
                Scene->levelComplete = true;
                Scene->aud_BadnikDestroyed->Play(1);
                App->Stack->Push("Act Clear!", Scene->aud_BGM_ActClear, 100, 0, 1.0f, false, false, true);
                Scene->MyPlayer->Vibrate(0, 0.25, 150);

                Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
            }
        }
        else {
            if (Scene->MyPlayer->AngleMode == 0 && Scene->MyPlayer->Gravity >= 0.25f && Animation == -1) {
                Scene->aud_BadnikDestroyed->Play(1);

                if ((Scene->act & 0xF) == 1) {
                    Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
                    Animation = 120;
                }
                else if ((Scene->act & 0xF) == 2 && Scene->ApparentAct == 1) {
                    Scene->levelComplete = true;
                    Scene->aud_BadnikDestroyed->Play(1);
                    App->Stack->Push("Act Clear!", Scene->aud_BGM_ActClear, 100, 0, 1.0f, false, false, true);
                    Scene->MyPlayer->Vibrate(0, 0.25, 150);
                }
                else {
                    Scene->levelComplete = true;
                    Scene->aud_BadnikDestroyed->Play(1);
                    App->Stack->Push("Act Clear!", Scene->aud_BGM_ActClear, 100, 0, 1.0f, false, false, true);
                    Scene->MyPlayer->Vibrate(0, 0.25, 150);

                    Scene->beginActZoneTransfer((Scene->act & 0xF) + 1);
                }
            }
        }
        //*/
    }
	return 1;
}
