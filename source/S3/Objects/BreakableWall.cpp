#include "BreakableWall.h"
#include <LevelScene.h>

void IBreakableWall::Create() {
    Active = true;
    Priority = false;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Sprite = App->Sprites["Breakable Wall"];

    W = 32;
    H = 80;

    if (Scene->ActualZone == 0x1) {
        Frame = (SubType & 0xF) >> 1;
    } else if (Scene->ActualZone == 0x3) {
        W = 64;
        H = 80;
        BreakableByRoll = true;
        BreakableByKnuckles = true;
        Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
    } else {
        W = 32;
        H = 64;
        BreakableByRoll = true;
        BreakableByKnuckles = true;
        Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
    }
}

void IBreakableWall::Update() {
    if (Scene->ActualZone == 0x1) {
        if (Scene->ButtonCount > 0) {
            Active = false;
        }
    }
}

void IBreakableWall::Render(int CamX, int CamY) {
    if (Solid) {
        G->PaletteShift(0);
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int IBreakableWall::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    return 1;
}

int IBreakableWall::OnBreakHorizontal(int PlayerID, int HitFrom) {
    if (Scene->GetPlayer(PlayerID)->Y + 20 < Y + H / 2 - 8 && BreakableByRoll && Scene->GetPlayer(PlayerID)->Character != CharacterType::Knuckles)
        return 0;

    if (HitFrom == 3)
        return 0;

    //float FF = (HitFrom == 2) - (HitFrom == 0);

    BreakableByRoll = false;
    BreakableByJump = false;
    BreakableByGlide = false;
    BreakableByKnuckles = false;
    Solid = false;

    Priority = true;

    Scene->aud_BreakDeeper->Play(3);

	return 1;
}

int IBreakableWall::OnBreakVertical(int PlayerID, int HitFrom) {
    if (HitFrom == 1) {
        return OnBreakHorizontal(PlayerID, HitFrom);
    }
	return 0;
}