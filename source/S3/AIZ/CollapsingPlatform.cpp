#include "CollapsingPlatform.H"
#include <LevelScene.H>

void ICollapsingPlatform::Create() {
    Active = true;
    Priority = false;

    W = 96;
    H = 64;

    for (int i = 0; i < allotted - 32; i++)
        tim[i] = 0.5f * i * i;

    Reset = false;

    SolidTop = true;
    SolidCustomized = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Sprite = App->Sprites["Collapsing Platform"];

    Frame = 0;
    if ((Scene->act & 0xF) > 1) {
        Frame = 1;
    }
}
void ICollapsingPlatform::Update() {
    if (!OnScreen && !Reset) {
        Reset = true;
        f = -1;
        W = 96;
        H = 64;
		SolidTop = true;

        for (int i = 0; i < allotted - 32; i++)
            tim[i] = 0.5f * i * i;
    }
    if (OnScreen) {
        Reset = false;
    }

    SolidTop = true;
    if (f > -1 && f <= allotted) {
        SolidTop = false;
        Solid = false;
        if (f == allotted)
            Scene->aud_BreakDeeper->Play(3);
    }
    if (f > 0)
        f--;
}
void ICollapsingPlatform::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->Scale(FlipX ? -1.0f : 1.0f, 1.0f);
    if (f >= 0 && f <= allotted) {
        float wd = -W / 2 / sections;
        float hi = -H / 2 / vsect;

        for (int i = 0; i < sections; i++) {
            for (int j = 0; j < vsect; j++) {
                if (std::max(allotted - f - (sections - 1 - i - j) * separation, 0) < allotted - 8 * 2 * separation) {
                    if (!FlipX) {
                        G->DrawSprite(Sprite, i * W / sections, j * H / vsect,
                            W / sections, H / vsect,
                            wd + X - CamX - W/2.0f + W / sections / 2.f + W / sections * i,
                            hi + Y - CamY - H/2.0f + H / vsect / 2.f + H / vsect * j + (tim[std::max(allotted - f - (sections - 1 - i - j) * separation, 0)] * 0.21875f),
                            0.1f, Frame, IE::LeftAlign | IE::TopAlign);
                    }
                    else {
                        G->DrawSprite(Sprite, i * W / sections, j * H / vsect,
                            W / sections, H / vsect,
                            wd + X - CamX + W/2.0f - W / sections / 2.f - W / sections * i + 16,
                            hi + Y - CamY - H/2.0f + H / vsect / 2.f + H / vsect * j + (tim[std::max(allotted - f - (sections - 1 - i - j) * separation, 0)] * 0.21875f),
                            0.1f, Frame, IE::LeftAlign | IE::TopAlign);
                    }
                }
            }
        }
    }
    else {
        G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, Frame, IE::CenterAlign | IE::MiddleAlign);
    }
    G->Scale(1.0f, 1.0f);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  ICollapsingPlatform::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom == 1 && Scene->GetPlayer(PlayerID)->Ground) {
        if (f == -1) {
            f = 60 + allotted;
            Priority = true;
        }
    }
	return 1;
}

bool ICollapsingPlatform::CustomSolidityCheck(int x, int y, int l, bool chk) {
    int XX = (x - X) * (FlipX ? -1 : 1);
    int YY = (y - Y);
    IPlayer* Player = Scene->GetPlayer(l);

    if (!chk)
        return false;

    if (!SolidTop)
        return false;

    if (Frame == 0) {
        if (XX >= -48 &&
            XX <  -16 &&
            YY >= -32 + 2 &&
            YY <   32 && Player->Y + Player->H / 2 - 6 <= Y - 32 + 2) {
            return true;
        }
        if (XX >= -16 &&
            XX <   48 &&
            YY >=  (XX - 112) / 4 + 2 &&
            YY <   32 && Player->Y + Player->H / 2 - 6 <= Y + (XX - 112) / 4 + 2) {
            return true;
        }
    }
    else {
        if (XX >= -48 &&
            XX <   16 &&
            YY >= -24 &&
            YY <   24 && Player->Y + Player->H / 2 - 6 <= Y - 24) {
            return true;
        }
        if (XX >=  16 &&
            XX <   48 &&
            YY >=  (XX - 112) / 4 &&
            YY <   32 && Player->Y + Player->H / 2 - 6 <= Y + (XX - 112) / 4) {
            return true;
        }
    }
    return false;
}
