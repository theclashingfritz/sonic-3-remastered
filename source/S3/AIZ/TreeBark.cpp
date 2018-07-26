#include "TreeBark.h"
#include <LevelScene.h>

void ITreeBark::Create() {
    Active = true;
    Priority = false;

    W = 16;
    H = 16;
}
void ITreeBark::Update() {

}
void ITreeBark::Render(int CamX, int CamY) {
    G->PaletteShift(2);
    Scene->DrawTile(57, X - 8 - CamX, Y - 8 - CamY, 8, 8, 0.1f, 0, 8);
    Scene->DrawTile(58, X - 8 - CamX, Y - 0 - CamY, 8, 8, 0.1f, 0, 8);
    Scene->DrawTile(59, X - 0 - CamX, Y - 0 - CamY, 8, 8, 0.1f, 0, 8);
    Scene->DrawTile(60, X - 0 - CamX, Y - 8 - CamY, 8, 8, 0.1f, 0, 8);
    G->PaletteShift(-1);
}
