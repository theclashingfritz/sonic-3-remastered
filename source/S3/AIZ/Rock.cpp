#include "Rock.h"
#include <LevelScene.h>

void IRock::Create() {
    Active = true;
    Priority = false;

    W = 48;
    H = 32;

    if ((SubType & 0x70) == 0x00)
        H = 80;
    else if ((SubType & 0x70) == 0x10)
        H = 48;
    else if ((SubType & 0x70) == 0x20)
        H = 32;

    W -= 4;
    H -= 2;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    if ((SubType & 0xF) == 0x01) {
        BreakableByJump = true;
        Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
    }
    else if ((SubType & 0xF) == 0x02) {
        Pushable = true;
    }
    else if ((SubType & 0xF) == 0x04) {
        BreakableByRoll = true;
        BreakableByKnuckles = true;
        Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
    }
    else if ((SubType & 0xF) == 0x0F) {
        BreakableByKnuckles = true;
        Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
    }

    if (SubType >= 0x80) {
        if (!BreakableByJump && !BreakableByRoll && !BreakableByKnuckles)
            Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
        BreakableByKnuckles = true;
        W = 48;
    }

    Sprite = App->Sprites["Rock"];

    StartX = (int)X;
}
void IRock::Update() {
    X = std::max(StartX - 64.f, std::min(X, StartX + 64.f));
    for (int i = 0; i < MiniRocks; i++) {
        MiniRockX[i] += MiniRockXS[i];
        MiniRockY[i] += MiniRockYS[i];
        MiniRockYS[i] += 0.09375f;
        MiniRockF[i] += 0.333333f;
        if (MiniRockF[i] >= 4.f)
            MiniRockF[i] = 0.f;
    }
}
void IRock::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    if (Solid) {
        if ((Scene->act & 0xF) <= 1)
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.09f, SubType >> 4, IE::CenterAlign | IE::MiddleAlign);
        else
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.09f, ((SubType >> 4) & 7) + 7, IE::CenterAlign | IE::MiddleAlign);
    }
    else {
        G->Scale(-1.f, 1.f);
        for (int i = 0; i < MiniRocks; i++) {
            if ((Scene->act & 0xF) <= 1)
                G->DrawSprite(Sprite, (int)MiniRockX[i] - CamX, (int)MiniRockY[i] - CamY, -1.7f, 3 + (int)MiniRockF[i], IE::CenterAlign | IE::MiddleAlign);
            else
                G->DrawSprite(Sprite, (int)MiniRockX[i] - CamX, (int)MiniRockY[i] - CamY, -1.7f, 10 + (int)MiniRockF[i], IE::CenterAlign | IE::MiddleAlign);
        }
        G->Scale(1.f, 1.f);
    }


    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IRock::OnPush(int PlayerID, int Direction) {
    if (X + Direction > StartX - 64.f && X + Direction < StartX + 64.f) {
        X = std::max(StartX - 64.f, std::min(X, StartX + 64.f));
        return 1;
    }
    X = std::max(StartX - 64.f, std::min(X, StartX + 64.f));
    return 0;
}
int  IRock::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
	return 1;
}
int  IRock::OnBreakHorizontal(int PlayerID, int HitFrom) {
    float RockVel0[16] = { -3.0f, -3.0f, -2.75f, -2.5f, -2.75f, -2.5f, -2.5f, -2.0f, -2.5f, -1.5f, -2.25f, -1.5f, -2.25f, -1.0f, -2.0f, -1.0f };
    //float RockVel1[16] = { 2.75f, -2.5f, 3.0f, -3.0f, 2.5f, -2.0f, 2.75f, -2.5f, 2.25f, -1.5f, 2.5f, -1.5f, 2.0f, -1.0f, 2.25f, -1.0f };
    float RockVel2[10] = { -2.0f, -2.0f, 2.0f, -2.0f, -1.0f, -1.875f, -1.6875f, -1.75f, 1.75f, -1.75f };
    float RockVel3[8] = { -1.0f, -2.0f, 1.0f, -1.875f, -1.6875f, -1.75f, 1.75f, -1.75f };
    /*
    float RockVel4[12] = { -0.6875f, -1.875f, 0.6875f, -1.8125f, -0.5f, -2.0f, 0.5f, -1.875f, -0.84375f, -1.75f, 0.875f, -1.75f };
    float RockVel5[12] = { -2.75f, -2.5f, -2.5f, -2.0f, -2.5f, -1.5f, -2.25f, -1.5f, -2.25f, -1.0f, -2.0f, -1.0f };
    float RockVel6[12] = { 2.75f, -2.5f, 2.5f, -2.0f, 2.5f, -1.5f, 2.25f, -1.5f, 2.25f, -1.0f, 2.0f, -1.0f };
    float RockVel7[16] = { -3.0f, -3.0f, -2.75f, -2.5f, -2.75f, -2.5f, -2.5f, -2.0f, -2.5f, -1.5f, -2.25f, -1.5f, -2.25f, -1.0f, -2.0f, -1.0f };
    float RockVel8[12] = { 0.75f, -1.875f, -0.5f, -1.625f, -0.625f, -1.375f, -1.0f, -1.875f, 0.25f, -1.75f, 1.0f, -1.875f };
    //*/

    if (Scene->GetPlayer(PlayerID)->Y + 20 < Y + H / 2 - 8 && BreakableByRoll)
        return 0;

    if (HitFrom == 3)
        return 0;

    float FF = (HitFrom == 2) - (HitFrom == 0);

    // Large
    if ((SubType & 0x70) == 0x00) {
        MiniRockX[0] = (int)X - 8;
        MiniRockY[0] = (int)Y - 24;
        MiniRockX[1] = (int)X + 11;
        MiniRockY[1] = (int)Y - 28;
        MiniRockX[2] = (int)X - 4;
        MiniRockY[2] = (int)Y - 12;
        MiniRockX[3] = (int)X + 12;
        MiniRockY[3] = (int)Y - 4;
        MiniRockX[4] = (int)X - 12;
        MiniRockY[4] = (int)Y + 4;
        MiniRockX[5] = (int)X + 4;
        MiniRockY[5] = (int)Y + 12;
        MiniRockX[6] = (int)X - 12;
        MiniRockY[6] = (int)Y + 28;
        MiniRockX[7] = (int)X + 12;
        MiniRockY[7] = (int)Y + 28;
        MiniRocks = 8;

        for (int r = 0; r < MiniRocks; r++) {
            MiniRockXS[r] = RockVel0[r * 2 + 0] * -FF;
            MiniRockYS[r] = RockVel0[r * 2 + 1];
            MiniRockF[r] = (3 + r) & 3;
        }
    }
    // Medium
    else if ((SubType & 0x70) == 0x10) {
        MiniRockX[0] = (int)X - 4;
        MiniRockY[0] = (int)Y - 12;
        MiniRockX[1] = (int)X + 11;
        MiniRockY[1] = (int)Y - 12;
        MiniRockX[2] = (int)X - 4;
        MiniRockY[2] = (int)Y - 4;
        MiniRockX[3] = (int)X - 12;
        MiniRockY[3] = (int)Y + 12;
        MiniRockX[4] = (int)X + 12;
        MiniRockY[4] = (int)Y + 12;
        MiniRocks = 5;

        for (int r = 0; r < MiniRocks; r++) {
            if (HitFrom != 1)
                MiniRockXS[r] = std::abs(RockVel2[r * 2 + 0]) * FF;
            else
                MiniRockXS[r] = RockVel2[r * 2 + 0];
            MiniRockYS[r] = RockVel2[r * 2 + 1];
            MiniRockF[r] = (3 + r) & 3;
        }
    }
    // Small
    else if ((SubType & 0x70) == 0x20) {
        MiniRockX[0] = (int)X - 4;
        MiniRockY[0] = (int)Y - 4;
        MiniRockX[1] = (int)X + 12;
        MiniRockY[1] = (int)Y - 4;
        MiniRockX[2] = (int)X - 12;
        MiniRockY[2] = (int)Y + 4;
        MiniRockX[3] = (int)X + 12;
        MiniRockY[3] = (int)Y + 4;
        MiniRocks = 4;

        for (int r = 0; r < MiniRocks; r++) {
            if (HitFrom != 1)
                MiniRockXS[r] = std::abs(RockVel3[r * 2 + 0]) * FF;
            else
                MiniRockXS[r] = RockVel3[r * 2 + 0];
            MiniRockYS[r] = RockVel3[r * 2 + 1];
            MiniRockF[r] = (3 + r) & 3;
        }
    }

    BreakableByRoll = false;
    BreakableByJump = false;
    BreakableByGlide = false;
    BreakableByKnuckles = false;
    Solid = false;
    
    Scene->GetPlayer(PlayerID)->Speed = Scene->GetPlayer(PlayerID)->LastSpeed;

    Priority = true;

    Scene->aud_BreakDeeper->Play(3);

	return 1;
}
int  IRock::OnBreakVertical(int PlayerID, int HitFrom) {
    if (HitFrom == 1) {
        return OnBreakHorizontal(PlayerID, HitFrom);
    }
	return 0;
}
