#include "AirBubble.h"
#include <S3/HCZ/Bubbler.h>
#include <LevelScene.h>

void IAirBubble::Create() {
    W = 16;
    H = 16;
    Active = true;
    Priority = false;
	Sprite = App->Sprites["Bubble"];
	
	hostBubbler = NULL;
	
	App->print(3, "AirBubble: Object created in current zone!");
}

void IAirBubble::Update() {
	Y -= 0.25f;
	Frame += 1 / 16.f;
	if (Frame > 6)
		Frame = 6;
	
    if (Lifespan > 0)
        Lifespan--;

    if (Lifespan == 0)
        Active = false;
}

void IAirBubble::SetBubbler(IBubbler *bubbler) {
	if (bubbler != NULL && bubbler->Active) {
		hostBubbler = bubbler;
	}
}

void IAirBubble::OnLeaveScreen() {
	if (hostBubbler != NULL && !Active) {
		hostBubbler->RemoveBubble(this);
		hostBubbler = NULL;
	}
}

void IAirBubble::Render(int CamX, int CamY) {
    if (Sprite != NULL) {
        G->PaletteShift(0);
		G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, Frame / 8, Frame % 8, IE::CenterAlign | IE::MiddleAlign);
    } else if (Scene->ShowHitboxes || true) {
		G->PaletteShift(-1);
		G->DrawRectangle((int)(std::round(X) - CamX - W / 2.f), (int)(std::round(Y) - CamY - H / 2.f), -3.9f, W, H, IColor(1, 1, 1, 0.5f));
	}
}