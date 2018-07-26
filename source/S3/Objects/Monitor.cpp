#include "Monitor.h"
#include <LevelScene.h>

void IMonitor::Create() {
    W = 26;
    H = 32;
    Active = true;
    Priority = false;

    Frame = 0;
    Gravity = 0;

    Solid = true;
    Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;

    BreakableByRoll = true;
    BreakableByJump = true;
    BreakableByGlide = true;
    Scene->ObjectsBreakable[Scene->ObjectBreakableCount++] = this;
}

void IMonitor::Update() {
    Frame = (Frame + 1) % 12;

    if (!CanFall && Gravity != 0) {
        CanFall = true;
    }
	
    if (CanFall) {
        Gravity += 0.21875f / 1.5f;
        for (float i = 0; i < std::abs(Gravity); i += 0.5f) {
            int angle = 0;
            if (Scene->CollisionAt(X, Y + 16, 0, &angle, true, NULL, true)) {
                Gravity = 0;
                Priority = false;
                break;
            }
            Y += signum(Gravity) * 0.5f;
        }
    }
	
    if (Gravity != 0) {
        Priority = true;
	}

    if (Timer >= 0 && Timer < 32 + 24) {
        Timer++;
    } else if (!CanFall) {
        Priority = false;
	}

    if (Timer == 32) {
        if (SubType == 0x00) {
            Scene->MyPlayer->Hurt(Scene->MyPlayer->X + Scene->MyPlayer->Flip, false);
        } else if (SubType == 0x01) {
            Scene->getLife(1);
        } else if (SubType == 0x02) {
            Scene->MyPlayer->Hurt(Scene->MyPlayer->X + Scene->MyPlayer->Flip, false);
        } else if (SubType == 0x03) {
            Scene->getRing(10);
            Scene->aud_Ring->Play(2);
        } else if (SubType == 0x04) {
            Scene->MyPlayer->SpeedSneakersActive = true;
            Scene->MyPlayer->SpeedSneakersTimer  = 1200;
        } else if (SubType == 0x05) {
            Scene->MyPlayer->Shield = ShieldType::Fire;
            Scene->MyPlayer->ShieldUsable = true;
            Scene->MyPlayer->ShieldAction = false;
            Scene->aud[0x3E]->Play(2);
        } else if (SubType == 0x06) {
            Scene->MyPlayer->Shield = ShieldType::Electric;
            Scene->MyPlayer->ShieldUsable = true;
            Scene->MyPlayer->ShieldAction = false;
            Scene->aud[0x41]->Play(2);
        } else if (SubType == 0x07) {
            Scene->MyPlayer->Shield = ShieldType::Bubble;
            Scene->MyPlayer->ShieldUsable = true;
            Scene->MyPlayer->ShieldAction = false;
            Scene->aud[0x3F]->Play(2);
        } else if (SubType == 0x08) {
            Scene->MyPlayer->Invincibility      = InvincibilityType::Full;
            Scene->MyPlayer->InvincibilityTimer = 1200;
            App->Stack->Push("Invincibility", Scene->aud_BGM_Invincibility, 5, 0, 0, true, false);
        } else if (SubType == 0x09) {
            Scene->MyPlayer->SuperForm = true;
            Scene->getRing(100);
        } else if (SubType == 0x0B) {
            Scene->MyPlayer->HyperRings = true;
            Scene->getRing(10);
            Scene->aud_Ring->Play(2);
        }
    }

    if (!BreakableByRoll) {
        GhostY += (-20 - GhostY) / 10.f;
        Priority = true;
        if (std::floor(GhostY) == -20) {
            GhostY = -20;
        }
    }
}

void IMonitor::Render(int CamX, int CamY) {
    int Thing = SubType + 1;
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, BreakableByRoll ? (Frame / 6) : 11, IE::CenterAlign | IE::MiddleAlign);

    if ((BreakableByRoll && Frame % 6 < 4) || (!BreakableByRoll && (Timer < 32 || Timer % 3 == 0))) {
        if (SubType == 0x0B) {
            G->DrawSprite(Sprite, 8, 4, 16, 14, (int)X - CamX, (int)Y - CamY - 5 + GhostY, -0.1f, 4, IE::CenterAlign | IE::MiddleAlign);
        } else if (SubType != 0x01) {
            G->DrawSprite(Sprite, 8, 4, 16, 14, (int)X - CamX, (int)Y - CamY - 5 + GhostY, -0.1f, Thing, IE::CenterAlign | IE::MiddleAlign);
        } else {
            if (Scene->MyPlayer->Character == CharacterType::Sonic)
                G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - 5 + GhostY, -0.1f, 12, IE::CenterAlign | IE::MiddleAlign);
            else if (Scene->MyPlayer->Character == CharacterType::Tails)
                G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - 5 + GhostY, -0.1f, 13, IE::CenterAlign | IE::MiddleAlign);
            else if (Scene->MyPlayer->Character == CharacterType::Knuckles)
                G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY - 5 + GhostY, -0.1f, 14, IE::CenterAlign | IE::MiddleAlign);
        }
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.1f, W, H, IColor(0, 1, 1, 0.5f));
    }
	
    G->PaletteShift(-1);
}

int IMonitor::OnBreakHorizontal(int PlayerID, int HitFrom) {
    BreakableByRoll = false;
    BreakableByJump = false;
    BreakableByGlide = false;
    Solid = false;

    Timer = 0;
    Priority = true;

    Scene->aud_BadnikDestroyed->Play(3);

	return 1;
}

int IMonitor::OnBreakVertical(int PlayerID, int HitFrom) {
    return OnBreakHorizontal(PlayerID, HitFrom);
}