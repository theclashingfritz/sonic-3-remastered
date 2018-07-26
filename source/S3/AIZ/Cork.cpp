#include "Cork.h"
#include <LevelScene.h>

void ICork::Create() {
    Active = true;
    Priority = false;

    W = 32;
    H = 86;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;

    Sprite = App->Sprites["Cork Floor"];


    if (Scene->act == 0x11) { // AIZ "Act" 1
        BreakableByJump = true;
        Y -= 2;
    }
    else if (Scene->act == 0x12) { // AIZ "Act" 2
        BreakableByRoll = true;
        BreakableByJump = true;
        Y -= 2;
    }
}
void ICork::Update() {
    for (int i = 0; i < MiniCorks; i++) {
        MiniCorkX[i] += MiniCorkXS[i];
        MiniCorkY[i] += MiniCorkYS[i];
        MiniCorkYS[i] += 0.09375f;
    }
}
void ICork::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    if (Solid) {
        if ((Scene->act & 0xF) <= 1)
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.09f, 0, IE::CenterAlign | IE::MiddleAlign);
        else
            G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.09f, 1, IE::CenterAlign | IE::MiddleAlign);
    }
    else {
        G->Scale(1.f, 1.f);
        for (int i = 0; i < MiniCorks; i++) {
            int w = 16;
            int h = 48;
            int rx = i % 2;
            int ry = i / 2;
            if ((Scene->act & 0xF) <= 1)
                G->DrawSprite(Sprite, rx * w, ry * h, w, h, (int)MiniCorkX[i] - CamX, (int)MiniCorkY[i] - CamY, -1.7f, 0, IE::CenterAlign | IE::MiddleAlign);
            else
                G->DrawSprite(Sprite, rx * w, ry * h, w, h, (int)MiniCorkX[i] - CamX, (int)MiniCorkY[i] - CamY, -1.7f, 1, IE::CenterAlign | IE::MiddleAlign);
        }
        G->Scale(1.f, 1.f);
    }


    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  ICork::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->act == 0x12 && HitFrom == 3 && SubType == 0x00 && BreakableByJump) {
        if (Scene->GetPlayer(PlayerID)->Action != ActionType::Rolling) {
            Scene->GetPlayer(PlayerID)->Gravity = -7.25f;
            Scene->GetPlayer(PlayerID)->Action = ActionType::Spring;
        }
        else {
            Scene->GetPlayer(PlayerID)->Speed = Scene->GetPlayer(PlayerID)->LastSpeed;
        }
        return OnBreakVertical(PlayerID, HitFrom);
    }

	return 1;
}
int  ICork::OnBreakHorizontal(int PlayerID, int HitFrom) {
    return 0;
}
int  ICork::OnBreakVertical(int PlayerID, int HitFrom) {
    float CorkVel3[8] = { -1.0f, -2.0f, 1.0f, -1.875f, -1.6875f, -1.75f, 1.75f, -1.75f };

    if (Scene->act == 0x11 && !(HitFrom == 1))
        return 0;

    if (Scene->act == 0x11 && std::abs(Scene->MyPlayer->X - X) >= 10)
        return 0;

    if (Scene->act == 0x12 && !((HitFrom == 1 && SubType == 0x01) || (HitFrom == 3 && SubType == 0x00)))
        return 0;


    // Large
    MiniCorkX[0] = (int)X - 8;
    MiniCorkY[0] = (int)Y - 24;
    MiniCorkX[1] = (int)X + 8;
    MiniCorkY[1] = (int)Y - 24;
    MiniCorkX[2] = (int)X - 8;
    MiniCorkY[2] = (int)Y + 24;
    MiniCorkX[3] = (int)X + 8;
    MiniCorkY[3] = (int)Y + 24;
    MiniCorks = 4;

    for (int r = 0; r < MiniCorks; r++) {
        MiniCorkXS[r] = CorkVel3[r * 2 + 0];
        MiniCorkYS[r] = CorkVel3[r * 2 + 1];
    }

    BreakableByRoll = false;
    BreakableByJump = false;
    BreakableByGlide = false;
    BreakableByKnuckles = false;
    Solid = false;

    Priority = true;

    Scene->aud_BreakDeeper->Play(3);
	return 1;
}
