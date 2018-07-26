#include "Barrel.h"

void IBarrel::Create() {
    W = 16;
    H = 16;
    Active = true;
    Priority = false;
	Sprite = App->Sprites["Barrel"];
}

void IBarrel::Update() {
    Frame = Scene->frameAnim;

	if ((lastHitFrom & 0xF0) == 0) { // non auto movement
        Y = Scene->playerBuffer[currentPlayerID]->Y + std::cos(toRadians((Scene->frameAnim % 256) * 360 / 256)) * 64;
    }
	
    if (currentPlayerID != -1) {
		if ((lastHitFrom & 0xF) > 4) {
            Y = Scene->playerBuffer[currentPlayerID]->Y + std::cos(toRadians((Frame % 256) * 360 / 256)) * 32 * (FlipX ? -1 : 1);
        } else {
            Y = Scene->playerBuffer[currentPlayerID]->X + std::cos(toRadians((Frame % 256) * 360 / 256)) * (lastHitFrom & 0xF) * 32 * (FlipX ? -1 : 1);
        }
		
        if (Scene->playerBuffer[currentPlayerID]->ObjectControlled == 0) {
            currentPlayerID = -1;
            return;
        }
        int ang = (96 - pR) * 360 / 96;
        Scene->playerBuffer[currentPlayerID]->X = X + std::cos(toRadians(ang)) * pX;
        Scene->playerBuffer[currentPlayerID]->Y = std::round(Y) - H / 2 - 20;
        Scene->playerBuffer[currentPlayerID]->Speed = 0;
        Scene->playerBuffer[currentPlayerID]->StoredRotation = float(((int)std::round(((float)ang / 30.f) + (pX > 0 ? 0 : 6 /* always facing outwards */) + 12) % 12));
        pR++;
        pR %= 96;
    }

    if (NonCollect > 0)
        NonCollect--;

    if (Lifespan > 0)
        Lifespan--;

    if (Lifespan == 0)
        Active = false;

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
	lastHitFrom = HitFrom;
	currentPlayerID = PlayerID;
    return 0;
}