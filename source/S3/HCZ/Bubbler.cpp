#include "Bubbler.h"
#include <S3/HCZ/AirBubble.h>
#include <LevelScene.h>

void IBubbler::Create() {
    W = 16;
    H = 16;
    Active = true;
    Priority = false;
	Sprite = App->Sprites["Bubbler"];
	
	App->print(3, "Bubbler: Object created in current zone!");
}

void IBubbler::Update() {
	Frame++;
	if (Frame > 23) Frame = 0;

	f++;
	f %= 60 * 6;
	
	if (f == 0) {
		// Release Air Bubbles
		IAirBubble *obj = new IAirBubble();
		obj->X = X; 
		obj->Y = Y;
		obj->G = G;
		obj->ID = ID;
		obj->App = App;
		obj->Scene = Scene;
		Scene->Objects[Scene->ObjectCount++] = obj;
		obj->Create();
		obj->SetBubbler(this);
		
		bubbles.push_back(obj);
	}
	
	for (auto it = bubbles.begin(); it != bubbles.end(); ++it) {
		IAirBubble *bubble = *it;
		if (bubble != NULL && bubble->Active) {
			bubble->Update();
		}
	}
	
    if (Lifespan > 0)
        Lifespan--;

    if (Lifespan == 0)
        Active = false;
}

void IBubbler::RemoveBubble(const IAirBubble *bubble) {
	if (bubble == NULL) {
		return;
	}
	for (auto it = bubbles.begin(); it != bubbles.end(); ++it) {
		IAirBubble *currentBubble = *it;
		if (currentBubble == bubble) {
			bubbles.erase(it);
		}
	}
}

void IBubbler::Render(int CamX, int CamY) {
    if (Sprite != NULL) {
        G->PaletteShift(0);
		G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.0f, Frame / 8, IE::CenterAlign | IE::MiddleAlign);
    } else if (Scene->ShowHitboxes || true) {
		G->PaletteShift(-1);
		G->DrawRectangle((int)(std::round(X) - CamX - W / 2.f), (int)(std::round(Y) - CamY - H / 2.f), -3.9f, W, H, IColor(1, 1, 1, 0.5f));
	}
}