#include "Spring.h"
#include <LevelScene.h>

void ISpring::Create() {
    Active = true;
    Priority = false;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;
    Scene->ObjectsSpring[Scene->ObjectSpringCount++] = this;

    W = 32;
    H = 18;
    Rotation = 0;

    if ((SubType & 0xF0) == 0x10 && !FlipX) {
        W = 16;
        H = 32;
        Rotation = 270;
        X--;
    }
    if ((SubType & 0xF0) == 0x10 && FlipX) {
        W = 16;
        H = 32;
        Rotation = 90;
    }
    if ((SubType & 0xF0) == 0x20) {
        W = 32;
        H = 16;
        Rotation = 180;
    }
    Animation = -1;
}
void ISpring::Update() {
    /* Frames
    -1 -> 0
    0  -> 1
    1  -> 0
    2  -> 0
    3  -> 2
    4  -> 2
    5  -> 2
    6  -> 2
    7  -> 2
    8  -> 2
    9  -> 2
    10
    //*/
    if (Animation >= 0)
        Animation++;
    if (Animation == 10)
        Animation = -1;
}
void ISpring::Render(int CamX, int CamY) {
    Frame = 2;
    if (Animation == -1 || Animation == 2 || Animation == 3)
        Frame = 0;
    if (Animation == 1)
        Frame = 1;

    G->PaletteShift(0);
    if (Rotation == 270)
        G->Scale(-1.f, 1.f);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, -0.1f, toRadians(Rotation), Frame + ((SubType & 0x2) != 0x0) * 3, IE::CenterAlign | IE::MiddleAlign);
    G->Scale(1.f, 1.f);

    if (Scene->ShowHitboxes || Sprite == NULL) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.1f, W, H, IColor(0, 1, 1, 0.5f));
    }
}

int  ISpring::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Data != 1)
        return 0;

    SpringPower = 16;

    int SpringFlips = 0;
    if ((SubType & 0x2) != 0x0) {
        SpringPower = 10;
        SpringFlips = (SubType & 0x1);
    }

    if (Scene->MyPlayer->Underwater)
        SpringFlips = 1 - SpringFlips;

    if (HitFrom == 1 && Rotation == 0) {
        //if (Scene->MyPlayer->AngleMode == 0) {
            Scene->MyPlayer->Gravity = -SpringPower;
            Scene->MyPlayer->Ground = false;
            Scene->MyPlayer->InputAlarm = 16;
            Scene->MyPlayer->Action = ActionType::Spring;
            Scene->MyPlayer->SpringFlip = SpringFlips;
            Scene->MyPlayer->AngleMode = 0;
            Scene->MyPlayer->Angle = 0;
            Scene->MyPlayer->Vibrate(0, 0.25, 150);
            Scene->aud_Spring->Play(1);

            Animation = 0;
            return 1;
        //}
    }
    else if (HitFrom == 3 && Rotation == 180) {
        Scene->MyPlayer->Gravity = SpringPower;
        if (Scene->MyPlayer->AngleMode == 1 || Scene->MyPlayer->AngleMode == 3) {
            Scene->MyPlayer->Flip = -(int)Scene->MyPlayer->Sin[Scene->MyPlayer->AngleMode * 90];
            Scene->MyPlayer->Speed = SpringPower * Scene->MyPlayer->Flip;
        }
        Scene->MyPlayer->Action = ActionType::Spring;
        Scene->MyPlayer->SpringFlip = SpringFlips;
        Scene->MyPlayer->Layer = 1 - ((SubType >> 2) & 0x1);
        Scene->MyPlayer->Vibrate(0, 0.25, 150);
        Scene->aud_Spring->Play(1);

        Animation = 0;
        return 1;
    }
    else if ((Rotation == 90 && HitFrom == 2) || (Rotation == 270 && HitFrom == 0)) {
        if (Scene->MyPlayer->Y + 20 - 4 - 4 <= Y - 16)
            return 0;

        int ToFlip = -(int)Scene->MyPlayer->Cos[(int)wrapAngle(Scene->MyPlayer->AngleMode * 90 - Rotation + 90)];

        Scene->MyPlayer->Flip = ToFlip;
        if (Scene->MyPlayer->AngleMode == 0 || Scene->MyPlayer->AngleMode == 2) {
            Scene->MyPlayer->Speed = SpringPower * Scene->MyPlayer->Flip;
        }
        if (((SubType >> 2) & 0x1) != ((SubType >> 3) & 0x1)) // Turns out, the 3rd bit controls which layer the player is switched to, but only if the 4th bit is the opposite of it.
            Scene->MyPlayer->Layer = 1 - ((SubType >> 2) & 0x1);

        Scene->MyPlayer->Vibrate(0, 0.25, 150);
        Scene->aud_Spring->Play(1);

        Animation = 0;
        return 1;
    }
	return 0;
}
