#include "RideVine.h"
#include <LevelScene.h>

void IRideVine::Create() {
    Active = true;
    Priority = false;

    W = 16;
    H = 16;

    Sprite = App->Sprites["Swing Vine"];

    StartX = X;
    StartY = Y;
    BaseX = X;
    BaseY = Y;
    PotentialEnergy = 0;
    Timer = 0;

    Y += 64;
	App->print(3, "RideVine: Object created in current zone!");
}

void IRideVine::Update() {
    t++;
    t %= 180;
    Angle = std::sin(t / 180.f * 2 * Math_PI) * 90.f;
    X = X + std::sin(Angle * Math_PI / 180.f) * 144;
    Y = Y + std::cos(Angle * Math_PI / 180.f) * 144;
    if (PlayerID != -1) {
        if (Scene->player->ObjectControlled == 0) {
            PlayerID = -1;
            return;
        }

        Scene->player->X = X + std::sin(0 * Math_PI / 180.f) * 20 + std::cos(0 * Math_PI / 180.f) * 2 * Scene->player->Flip;
        Scene->player->Y = Y + std::cos(0 * Math_PI / 180.f) * 20 + std::sin(0 * Math_PI / 180.f) * 2 * Scene->player->Flip;
        Scene->player->Angle = 0;//Angle * Math_PI / 180.f;

        /*
		// Manually set Animation Frame for Sonic swinging
        Scene->player->AnimationOffset = -((int)Scene->player->Flip - 1);
        if (std::abs(Angle) < 45) {
            Scene->player->AnimationOffset = 1;
        } else if (Angle > 45) {
            Scene->player->AnimationOffset = 2 - Scene->player->AnimationOffset;
		}
		*/

        Scene->player->Action = ActionType::ObjectGrab;
        Scene->player->Speed = signum(std::sin(Angle * Math_PI / 180.f)) * 2.5f;
    }
}
void IRideVine::Render(int CamX, int CamY) {
	G->PaletteShift(0);
    G->DrawSprite(Sprite, X - CamX - (Scene->player->X - (int)Scene->player->X), Y - CamY - (Scene->player->Y - (int)Scene->player->Y), 0, 4, IE::CenterAlign | IE::MiddleAlign);

    /*int framesAtAngle[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0, -1,
        0,  0,  0, -1, -1, -1, -1, -1, -1,
        0,  0,  0, -1, -1, -1, -1, -1, -1,
    };*/

    //float vineSegmentX = $X;
    //float vineSegmentY = $Y;
    //for (int i = 0; i < 9; i++) {
        //App->drawSpriteOPTeasy(Scene->tex_objects[0x06], vineSegmentX - camX, vineSegmentY - camY, vineSegmentFrame % 8, vineSegmentFrame / 8, 1, 1, 1.f, 1.f, 6);
    //}
    //App->renderSpriteOPT(Sprite, 6);
	
    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 0, 0, 0.5f));
    }
}

int IRideVine::OnGrabbed(int PlayerID) {
    if (this->PlayerID == -1) {
        if (Scene->MyPlayer->ObjectControlled != 0)
            return 0;

        this->PlayerID = PlayerID;
        return 1;
    }
    return 0;
}

int IRideVine::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->MyPlayer->X - X + 16 >= 32)
        return 0;
    if (Scene->MyPlayer->Y - Y >= 24)
        return 0;
    if (Scene->MyPlayer->Ground)
        return 0;
    if (Scene->MyPlayer->ObjectControlled != 0)
        return 0;
    if (Scene->MyPlayer->LastGrab == this)
        return 0;
    if (Scene->DebugMode != 0)
        return 0;
    if (!Active)
        return 0;

    // 0618 secondary water level AIZ2

    Scene->MyPlayer->Speed = 0;
    Scene->MyPlayer->Gravity = 0;
    Scene->MyPlayer->X = X;
    Scene->MyPlayer->Y = Y + 18;
    Scene->MyPlayer->Action = ActionType::Grab;
    Scene->MyPlayer->ObjectControlled = 3;
    Scene->MyPlayer->Flip = 1;
    Scene->MyPlayer->LastGrab = this;

    Scene->aud[0x4A]->Play(1);

    if (PotentialEnergy == 0) {
        Speed = 8;
        Gravity = 2;
    }

	return 1;
}

void IZiplinePeg::Create() {
    Active = true;
    Priority = false;

    W = 64;
    H = 24;

    Sprite = App->Sprites["Zipline Peg"];
}

void IZiplinePeg::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);
}