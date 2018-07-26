#include "LogBridge.H"
#include <LevelScene.H>

void ILogBridge::Create() {
    Active = true;
    Priority = false;

    W = 182;
    H = 32;

    separation = 8;

    for (int i = 0; i < allotted - 32; i++)
        tim[i] = 0.5f * i * i;

    Reset = false;

    SolidTop = true;
    SolidCustomized = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    Sprite = App->Sprites["Collapsing Bridge"];
}
void ILogBridge::Update() {
    if (!OnScreen && !Reset) {
        Reset = true;
        f = -1;
		SolidTop = true;

        for (int i = 0; i < allotted - 32; i++)
            tim[i] = 0.5f * i * i;
    }
    if (OnScreen) {
        Reset = false;
    }

    SolidTop = true;
    if (f > -1 && f <= allotted - 60) {
        SolidTop = false;
        Solid = false;
        if (f == allotted)
            Scene->aud_BreakDeeper->Play(3);
    }
    if (f > 0)
        f--;
}
void ILogBridge::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->Scale(1.0f, 1.0f);
    if (f >= 0 && f <= allotted) {
        for (int i = 0; i < sections; i++) {
            for (int j = 0; j < vsect; j++) {
                if (std::max(allotted - f - (sections - 1 - i - j) * separation, 0) < allotted - 8 * 2 * separation) {
                    G->DrawSprite(Sprite, X - CamX - 76 + 30 * (5 - i), (int)Y - CamY + 8 + (tim[std::max(allotted - f - (sections - 1 - i - j) * separation, 0)] * 0.21875f), 0.09f - i * 0.001f, i != 0 ? 0 : 1, IE::CenterAlign | IE::MiddleAlign);
                }
            }
        }
    }
    else {
        for (int i = 0; i < sections; i++) {
            G->DrawSprite(Sprite, (int)X - CamX - 76 + 30 * (5 - i), (int)Y - CamY + 8, 0.09f - i * 0.001f, i != 0 ? 0 : 1, IE::CenterAlign | IE::MiddleAlign);
        }
    }
    G->Scale(1.0f, 1.0f);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  ILogBridge::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (HitFrom == 1 && Scene->GetPlayer(PlayerID)->Ground) {
        if (f == -1) {
            f = 60 + allotted;
            Priority = true;
        }
    }
	return 1;
}

bool ILogBridge::CustomSolidityCheck(int x, int y, int l, bool chk) {
    int XX = (x - X);
    int YY = (y - Y);
    IPlayer* Player = Scene->GetPlayer(l);

    if (!chk)
        return false;

    if (!SolidTop)
        return false;

    for (int i = 0; i < sections; i++) {
        if (XX >= -W/2 + 30 * (5 - i) &&
            XX <  -W/2 + 30 * (5 - i) + 30 &&
            YY >=  -8 &&
            YY <    8 && Player->Y + Player->H / 2 - 6 <= Y - 8 + 2 && (f == -1 || tim[std::max(allotted - f - (sections - 1 - i) * separation, 0)] * 0.21875f <= 1)) {
            return true;
        }
    }
    return false;
}
