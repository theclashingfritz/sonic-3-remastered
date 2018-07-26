#include "Dummy.h"
#include <LevelScene.h>

void IDummy::Create() {
    Active = true;
    Priority = false;

    W = 32;
    H = 32;

    Sprite = App->Sprites["Ring"];
}
void IDummy::Update() {

}
void IDummy::Render(int CamX, int CamY) {
    G->PaletteShift(0);
    G->DrawSprite(Sprite, (int)X - CamX, (int)Y - CamY, 0.1f, 0, IE::CenterAlign | IE::MiddleAlign);

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IDummy::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {

	return 1;
}
