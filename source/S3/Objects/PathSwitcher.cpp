#include "PathSwitcher.h"
#include <LevelScene.h>

void IPathSwitcher::Create() {
    Size = SubType & 0x03;

    W = 4;
    H = Size * 128;
    Active = true;
    Priority = true;

    if (((SubType >> 2) & 0x01) == 1) {
        W = H;
        H = 4;
    }

    Scene->ObjectsPathSwitcher[Scene->ObjectPathSwitcherCount++] = this;
}
void IPathSwitcher::Update() {

}
void IPathSwitcher::Render(int CamX, int CamY) {
    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -0.1f, W, H, IColor(1, 0, 1, 0.5f));
    }
}
