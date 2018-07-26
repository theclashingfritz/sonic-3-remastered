#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_CNZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_AIZ.h"

// Unique
const char LevelID = 0x40;
// Modularized
Scene_CNZ::Scene_CNZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[1 + 2 + 20 + 12] = {
        newFile(res_CNZ_level_lvl, "Resource/Stages/CNZ/level.lvl"),
        // Musics
        newFile(res_CNZ_act_1_ogg, "Resource/Stages/CNZ/Music/Act 1.ogg"),
        newFile(res_CNZ_act_2_ogg, "Resource/Stages/CNZ/Music/Act 2.ogg"),
        // Layouts
        newFile(res_CNZ_layout_1_bin, "Resource/Stages/CNZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_CNZ_layout_1_bin, "Resource/Stages/CNZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_CNZ_layout_2_bin, "Resource/Stages/CNZ/Data (Layout)/2.bin"),
        newFile(res_CNZ_layout_2_bin, "Resource/Stages/CNZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_CNZ_chunks_2_bin, "Resource/Stages/CNZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_CNZ_chunks_1_bin, "Resource/Stages/CNZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_CNZ_chunks_2_bin, "Resource/Stages/CNZ/Data (Chunks)/2.bin"),
        newFile(res_CNZ_chunks_2_bin, "Resource/Stages/CNZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_CNZ_tiles_0_bin, "Resource/Stages/CNZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_CNZ_tiles_1_bin, "Resource/Stages/CNZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_CNZ_tiles_2_bin, "Resource/Stages/CNZ/Data (Tiles)/2.bin"),
        newFile(res_CNZ_tiles_2_bin, "Resource/Stages/CNZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_CNZ_rings_2_bin, "Resource/Stages/CNZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_CNZ_rings_1_bin, "Resource/Stages/CNZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_CNZ_rings_2_bin, "Resource/Stages/CNZ/Data (Rings)/2.bin"),
        newFile(res_CNZ_rings_2_bin, "Resource/Stages/CNZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_CNZ_objects_1_bin, "Resource/Stages/CNZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_CNZ_objects_1_bin, "Resource/Stages/CNZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_CNZ_objects_2_bin, "Resource/Stages/CNZ/Data (Objects)/2.bin"),
        newFile(res_CNZ_objects_2_bin, "Resource/Stages/CNZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_CNZ_PAL1_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_CNZ_PAL1_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_CNZ_PAL1_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"),
        newFile(res_CNZ_PAL1_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"),
        // Palettes (Wet)
        newFile(res_CNZ_PAL1_WET_png, "Resource/Stages/CNZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_CNZ_PAL1_WET_png, "Resource/Stages/CNZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_CNZ_PAL1_WET_png, "Resource/Stages/CNZ/Palettes/PAL1_WET.png"),
        newFile(res_CNZ_PAL1_WET_png, "Resource/Stages/CNZ/Palettes/PAL1_WET.png"),
        // Palettes (Other)
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"), // Mini-boss
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"), // Boss
        newFile(res_HCZ_PALm_WET_png, "Resource/Stages/CNZ/Palettes/PAL1_DRY.png"), // Extra 1 (Mini-boss)
        NULL, // Extra 2
    };

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
// Mandatory
int Scene_CNZ::CustomAnimTile(int ind, int i, int parRow) {
	int tileID = AnimTiles[i * 5 + 0];
	int tileCount = AnimTiles[i * 5 + 1];
	int aniID = AnimTiles[i * 5 + 2];
	int frmCount = AnimTiles[i * 5 + 3];
	int frmSpd = AnimTiles[i * 5 + 4];

	if (ind >= tileID && ind < tileID + tileCount) {
		if (i == 0) {
			if (( (frameAnim / frmSpd) % (frmCount * 2) ) >= frmCount) {
				int offset = 9;
				return 2048 + aniID + (ind - tileID + offset) % tileCount + (tileCount * (frameAnim / frmSpd)) % (tileCount * frmCount);
			}
            else {
                int offset = 0;
				return 2048 + aniID + (ind - tileID + offset) % tileCount + (tileCount * (frameAnim / frmSpd)) % (tileCount * frmCount);
            }
		}
		else if (i == 5) {
			int frm = ((int)cameraX[app->CurrentViewport] / 4);
			if (frmSpd != 0)
				frm = (frameAnim / frmSpd);

			int bgTileColumnCount = 8;
			int bgTileColumnHeight = 4;
			int offset = ( (tileCount * frm) / (tileCount * frmCount) ) % bgTileColumnCount;
			return 2048 + aniID + (ind - tileID + offset * bgTileColumnHeight) % (tileCount) + (tileCount * frm) % (tileCount * frmCount);
		}
	}
    else if (ind >= 708 && ind < 708 + 16) {
        aniID = 144;
        int frm = frameAnim % 64;
        if (frm < 4)
            return 2048 + aniID + ind - 708 + 0;
        else if (frm < 8)
            return 2048 + aniID + ind - 708 + 16;
        else if (frm < 12)
            return 2048 + aniID + ind - 708 + 32;
        else if (frm < 16)
            return 2048 + aniID + ind - 708 + 48;
        else if (frm < 20)
            return 2048 + aniID + ind - 708 + 64;
        else if (frm < 24)
            return 2048 + aniID + ind - 708 + 80;
        else if (frm < 28)
            return 2048 + aniID + ind - 708 + 96;
        else if (frm < 32)
            return 2048 + aniID + ind - 708 + 112;
        else if (frm < 36)
            return 2048 + aniID + ind - 708 + 128;
        else if (frm < 40)
            return 2048 + aniID + ind - 708 + 144;
        else if (frm < 44)
            return 2048 + aniID + ind - 708 + 160;
        else if (frm < 48)
            return 2048 + aniID + ind - 708 + 176;
        else if (frm < 52)
            return 2048 + aniID + ind - 708 + 192;
        else if (frm < 56)
            return 2048 + aniID + ind - 708 + 208;
        else if (frm < 60)
            return 2048 + aniID + ind - 708 + 224;
        else if (frm < 64)
            return 2048 + aniID + ind - 708 + 240;
    }
    else if (ind >= 724 && ind < 724 + 32) {
        aniID = 400;
        int frm = frameAnim % 32;
        if (frm < 4)
            return 2048 + aniID + ind - 724 + 0;
        else if (frm < 8)
            return 2048 + aniID + ind - 724 + 32;
        else if (frm < 12)
            return 2048 + aniID + ind - 724 + 64;
        else if (frm < 16)
            return 2048 + aniID + ind - 724 + 96;
        else if (frm < 20)
            return 2048 + aniID + ind - 724 + 128;
        else if (frm < 24)
            return 2048 + aniID + ind - 724 + 160;
        else if (frm < 28)
            return 2048 + aniID + ind - 724 + 192;
        else if (frm < 32)
            return 2048 + aniID + ind - 724 + 224;
    }
    else if (ind >= 756 && ind < 756 + 16) {
        aniID = 656;
        int frm = frameAnim % 32;
        if (frm < 4)
            return 2048 + aniID + ind - 756 + 0;
        else if (frm < 8)
            return 2048 + aniID + ind - 756 + 16;
        else if (frm < 12)
            return 2048 + aniID + ind - 756 + 32;
        else if (frm < 16)
            return 2048 + aniID + ind - 756 + 48;
        else if (frm < 20)
            return 2048 + aniID + ind - 756 + 64;
        else if (frm < 24)
            return 2048 + aniID + ind - 756 + 80;
        else if (frm < 28)
            return 2048 + aniID + ind - 756 + 96;
        else if (frm < 32)
            return 2048 + aniID + ind - 756 + 112;
    }
    else if (ind >= 772 && ind < 772 + 4) {
        aniID = 784;
        int frm = frameAnim % 24;
        if (frm < 4)
            return 2048 + aniID + ind - 772 + 0;
        else if (frm < 8)
            return 2048 + aniID + ind - 772 + 4;
        else if (frm < 12)
            return 2048 + aniID + ind - 772 + 8;
        else if (frm < 16)
            return 2048 + aniID + ind - 772 + 0;
        else if (frm < 20)
            return 2048 + aniID + ind - 772 + 4;
        else if (frm < 24)
            return 2048 + aniID + ind - 772 + 8;
    }
    else if (ind >= 808 && ind < 808 + 20) {
        aniID = 796;
        int frm = frameAnim % 8;
        if (frm < 2)
            return 2048 + aniID + ind - 808 + 0;
        else if (frm < 4)
            return 2048 + aniID + ind - 808 + 20;
        else if (frm < 6)
            return 2048 + aniID + ind - 808 + 40;
        else if (frm < 8)
            return 2048 + aniID + ind - 808 + 60;
    }
	return ind;
}
// Unique
void Scene_CNZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    Tileset = new ITexture(IResources::Load("Stages/CNZ/Sprites/TILES.png"), false);

    // Parallax Sizes and Auto Scrolls
    if (a >= 1) {
        int PS[] =    { 0x80,        0x30,        0x60,        0xC0,        0x7FFF      };
        float PAS[] = { 0,           0,           0,           0,           0,          };
        float PM[] =  { 0.03125000f, 0.12500000f, 0.25000000f, 0.43750000f, 0.50000000f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }

    // Animated Tiles
    if (a >= 1) {
        int animTilesTmp[7 * 5] = {
        //  Tile ID		# of tiles		AniTile ID		# of frames		# frames to next anim frame (0 for background tile)
        	690,		18,				0,				8,				4,
        	708,		16,				144,			16, 			4,
        	724,		32,				400,			8,				4,
        	756,		16,				656,			8,				4,
        	772,		4,				784,			3,				4,
        	776,		32,				876,			8,				0,
        	808,		20,				796,			4,				4,
        };
        memcpy(AnimTiles, animTilesTmp, sizeof(AnimTiles));
        AnimTilesCount = sizeof(animTilesTmp) / sizeof(*animTilesTmp) / 5;
    }

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_CNZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

	app->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_CNZ::UnloadUniqueTextures(Application* app, int act) {
	if (act == 1) {
		if (tex_ChunksFront != NULL) tex_ChunksFront->Free(); tex_ChunksFront = NULL;
	}
	else {
		if (tex_Bubble != NULL) tex_Bubble->Free(); tex_Bubble = NULL;

		if (tex_ChunksBack != NULL) tex_ChunksBack->Free(); tex_ChunksBack = NULL;
	    if (tex_ChunksFront != NULL) tex_ChunksFront->Free(); tex_ChunksFront = NULL;

		if (tex_Miniboss != NULL) tex_Miniboss->Free(); tex_Miniboss = NULL;
	    if (tex_NonAnim_ != NULL) tex_NonAnim_->Free(); tex_NonAnim_ = NULL;
	    if (tex_Waves != NULL) tex_Waves->Free(); tex_Waves = NULL;
	}
}
void Scene_CNZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if ((act & 0xF) == 2) {
        Scene_AIZ* nextLevel = new Scene_AIZ(App, 1, -1);
        nextLevel->score = score;
        nextLevel->lives = lives;
        App->nextScene = nextLevel;
        return;
    }

    MyPlayer->X -= 0x3000;
	MyPlayer->Y -= 0x0200;

	act = LevelID + (act & 0xF) + 1;

	InitZone(true, 0, -1, true);

	cameraX[App->CurrentViewport] -= 0x3000;
	cameraY[App->CurrentViewport] -= 0x0200;
	cameraMinX = 0;
	cameraMinY = 0;
}

void Scene_CNZ::Update() {
    // ZONE SPECIFIC
    CurrentTilesetAnimated = true;
    BGstart = 0;
    BGsize = 4;
    BGx = 7.f / 16.f;
    BGy = 13.f / 128.f;
    BGcameraLock = false;



    LevelScene::Update();


    // Palette Editing
    int len = 4;
    int pick;
    if ((false)) {
        // Find: \$([A-Z][A-Z]+)
        // Repl:  0x$1
        int ew[] = {
            0xEE8, 0xEE2, 0xEC4, 0xEA4,
            0xEE2, 0xEC4, 0xEA4, 0xEE8,
            0xEC4, 0xEA4, 0xEE8, 0xEE2,
            0xEA4, 0xEE8, 0xEE2, 0xEC4 };

        // Waterfalls
        len = 4;
        pick = (frameAnim / 8) % len;
        setPixel(paletteDry->pixels, 3, 12, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 12, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 1) % len;
        setPixel(paletteDry->pixels, 3, 13, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 13, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 2) % len;
        setPixel(paletteDry->pixels, 3, 14, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 14, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 3) % len;
        setPixel(paletteDry->pixels, 3, 15, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 15, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
	}

    // Do common palette stuffs (Super Sonic)
    if (player->SuperForm || player->SuperFormAnim == SuperFormAnimType::None) {
        // Super Sonic
        len = 4;
        pick = 3 * ((frameAnim / 16) % len + 6);

        if (player->SuperFormAnim == SuperFormAnimType::Transforming) {
            pick = ((4 * 10 - player->SuperFormTimer) / 4);
            if (pick < 2)
                pick = 2;
            pick = 3 * pick;
        }
        else if (player->SuperFormAnim == SuperFormAnimType::None) {
            pick = 3 * (player->SuperFormTimer / 4);
        }
        else if (player->SuperFormTimer == 0) {
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, paletteDry->w,
        paletteDry->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        paletteDry->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, paletteWet->w,
        paletteWet->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
        paletteWet->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Scene_CNZ::Render() {
    LevelScene::Render();
}
void Scene_CNZ::Free() {
    LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
