#include "Spikes.h"
#include <LevelScene.h>

void ISpikes::Create() {
    Active = true;
    Priority = false;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Rotation = 0;
    Size = 1 + (SubType & 0x30) / 0x10;
    W = 32 * Size;
    H = 32;

    Vertical = (SubType >> 6) & 0x1;
    if (Vertical && FlipX) {
        W = 32;
        H = 32 * Size;
        Rotation = 270;
    }
    if (Vertical && !FlipX) {
        W = 32;
        H = 32 * Size;
        Rotation = 90;
    }
    if (!Vertical && FlipY) {
        W = 32 * Size;
        H = 32;
        Rotation = 180;
    }

    if ((SubType & 0xF) == 0x1) {
        if (FlipY) {
            Y += 32;
        }
    }

    Cos[0] = 1;
    Cos[1] = 0;
    Cos[2] = -1;
    Cos[3] = 0;

    Sin[0] = 0;
    Sin[1] = 1;
    Sin[2] = 0;
    Sin[3] = -1;
}
void ISpikes::Update() {
    if ((SubType & 0xF) == 0x1 || (SubType & 0xF) == 0x2) {
        Timer++;
        Timer %= 64;

        if (Timer >= 60) {
            if (Timer == 60) In = !In;

            if (In) {
                X += Sin[Rotation / 90] * 8;
                Y += Cos[Rotation / 90] * 8;
            }
            else {
                X += Sin[Rotation / 90] * -8;
                Y += Cos[Rotation / 90] * -8;
            }
        }
    }
}
void ISpikes::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    if (Rotation == 270)
        G->Scale(-1.f, 1.f);
    for (int i = 0; i < Size; i++) {
        if (Vertical)
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - (Size - 1) * 16 + i * 32, -0.1f, toRadians(Rotation), 0, IE::CenterAlign | IE::MiddleAlign);
        else
            G->DrawSprite(Sprite, (int)X - CamX - (Size - 1) * 16 + i * 32, (int)Y - CamY, -0.1f, toRadians(Rotation), 0, IE::CenterAlign | IE::MiddleAlign);
    }
    G->Scale(1.f, 1.f);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.11f, W, H, IColor(0, 1, 1, 0.5f));
    }
}

int  ISpikes::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom == 1 && Rotation == 0 && Scene->MyPlayer->Gravity >= 0) {
        Scene->MyPlayer->Hurt(X, true);
    }
    else if (HitFrom == 3 && Rotation == 180 && Scene->MyPlayer->Gravity < 0 && !In && ((SubType & 0xF) == 0x1 || (SubType & 0xF) == 0x2)) {
        Scene->MyPlayer->Hurt(X, true);
    }
    else if (HitFrom == 3 && Rotation == 180 && Scene->MyPlayer->Gravity < 0 && In && !((SubType & 0xF) == 0x1 || (SubType & 0xF) == 0x2)) {
        Scene->MyPlayer->Hurt(X, true);
    }
    else if ((Rotation == 90 && HitFrom == 2) || (Rotation == 270 && HitFrom == 0)) {
        if (Scene->MyPlayer->Y + 20 - 4 - 4 <= Y - 16)
            return 0;

        Scene->MyPlayer->Hurt(X, true);
    }
	return 1;
}
