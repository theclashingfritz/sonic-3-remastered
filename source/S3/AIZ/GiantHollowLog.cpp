#include "GiantHollowLog.h"
#include <LevelScene.h>

void IGiantHollowLog::Create() {
    Active = true;
    Priority = false;

    W = 0xD0;
    H = (0x46C - 0x3CC) << 1;
}
void IGiantHollowLog::Update() {
    if (Scene->MyPlayer->Data1 < 2) return;

    if (Scene->MyPlayer->Data3 > 0) {
        int AIZTree_PlayerFrames[54] = {
    		0x69, 0x6A, 0x6B, 0x77, 0x6C, 0x6C, 0x6D, 0x6D, 0x6E, 0x6E, 0x6F, 0x6F, 0x70, 0x70, 0x71, 0x71, 0x72, 0x72, 0x73, 0x73,
    		0x74, 0x74, 0x75, 0x75, 0x76, 0x76, 0x77, 0x77, 0x6C, 0x6C, 0x6D, 0x6D, 0x6E, 0x6E, 0x6F, 0x6F, 0x70, 0x70, 0x71, 0x71,
    		0x72, 0x72, 0x73, 0x73, 0x74, 0x74, 0x75, 0x75, 0x6B, 0x6B, 0x6A, 0x6A, 0x69, 0x69
        };

        if (FirstRun == -1) {
            int left = 0x2C80 / 128;
            int top = 0x280 / 128;

            //int yyy = (int)((Scene->MyPlayer->Data3 / 810.f) * (4));
            for (int g = 0; g < 4; g++) {
                Scene->level[(Scene->level[8] & 0xFF) * 0x100 + (Scene->level[9] & 0xFF) - 0x8000 + (left + 0) + (top + g) * Scene->levelW] =
                    Scene->level[(Scene->level[8] & 0xFF) * 0x100 + (Scene->level[9] & 0xFF) - 0x8000 + (0) + (g) * Scene->levelW];

                Scene->level[(Scene->level[8] & 0xFF) * 0x100 + (Scene->level[9] & 0xFF) - 0x8000 + (left + 1) + (top + g) * Scene->levelW] =
                    Scene->level[(Scene->level[8] & 0xFF) * 0x100 + (Scene->level[9] & 0xFF) - 0x8000 + (1) + (g) * Scene->levelW];
            }
        }

        if (FirstRun == -1)
            FirstRun = 0;

        Scene->MyPlayer->ObjectControlled = 5;
        if (Scene->MyPlayer->Data3 > 90) {
            Scene->MyPlayer->X = Scene->MyPlayer->Data4 + 0xB0 / 2 * std::sin(toRadians(810 - Scene->MyPlayer->Data3));

            Scene->cameraMinX = Scene->MyPlayer->Data4 - App->renderWidth / 2;
            Scene->cameraMaxX = Scene->MyPlayer->Data4 - App->renderWidth / 2;
        } else {
            Scene->MyPlayer->X = Scene->MyPlayer->Data4 + 0xC0 / 2 * ((90 - Scene->MyPlayer->Data3) / 90.f);
            Scene->cameraMinX = 0;
            Scene->cameraMaxX = Scene->levelW * 128 - App->renderWidth;
        }
        Scene->MyPlayer->Y = Scene->MyPlayer->Data5 - (810 - Scene->MyPlayer->Data3) / 810.0f * (Scene->MyPlayer->Data5 - (0x33A));
        int index = (int)((810 - Scene->MyPlayer->Data3) * 11 / 160.f);

        Scene->MyPlayer->Action = ActionType::Corkscrew;
        Scene->MyPlayer->Frame = 0;
        Scene->MyPlayer->AnimationSpeed = 0;
        Scene->MyPlayer->AnimationStart = 68 + AIZTree_PlayerFrames[index % 54] - 0x69;

        if (Scene->MyPlayer->Data3 > 0)
            Scene->MyPlayer->Data3 -= Scene->MyPlayer->Data2 / 1.75f;
    } else {
        Scene->MyPlayer->ObjectControlled = 0;
        Scene->MyPlayer->Speed = Scene->MyPlayer->Data2 / 2;
        FirstRun = 1;
        Scene->MyPlayer->Action = ActionType::Normal;
        Scene->MyPlayer->Data1 = 0;
    }
}
void IGiantHollowLog::Render(int CamX, int CamY) {
    if (FirstRun == 0) {
        int left = 0x2C80 / 128;
        int top = 0x280 / 128;

        int completionInTiles = std::max((int)((Scene->MyPlayer->Data3 / 810.f) * (14 * 4)) + (16 - 14) * 4, 0);

        int chunkList[8] = {
            0xA8,
            0xAC,
            0xAB,
            0xAE,
            0xA9,
            0xAA,
            0xAD,
            0xAF,
        };

        G->PaletteShift(0);
        Scene->JustInstantDrawAnimTiles = false;

        bool oop = false;

        int cit = completionInTiles % 16;

        if ((completionInTiles - 2) % 16 >= 0)
            oop = true;

        if (cit == 0) { cit = 16; completionInTiles -= 16; }
        if (completionInTiles >= 0) {
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,       0,  0,  6, cit, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,      10,  0,  6, cit, -1.7f);
        }
        if (oop && completionInTiles >= 16) {
            completionInTiles -= 16;
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,       0,  0,  6, 16, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,      10,  0,  6, 16, -1.7f);
            completionInTiles += 16;
        }
        if (cit == 16) { completionInTiles += 16; }

        completionInTiles--;
        cit = completionInTiles % 16;
        if (cit == 0) { cit = 16; completionInTiles -= 16; }
        if (completionInTiles >= 0) {
            if (cit == 0) cit = 16;
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,       6,  0,  4, cit, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,       6,  0,  4, cit, -1.7f);
        }
        if (oop && completionInTiles >= 16) {
            completionInTiles -= 16;
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,       6,  0,  4, 16, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,       6,  0,  4, 16, -1.7f);
            completionInTiles += 16;
        }
        if (cit == 16) { completionInTiles += 16; }

        completionInTiles--;
        cit = completionInTiles % 16;
        if (cit == 0) { cit = 16; completionInTiles -= 16; }
        if (completionInTiles >= 0) {
            if (cit == 0) cit = 16;
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,      10,  0,  6, cit, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,       0,  0,  6, cit, -1.7f);
        }
        if (completionInTiles >= 16) {
            completionInTiles -= 16;
            Scene->DrawChunk(left + 0, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 0], 0, 0,      10,  0,  6, 16, -1.7f);
            Scene->DrawChunk(left + 1, top + completionInTiles / 16, chunkList[completionInTiles / 16 + 4], 0, 0,       0,  0,  6, 16, -1.7f);
            completionInTiles += 16;
        }

        Scene->JustInstantDrawAnimTiles = true;
    }

    if (Scene->ShowHitboxes) {
        G->PaletteShift(-1);
        G->DrawRectangle((int)X - CamX - W / 2, (int)Y - CamY - H / 2, -2.11f, W, H, IColor(1, 1, 1, 0.5f));
    }
}

int  IGiantHollowLog::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    if (Scene->MyPlayer->Data1 != 2) {
        if (Scene->MyPlayer->X >= X && Scene->MyPlayer->Y > Y) {
            if (Scene->MyPlayer->Ground && Scene->MyPlayer->Speed >= 6 && Scene->MyPlayer->ObjectControlled == 0) {
                //pID = PlayerID;
                Scene->MyPlayer->Data1 = 2;
                Scene->MyPlayer->Data2 = Scene->MyPlayer->Speed;
                Scene->MyPlayer->Data3 = 810 - 1;
                Scene->MyPlayer->Data4 = Scene->MyPlayer->X;
                Scene->MyPlayer->Data5 = Scene->MyPlayer->Y;
				Scene->MyPlayer->Ground = 1;
            }
        }
    } else if (Scene->MyPlayer->X < X && Scene->MyPlayer->ObjectControlled == 0) {
            Scene->MyPlayer->Data1 = 1;
    }

	return 1;
}
