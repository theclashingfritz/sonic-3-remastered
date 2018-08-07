#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_LBZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_AIZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x60;
// Modularized
Scene_LBZ::Scene_LBZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[35] = {
        newFile(res_LBZ_level_lvl, "Resource/Stages/LBZ/level.lvl"),
        // Musics
        newFile(res_LBZ_act_1_ogg, "Resource/Stages/LBZ/Music/Act_1.ogg"),
        newFile(res_LBZ_act_2_ogg, "Resource/Stages/LBZ/Music/Act_2.ogg"),
        // Layouts
        newFile(res_LBZ_layout_1_bin, "Resource/Stages/LBZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_LBZ_layout_1_bin, "Resource/Stages/LBZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_LBZ_layout_2_bin, "Resource/Stages/LBZ/Data (Layout)/2.bin"),
        newFile(res_LBZ_layout_2_bin, "Resource/Stages/LBZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_LBZ_chunks_1_bin, "Resource/Stages/LBZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_LBZ_chunks_1_bin, "Resource/Stages/LBZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_LBZ_chunks_2_bin, "Resource/Stages/LBZ/Data (Chunks)/2.bin"),
        newFile(res_LBZ_chunks_2_bin, "Resource/Stages/LBZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_LBZ_tiles_0_bin, "Resource/Stages/LBZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_LBZ_tiles_1_bin, "Resource/Stages/LBZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_LBZ_tiles_2_bin, "Resource/Stages/LBZ/Data (Tiles)/2.bin"),
        newFile(res_LBZ_tiles_2_bin, "Resource/Stages/LBZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_LBZ_rings_2_bin, "Resource/Stages/LBZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_LBZ_rings_1_bin, "Resource/Stages/LBZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_LBZ_rings_2_bin, "Resource/Stages/LBZ/Data (Rings)/2.bin"),
        newFile(res_LBZ_rings_2_bin, "Resource/Stages/LBZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_LBZ_objects_1_bin, "Resource/Stages/LBZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_LBZ_objects_1_bin, "Resource/Stages/LBZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_LBZ_objects_2_bin, "Resource/Stages/LBZ/Data (Objects)/2.bin"),
        newFile(res_LBZ_objects_2_bin, "Resource/Stages/LBZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_LBZ_PAL1_DRY_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_LBZ_PAL1_DRY_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_LBZ_PAL2_DRY_png, "Resource/Stages/LBZ/Palettes/PAL2_DRY.png"),
        newFile(res_LBZ_PAL2_DRY_png, "Resource/Stages/LBZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_LBZ_PAL1_WET_png, "Resource/Stages/LBZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_LBZ_PAL1_WET_png, "Resource/Stages/LBZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_LBZ_PAL2_WET_png, "Resource/Stages/LBZ/Palettes/PAL2_WET.png"),
        newFile(res_LBZ_PAL2_WET_png, "Resource/Stages/LBZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_LBZ_PALm_DRY_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Mini-boss
        newFile(res_LBZ_PALm_DRY_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Boss
        newFile(res_LBZ_PALm_WET_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Extra 1 (Mini-boss)
        newFile(res_LBZ_PALm_WET_png, "Resource/Stages/LBZ/Palettes/PAL1_DRY.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_LBZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if (ActualAct == 2) {
        Scene_AIZ* nextLevel = new Scene_AIZ(App, 1, -1);
        nextLevel->score = score;
        nextLevel->lives = lives;
        App->nextScene = nextLevel;
        return;
    }

    int ObjectPlayerOffsetX = -0x3A00;
    int ObjectPlayerOffsetY =  0x0000;

	MyPlayer->X += ObjectPlayerOffsetX;
	MyPlayer->Y += ObjectPlayerOffsetY;

	act = LevelID + ActualAct + 1;

	InitZone(true, 0, -1, true);

	cameraX[App->CurrentViewport] += ObjectPlayerOffsetX;
	cameraY[App->CurrentViewport] += ObjectPlayerOffsetY;
	cameraMinX = 0;
	cameraMinY = 0;
}
// Unique
void Scene_LBZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/LBZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/LBZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    if (a <= 1) {
        int PS[] =    {  0x000000D0, 0x00000018, 0x00000008, 0x00000008, 0x00007FFF };
        float PAS[] = {  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
        float PM[] =  {  0.0722656f, 0.1250000f, 0.1328125f, 0.1406250f, 0.1484375f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }
    else {
        int PS[] =    { 0x000000C0, 0x00000040, 0x00000038, 0x00000018, 0x00000028, 0x00000010, 0x00000010, 0x00000010, 0x00000018, 0x00000040, 0x00000020, 0x00000010, 0x00000020, 0x00000070, 0x00000030, 0x000080E0, 0x00000020, 0x000000C0, 0x00000040, 0x00000038, 0x00000018, 0x00000028, 0x00000010, 0x00000010, 0x00000010, 0x00000018, 0x00000040, 0x00000020, 0x00000010, 0x00000020, 0x00000070, 0x00007FFF };
        float PAS[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
        float PM[] =  { 1.0000000f, 0.9863281f, 0.9726562f, 0.9589844f, 0.9453125f, 0.9316406f, 0.9179687f, 0.9042969f, 0.8906250f, 0.8769531f, 0.8632812f, 0.8496094f, 0.8359375f, 0.8222656f, 0.8085937f, 0.7949219f, 0.7812500f, 0.7675781f, 0.7539062f, 0.7402344f, 0.7265625f, 0.7128906f, 0.6992187f, 0.6855469f, 0.6718750f, 0.6582031f, 0.6445312f, 0.6308594f, 0.6171875f, 0.6035156f, 0.5898437f, 0.5761719f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(PAS));
        memcpy(ParallaxSizes, PS, sizeof(PS));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }

    // Animated Tiles
    AnimTilesCount = 0;

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_LBZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("AIZ") = new ITexture(IResources::Load("Stages/AIZ/Sprites/Texture.png"), false);



	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_LBZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_LBZ::Update() {
    if (ActualAct == 1) {
        BGx = 1.f;
        BGy = 0.0625f;
        BGstart = 0;
        BGsize = 12;
    }
    else {
        BGx = 1.f;
        BGy = 0.0625f;
        BGstart = 0;
        BGsize = 4;
    }

	LevelScene::Update();

	// Do palette stuffs
    if (frameAnim % 8 == 0) {
    	int len = 4;
        int pick;

        // Do common palette stuffs (Super Sonic)
        if (MyPlayer->SuperForm || MyPlayer->SuperFormAnim != SuperFormAnimType::None) {
            // Super Sonic
            len = 4;
            pick = 3 * ((frameAnim / 16) % len + 6);

            if (MyPlayer->SuperFormAnim == SuperFormAnimType::Transforming) {
                pick = ((MyPlayer->SuperFormAnimTimerMax - MyPlayer->SuperFormAnimTimer) / 4);
                if (pick < 2)
                    pick = 2;
                pick = 3 * pick;
            }
            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Deforming) {
                pick = 3 * (MyPlayer->SuperFormAnimTimer / 4);
            }
            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Super) {
                pick = 0;
            }

            int *PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterHCZCNZLBZ;
            if (LevelID == 0x10 || LevelID == 0x50)
                PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterAIZICZ;

            setPixel(paletteDry->pixels, 0, 2, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 2, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
            pick++;
            setPixel(paletteDry->pixels, 0, 3, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 3, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
            pick++;
            setPixel(paletteDry->pixels, 0, 4, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 4, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
        }

        glBindTexture(GL_TEXTURE_2D, paletteDry->tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, 0, paletteDry->w, paletteDry->h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            paletteDry->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, 0, paletteWet->w, paletteWet->h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            paletteWet->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

int Scene_LBZ::CustomAnimTile(int ind, int i, int parRow) {
	if (ActualAct == 1) {
        if (ind >= 869 && ind < 869 + 8) {
            int frm = frameAnim % 12;
            if (frm < 3)
                return 2048 + 256 + ind - 869 + 0;
            else if (frm < 6)
                return 2048 + 256 + ind - 869 + 8;
            else if (frm < 9)
                return 2048 + 256 + ind - 869 + 16;
            else if (frm < 12)
                return 2048 + 256 + ind - 869 + 24;
        }
    }
    else {
        if (ind >= 368 && ind < 368 + 5) {
            int frm = frameAnim % 32;
            if (frm < 8)
                return 2048 + 0 + ind - 368 + 0;
            else if (frm < 16)
                return 2048 + 0 + ind - 368 + 5;
            else if (frm < 24)
                return 2048 + 0 + ind - 368 + 10;
            else if (frm < 32)
                return 2048 + 0 + ind - 368 + 15;
        }
        if (ind >= 373 && ind < 373 + 4) {
            int frm = frameAnim % 48;
            if (frm < 8)
                return 2048 + 160 + ind - 373 + 0;
            else if (frm < 16)
                return 2048 + 160 + ind - 373 + 4;
            else if (frm < 24)
                return 2048 + 160 + ind - 373 + 8;
            else if (frm < 32)
                return 2048 + 160 + ind - 373 + 12;
            else if (frm < 40)
                return 2048 + 160 + ind - 373 + 16;
            else if (frm < 48)
                return 2048 + 160 + ind - 373 + 20;
        }
    }
	return ind;
}

// OVERRIDES
void Scene_LBZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;
}
void Scene_LBZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();
    return;
}
void Scene_LBZ::Render() {
	LevelScene::Render();
}
void Scene_LBZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
