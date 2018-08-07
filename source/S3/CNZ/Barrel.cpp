#include "Barrel.h"

void IBarrel::Create() {
    W = 64;
    H = 64;
	$X = X;
    $Y = Y;
    Active = true;
    Priority = false;
	Scene->ObjectsSolid[Scene->ObjectSolidCount++] = this;
	Sprite = App->Sprites["Barrel"];
}

void IBarrel::Update() {
    Frame = Scene->frameAnim;
	
	if (this->PlayerID == -1) {
		return;
	}
	
	IPlayer *currentPlayer = Scene->playerBuffer.at(PlayerID);
	//IPlayer *currentPlayer = Scene->MyPlayer;
	
	if (currentPlayer == nullptr) {
		PlayerID = -1;
		currentPlayer = nullptr;
		return;
	}
	
	if (currentPlayer->ObjectControlled == 0) {
		PlayerID = -1;
		currentPlayer = nullptr;
		return;
	}
	
	if ((Data & 0xF0) == 0) { // non auto movement
		Y = $Y + std::cos(toRadians((Frame % 256) * 360 / 256)) * 64;
		//App->print(0, "Barrel: New Y is %d", Y);
	} else if ((Data & 0xF) > 4) {
		Y = $Y + std::cos(toRadians((Frame % 256) * 360 / 256)) * 32 * (FlipX ? -1 : 1);
		//App->print(0, "Barrel: (Flip X) New Y is %d", Y);
	} else {
		X = $X + std::cos(toRadians((Frame % 256) * 360 / 256)) * (Data & 0xF) * 32 * (FlipX ? -1 : 1);
		//App->print(0, "Barrel: New X is %d", X);
	}
	
	int ang = (96 - pR) * 360 / 96;
	currentPlayer->X = X + std::cos(toRadians(ang)) * pX;
	currentPlayer->Y = std::round(Y) - H / 2 - 20;
	currentPlayer->Speed = 0;
	currentPlayer->StoredRotation = float(((int)std::round(((float)ang / 30.f) + (pX > 0 ? 0 : 6 /* always facing outwards */) + 12) % 12));
	currentPlayer = nullptr;
	pR++;
	pR %= 96;
}

void IBarrel::Render(int CamX, int CamY) {
    if (Sprite != NULL) {
		G->PaletteShift(0);
		G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, Frame, IE::CenterAlign | IE::MiddleAlign);
    } else if (Scene->ShowHitboxes || true) {
		G->PaletteShift(-1);
		G->DrawRectangle((int)(std::round(X) - CamX - W / 2.f), (int)(std::round(Y) - CamY - H / 2.f), -3.9f, W, H, IColor(1, 1, 1, 0.5f));
	}
}

int IBarrel::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (!Active) {
        return 0;
	}
	
	IPlayer *currentPlayer = Scene->MyPlayer;
	//App->print(0, "Barrel: Setting Data to %d", Data);
	this->Data = Data;
	currentPlayer->ObjectControlled = 0;
	currentPlayer = nullptr;

	return 1;
}

int IBarrel::OnStandControlled(int PlayerID) {
	if (this->PlayerID == -1) {
		this->PlayerID = PlayerID;
		IPlayer *currentPlayer = Scene->playerBuffer.at(PlayerID);

		pX = currentPlayer->X - X;
		if (std::abs(currentPlayer->X - X) > 32) {
			if (pX < 0) {
				pX = -32;
			} else {
				pX = 32;
			}
		}
		pR = 0;
		currentPlayer = nullptr;
		return 1;
	}
	return 0 ;
}