#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_HCZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "S3/Objects/InvisibleSolid.h"

#include "Scene_CNZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x20;
// Modularized
Scene_HCZ::Scene_HCZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[1 + 2 + 20 + 12] = {
        newFile(res_HCZ_level_lvl, "Resource/Stages/HCZ/level.lvl"),
        // Musics
        newFile(res_HCZ_act_1_ogg, "Resource/Stages/HCZ/Music/Act 1.ogg"),
        newFile(res_HCZ_act_2_ogg, "Resource/Stages/HCZ/Music/Act 2.ogg"),
        // Layouts
        newFile(res_HCZ_layout_1_bin, "Resource/Stages/HCZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_HCZ_layout_1_bin, "Resource/Stages/HCZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_HCZ_layout_2_bin, "Resource/Stages/HCZ/Data (Layout)/2.bin"),
        newFile(res_HCZ_layout_2_bin, "Resource/Stages/HCZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_HCZ_chunks_1_bin, "Resource/Stages/HCZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_HCZ_chunks_1_bin, "Resource/Stages/HCZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_HCZ_chunks_2_bin, "Resource/Stages/HCZ/Data (Chunks)/2.bin"),
        newFile(res_HCZ_chunks_2_bin, "Resource/Stages/HCZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_HCZ_tiles_0_bin, "Resource/Stages/HCZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_HCZ_tiles_1_bin, "Resource/Stages/HCZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_HCZ_tiles_2_bin, "Resource/Stages/HCZ/Data (Tiles)/2.bin"),
        newFile(res_HCZ_tiles_2_bin, "Resource/Stages/HCZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_HCZ_rings_2_bin, "Resource/Stages/HCZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_HCZ_rings_1_bin, "Resource/Stages/HCZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_HCZ_rings_2_bin, "Resource/Stages/HCZ/Data (Rings)/2.bin"),
        newFile(res_HCZ_rings_2_bin, "Resource/Stages/HCZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_HCZ_objects_1_bin, "Resource/Stages/HCZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_HCZ_objects_1_bin, "Resource/Stages/HCZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_HCZ_objects_2_bin, "Resource/Stages/HCZ/Data (Objects)/2.bin"),
        newFile(res_HCZ_objects_2_bin, "Resource/Stages/HCZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_HCZ_PAL1_DRY_png, "Resource/Stages/HCZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_HCZ_PAL1_DRY_png, "Resource/Stages/HCZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_HCZ_PAL2_DRY_png, "Resource/Stages/HCZ/Palettes/PAL2_DRY.png"),
        newFile(res_HCZ_PAL2_DRY_png, "Resource/Stages/HCZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_HCZ_PAL1_WET_png, "Resource/Stages/HCZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_HCZ_PAL1_WET_png, "Resource/Stages/HCZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_HCZ_PAL2_WET_png, "Resource/Stages/HCZ/Palettes/PAL2_WET.png"),
        newFile(res_HCZ_PAL2_WET_png, "Resource/Stages/HCZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/HCZ/Palettes/PAL_MB_DRY.png"), // Mini-boss
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/HCZ/Palettes/PAL_MB_DRY.png"), // Boss
        newFile(res_HCZ_PALm_WET_png, "Resource/Stages/HCZ/Palettes/PAL_MB_WET.png"), // Extra 1 (Mini-boss)
        newFile(res_HCZ_PALm_WET_png, "Resource/Stages/HCZ/Palettes/PAL_MB_WET.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_HCZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if ((act & 0xF) == 2) {
        Scene_CNZ* nextLevel = new Scene_CNZ(App, 1, -1);
        nextLevel->score = score;
        nextLevel->lives = lives;
        App->nextScene = nextLevel;
        return;
    }

	MyPlayer->X -= 0x3600;
	MyPlayer->Y  = 0x0830;

	act = LevelID + (act & 0xF) + 1;

	InitZone(true, 0, -1, true);

	cameraX[App->CurrentViewport] -= 0x3600;
	cameraY[App->CurrentViewport]  = 0x0830 - App->gameHeight / 2;
	cameraMinX = 0;
	cameraMinY = 0;
}
// Unique
void Scene_HCZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/HCZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/HCZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    if (a <= 1) {
        int PS[] =    {  0x0040,   0x0008,   0x0008,   0x0005,   0x0005,   0x0006,   0x00F0,   0x0006,   0x0005,   0x0005,   0x0008,   0x0008,   0x0030,   0x80C0,   0x7FFF };
        float PAS[] = {  0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,     };
        float PM[] =  {  0.250f, 0.21875f,  0.1875f, 0.15625f,   0.125f, 0.09375f,  0.0625f, 0.09375f,   0.125f, 0.15625f,  0.1875f, 0.21875f,   0.250f,   0.250f,   0.125f };

        // $FFFFA800 - BG deformation RAM address
        // d0 = 1 / 4
        // d1 = 1 / 32

        // 0x00: 1 / 4
        // 0x02: 1 / 4 - 1 / 32
        // 0x04: 1 / 4 - 1 / 32 - 1 / 32
        // 0x06: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x08: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0C: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0E: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x10: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x12: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x14: 1 / 4 - 1 / 32 - 1 / 32
        // 0x16: 1 / 4 - 1 / 32
        // 0x18: 1 / 4
        //0x19A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // d3 = 1 / 4

        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }
    else {
        int PS[] =    { 0x0008, 0x0008, 0x0090, 0x0010, 0x0008, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00B0, 0x0010, 0x0008, 0x7FFF };
        float PAS[] = { 0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,     };
        float PM[] =  { 0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(PAS));
        memcpy(ParallaxSizes, PS, sizeof(PS));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }

    // Animated Tiles
    if (a == 1) {
		int animTilesTmp[] = {
		//  Tile ID		# of tiles		AniTile ID		# of frames		# frames to next anim frame (0 for background tile)
			277,		6,				0,				9,				3,

            732,		24,				186,			1, 			    2,
            756,		24,				162,		    1, 			    2,

			780,		36,				54,			    3, 			    4,
		};
        memcpy(AnimTiles, animTilesTmp, sizeof(AnimTiles));
        AnimTilesCount = sizeof(animTilesTmp) / sizeof(*animTilesTmp) / 5;
    }
    else {
        int animTilesTmp[] = {
		//  Tile ID		# of tiles		AniTile ID		# of frames		# frames to next anim frame (0 for background tile)
			277,		6,				0,				9,				3,

            606,		21,				54,			    4, 			    4,
            722,		4,				138,		    8, 			    0,
            726,		8,				170,		    8, 			    0,
            734,		16,				234,		    8, 			    0,
            750,		48,				362,		    8, 			    0,
		};
        memcpy(AnimTiles, animTilesTmp, sizeof(AnimTiles));
        AnimTilesCount = sizeof(animTilesTmp) / sizeof(*animTilesTmp) / 5;
    }

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);

	// Create Object: Act 2 wall
	if ((act & 0xF) == 2) {
		IInvisibleSolid* obj = new IInvisibleSolid();
		obj->X = 0x400 + 2 * 128;
		obj->Y = 0x500 + 2 * 128;
		obj->W = 128 * 4;
		obj->H = 128 * 4;
		//obj->data1 = 0x28;

		obj->Scene = this;
		obj->App = app;
		Objects[objects_count] = obj;
		giantWallID = objects_count;
		objects_count++;
		giantWallX = 0;
		obj->Create();
	}

    if (check <= 0) {
        MyPlayer->Action = ActionType::Peril;
    }
}
void Scene_HCZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("HCZ") = new ITexture(IResources::Load("Stages/HCZ/Sprites/Texture.png"), false);

    S("Blastoid") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Blastoid.spr"), T("HCZ"));
    S("Block") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Block.spr"), T("HCZ"));
    S("Boss Effects") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Boss Effects.spr"), T("HCZ"));
    S("Boss Extra") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Boss Extra.spr"), T("HCZ"));
    S("Breakable Bar") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Breakable Bar.spr"), T("HCZ"));
    S("Breakable Platforms") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Breakable Platforms.spr"), T("HCZ"));
    S("Breakable Wall") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Breakable Wall.spr"), T("HCZ"));
    S("Bridge") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Bridge.spr"), T("HCZ"));
    S("Bubbler") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Bubbler.spr"), T("HCZ"));
    S("Buggernaut") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Buggernaut.spr"), T("HCZ"));
    S("Button") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Button.spr"), T("HCZ"));
    S("Fan") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Fan.spr"), T("HCZ"));
    S("Floating Platform") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Floating Platform.spr"), T("HCZ"));
    S("Geyser Particles") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Geyser Particles.spr"), T("HCZ"));
    S("Geyser") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Geyser.spr"), T("HCZ"));
    S("Hand Launcher") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Hand Launcher.spr"), T("HCZ"));
    S("Jawz") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Jawz.spr"), T("HCZ"));
    S("Large Fan") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Large Fan.spr"), T("HCZ"));
    S("Mega Chopper") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Mega Chopper.spr"), T("HCZ"));
    S("Miniboss Pole") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Miniboss Pole.spr"), T("HCZ"));
    S("Miniboss") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Miniboss.spr"), T("HCZ"));
    S("Non Anim 1") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Non Anim 1.spr"), T("HCZ"));
    S("Non Anim 2") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Non Anim 2.spr"), T("HCZ"));
    S("Pointdexter") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Pointdexter.spr"), T("HCZ"));
    S("Snake Block") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Snake Block.spr"), T("HCZ"));
    S("Spinning Column") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Spinning Column.spr"), T("HCZ"));
    S("Turbo Spiker") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Turbo Spiker.spr"), T("HCZ"));
    S("Water Drop") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Water Drop.spr"), T("HCZ"));
    S("Water Rush") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Water Rush.spr"), T("HCZ"));
    S("Water Splash") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Water Splash.spr"), T("HCZ"));
    S("Waterline Top") = new ISprite(IResources::Load("Stages/HCZ/Sprites/Waterline Top.spr"), T("HCZ"));

	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_HCZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_HCZ::Update() {
    BGx = 1.f;
    if ((act & 0xF) == 1) {
        BGstart = 0;
        BGsize = 4;
        BGy = (backHorizonChunkHeight - 1.75f / 2.f) / (frontHorizonChunkHeight - 1.75f / 2.f);
    }
    else {
        BGstart = 0;
        BGsize = 4;
        BGy = 1.f / 4.f;
    }

	LevelScene::Update();

    HCZ2_UpdateMovingWall();

	// Do palette stuffs
    if (frameAnim % 8 == 0) {
    	int len = 4;
        int pick;
    	if ((act & 0xF) == 1) {
    		int ew[] = { 0xEC8, 0xEC0, 0xEA0, 0xE80 };

            // Waterfalls
            len = 4;
            pick = (frameAnim / 8) % len;
            setPixel(paletteDry->pixels, 2, 3, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 2, 3, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

            pick = (frameAnim / 8 + 1) % len;
            setPixel(paletteDry->pixels, 2, 4, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 2, 4, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

            pick = (frameAnim / 8 + 2) % len;
            setPixel(paletteDry->pixels, 2, 5, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 2, 5, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

            pick = (frameAnim / 8 + 3) % len;
            setPixel(paletteDry->pixels, 2, 6, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 2, 6, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
    	}
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

	// WaterLevel updating
	if ((act & 0xF) == 1) {
		if (MyPlayer->X < 0x9A0) {
			waterLevel = 0x500;
			visualWaterHeight = 0x780;
		}
		else if (MyPlayer->X < 0x35B0) {
			waterLevel = 0x680;
			visualWaterHeight = 0x680;
		}
		else {
			waterLevel = 0x6A0;
			visualWaterHeight = 0x6A0;
		}
	}
	else {
        if (MyPlayer->X > 0xC00) {
    		waterLevel = 0x700;
    		visualWaterHeight = 0x700;
        }
	}

    #if 0

	// HCZ_WaterTunnels
	WITH_ALL(
		if (player->Action == Actions::Dead)
			continue;

        if ((act & 0xF) == 2 && player->Ground && player->Layer == 1) {
			if (player->x >= 0x2CC0 && player->x <= 0x3100) {
                if (player->y >= 0x0000 && player->y <= 0x0500) {
                    player->Action = Actions::Slide;
                }
            }
			else if (player->x >= 0x1240 && player->x <= 0x1500) {
                if (player->y >= 0x0000 && player->y <= 0x0460) {
                    player->Action = Actions::Slide;
                }
            }
        }

		bool piped = false;
		if ((act & 0xF) == 1) {
			int start = 1;

			Objectx37* obj = (Objectx37*)objects[9];
			if (!obj->solid) {
				start = 0;
			}

			for (int i = start; i < 16 && !piped; i++) {
				int h = 0;
				if (i == 0)
					h = 16;
				if (player->x >= waterTunnels[i * 7 + 0] &&
					player->y - h >= waterTunnels[i * 7 + 1] &&
					player->x <= waterTunnels[i * 7 + 2] &&
					player->y + h <= waterTunnels[i * 7 + 3]) {

                        int XVel = waterTunnels[i * 7 + 4];
                        int YVel = waterTunnels[i * 7 + 5];
                        if ((XVel & 0x8000) != 0) {
                            XVel = 0x8000 - (XVel & 0x7FFF);
                        }
                        if ((YVel & 0x8000) != 0) {
                            YVel = 0x8000 - (YVel & 0x7FFF);
                        }
                        YVel = -YVel;

	                    if (waterTunnels[i * 7 + 6] != 0x200 && player->Action != Actions::Hurt &&
	    					player->Action != Actions::InStreamGrab &&
	    					player->Action != Actions::InStream &&
	    					player->Action != Actions::InStreamPipe) {
	    					player->Action = Actions::InStream;
	    					player->Speed = XVel / 128.f;
	    					player->Gravity += YVel / 128.f;
	    				}
	    				if (waterTunnels[i * 7 + 6] == 0x200 && player->Action != Actions::Hurt && (player->Action == Actions::InStream || player->Action == Actions::InStreamPipe)) {
	    					player->Action = Actions::InStreamPipe;

	    					player->Speed = XVel / 128.f;
	    					player->Gravity += YVel / 128.f;
	    				}

	    				if (player->Action != Actions::InStreamGrab && player->Action != Actions::Hurt && (player->Action == Actions::InStream || player->Action == Actions::InStreamPipe)) {
	    					player->Speed = XVel / 128.f;
	    					player->Gravity += YVel / 128.f;
	    				}

					piped = true;
				}
			}
		}
		else {
			for (int i = 16; i < 18 && !piped; i++) {
				if (player->x >= waterTunnels[i * 7 + 0] &&
					player->y >= waterTunnels[i * 7 + 1] &&
					player->x <= waterTunnels[i * 7 + 2] &&
					player->y <= waterTunnels[i * 7 + 3]) {

					if (waterTunnels[i * 7 + 6] != 0x200 && player->Action != Actions::Hurt &&
						player->Action != Actions::InStreamGrab &&
						player->Action != Actions::InStream &&
						player->Action != Actions::InStreamPipe) {
						player->Action = Actions::InStream;
						player->Speed = waterTunnels[i * 7 + 4] / 128.f;
						player->Gravity = waterTunnels[i * 7 + 5] / 128.f;
					}
					if (waterTunnels[i * 7 + 6] == 0x200 && player->Action != Actions::Hurt && (player->Action == Actions::InStream || player->Action == Actions::InStreamPipe)) {
						player->Action = Actions::InStreamPipe;

						player->Speed = waterTunnels[i * 7 + 4] / 128.f;
						player->Gravity = waterTunnels[i * 7 + 5] / 128.f;
					}

					if (player->Action != Actions::InStreamGrab && player->Action != Actions::Hurt && (player->Action == Actions::InStream || player->Action == Actions::InStreamPipe)) {
						player->Speed = waterTunnels[i * 7 + 4] / 128.f;
						player->Gravity = waterTunnels[i * 7 + 5] / 128.f;
					}

					piped = true;
				}
			}
		}
		if (!piped &&
			(player->Action == Actions::InStreamGrab ||
			 player->Action == Actions::InStream ||
			 player->Action == Actions::InStreamPipe)) {
			player->Action = Actions::Peril;
			player->Speed /= 2;
			player->JumpVariable = 1;
		}
	);
    #endif
}

int Scene_HCZ::CustomAnimTile(int ind, int i, int parRow) {
	int tileID = AnimTiles[i * 5 + 0];
	int tileCount = AnimTiles[i * 5 + 1];
	int aniID = AnimTiles[i * 5 + 2];
	int frmCount = AnimTiles[i * 5 + 3];
	int frmSpd = AnimTiles[i * 5 + 4];

    if ((act & 0xF) == 1) {
        if (ind >= 277 && ind < 277 + 6) {
            int frm = frameAnim % 48;
            if (frm < 5)
                return 2048 + aniID + ind - 277 + 0;
            else if (frm < 9)
                return 2048 + aniID + ind - 277 + 6;
            else if (frm < 12)
                return 2048 + aniID + ind - 277 + 12;
            else if (frm < 14)
                return 2048 + aniID + ind - 277 + 18;
            else if (frm < 15)
                return 2048 + aniID + ind - 277 + 24;
            else if (frm < 17)
                return 2048 + aniID + ind - 277 + 30;
            else if (frm < 20)
                return 2048 + aniID + ind - 277 + 36;
            else if (frm < 24)
                return 2048 + aniID + ind - 277 + 42;
            else if (frm < 29)
                return 2048 + aniID + ind - 277 + 48;
            else if (frm < 33)
                return 2048 + aniID + ind - 277 + 42;
            else if (frm < 36)
                return 2048 + aniID + ind - 277 + 36;
            else if (frm < 38)
                return 2048 + aniID + ind - 277 + 30;
            else if (frm < 39)
                return 2048 + aniID + ind - 277 + 24;
            else if (frm < 41)
                return 2048 + aniID + ind - 277 + 18;
            else if (frm < 44)
                return 2048 + aniID + ind - 277 + 12;
            else if (frm < 48)
                return 2048 + aniID + ind - 277 + 6;
        }
        else if (ind >= 732 && ind < 732 + 24) {
            return 2048 + 186 + ind - 732;
        }
        else if (ind >= 756 && ind < 756 + 24) {
            return 2048 + 162 + ind - 756;
        }
        else if (ind >= 780 && ind < 780 + 36) {
            int frm = frameAnim % 12;
            return 2048 + 54 + ind - 780 + (frm / 4) * 36;
        }
    }
    else {
        if (ind >= 277 && ind < 277 + 6) {
            i = 0;
            aniID = AnimTiles[i * 5 + 2];
            int frm = frameAnim % 48;
            if (frm < 5)
                return 2048 + aniID + ind - 277 + 0;
            else if (frm < 9)
                return 2048 + aniID + ind - 277 + 6;
            else if (frm < 12)
                return 2048 + aniID + ind - 277 + 12;
            else if (frm < 14)
                return 2048 + aniID + ind - 277 + 18;
            else if (frm < 15)
                return 2048 + aniID + ind - 277 + 24;
            else if (frm < 17)
                return 2048 + aniID + ind - 277 + 30;
            else if (frm < 20)
                return 2048 + aniID + ind - 277 + 36;
            else if (frm < 24)
                return 2048 + aniID + ind - 277 + 42;
            else if (frm < 29)
                return 2048 + aniID + ind - 277 + 48;
            else if (frm < 33)
                return 2048 + aniID + ind - 277 + 42;
            else if (frm < 36)
                return 2048 + aniID + ind - 277 + 36;
            else if (frm < 38)
                return 2048 + aniID + ind - 277 + 30;
            else if (frm < 39)
                return 2048 + aniID + ind - 277 + 24;
            else if (frm < 41)
                return 2048 + aniID + ind - 277 + 18;
            else if (frm < 44)
                return 2048 + aniID + ind - 277 + 12;
            else if (frm < 48)
                return 2048 + aniID + ind - 277 + 6;
        }
        else if (ind >= 606 && ind < 606 + 21) {
            i = 1;
            int frm = frameAnim % 16;
            return 2048 + 54 + ind - 606 + (frm / 4) * 21;
        }
        else if (ind >= 722 && ind < 722 + 4)
            i = 2;
        else if (ind >= 726 && ind < 726 + 8)
            i = 3;
        else if (ind >= 734 && ind < 734 + 16)
            i = 4;
        else if (ind >= 750 && ind < 750 + 48)
            i = 5;

        if (i >= 2 && i <= 5) {
            int frm = (int)cameraX[App->CurrentViewport];
            if (frmSpd != 0)
                frm = (frameAnim / frmSpd);

            int bgTileColumnCount = 4;
            int bgTileColumnHeight = 1;

            if (i == 2) {
                //float cX = cameraX[App->CurrentViewport] * (ParallaxMult[parRow]); // Multiply Camera X by Parallax Row Multiplier to Freeze Anim Tile X
                float cX = cameraX[App->CurrentViewport] * (ParallaxMult[parRow] / 4.f);
                frm = bgTileColumnCount * 8 - (int)cX % (bgTileColumnCount * 8);
            }
            else if (i == 3) {
                bgTileColumnCount = 4;
                bgTileColumnHeight = 2;

                float cX = cameraX[App->CurrentViewport] * (ParallaxMult[parRow] - ParallaxMult[parRow + 1]);
                frm = bgTileColumnCount * 8 - (int)cX % (bgTileColumnCount * 8);
            }
            else if (i == 4) {
                bgTileColumnCount = 4;
                bgTileColumnHeight = 4;

                float cX = cameraX[App->CurrentViewport] * (ParallaxMult[parRow] / 2.f);
                frm = bgTileColumnCount * 8 - (int)cX % (bgTileColumnCount * 8);
            }
            else if (i == 5) {
                bgTileColumnCount = 8;
                bgTileColumnHeight = 6;

                float cX = cameraX[App->CurrentViewport] * (ParallaxMult[parRow] / 2.f);
                frm = bgTileColumnCount * 8 - (int)cX % (bgTileColumnCount * 8);
                frm = 7 - (frm % 8) + (frm / 8) * 8;
            }

            int offset = ( (tileCount * frm) / (tileCount * frmCount) ) % bgTileColumnCount;
            return 2048 + aniID + (ind - tileID + offset * bgTileColumnHeight) % (tileCount) + (tileCount * frm) % (tileCount * frmCount);
        }
    }
    /*
    Tile ID		# of tiles		AniTile ID		# of frames		# frames to next anim frame (0 for background tile)
    277,		6,				0,				9,				3,

    606,		21,				54,			    4, 			    4,
    722,		4,				138,		    8, 			    0,
    726,		8,				170,		    8, 			    0,
    734,		16,				234,		    8, 			    0,
    750,		48,				362,		    8, 			    0,
    //*/
	return ind;
}

// UNIQUE
void Scene_HCZ::HCZ1_RenderWaterline() {
	//App->ChangeShader(App->shader_basic);
	if ((act & 0xF) != 1) {
		//App->ChangeShader(App->shader_test);
		return;
	}
	// HCZ Waterline Rendering
	int pieces = 12;
	float scaleY = (frontHorizonChunkHeight - 1.75f / 2.f) / (backHorizonChunkHeight - 1.75f / 2.f);
	int pos_BG = (int)(4 * 128.f - cameraY[App->CurrentViewport] / scaleY);
	int pos_FG = (int)(13 * 128.f - cameraY[App->CurrentViewport]);
	for (int y = 0; y < pieces; y++) {
		//float uhhX = (x * 8.f + (frame * .001f) * 13.f - 13.f - cameraX[App->CurrentViewport] / (4 + y / 3.f)) + (int)(cameraX[App->CurrentViewport] / (4 + y / 3.f) / (levelSubW * 128)) * (levelSubW * 128);
		//if (uhhX >= -8 && uhhX < App->gameWidth + 8) {
		//float pos_BG = 4 * 128.f - cameraY[App->CurrentViewport] / ((13.f - 1.75f / 2.f) / (4.f - 1.75f / 2.f));
		//float pos_FG = 13 * 128.f - cameraY[App->CurrentViewport];
		//float scalar = (pos_FG - pos_BG) / 12.f;
		//App->drawTextureTop(tex_WaterlineTop, 0/*(x * 8.f - cameraX[App->CurrentViewport] / (4 + y / 3.f)) + (int)(cameraX[App->CurrentViewport] / (4 + y / 3.f) / (levelSubW * 128)) * (levelSubW * 128)*/, slope(pos_BG, pos_FG, y / 12.f), App->gameWidth, scalar, 0.f, y * 1.f / 12.f, 1.f, 1.f / 12.f);


		int myX = 0;
		if (pos_FG - pos_BG > 0)
			myX = (int)((0 * 8.f - cameraX[App->CurrentViewport] / (4 - 3.f * y / pieces)));
		else
			myX = (int)((0 * 8.f - cameraX[App->CurrentViewport] / (4 - 3.f * (pieces - y) / pieces)));

		myX += (int)(std::sin((frameAnim % 120) / 120.f * Math_PI / 180.f) * 3);

		myX %= App->gameWidth;

		int myY = 0;
		if (pos_FG - pos_BG > 0)
			myY = slope(pos_BG, pos_BG + std::min(std::abs(pos_FG - pos_BG), 96), y / (float)pieces);
		else
			myY = slope(pos_BG - std::min(std::abs(pos_BG - pos_FG), 96), pos_BG, y / (float)pieces);

		int myH = (int)std::ceil(std::min(std::abs(pos_FG - pos_BG), 96) / (float)pieces);

		if (myY + myH >= 0 && myY < App->gameHeight) {
			if (pos_FG - pos_BG > 0) {
				App->drawSpriteOPT(
									myX,
									myY,

									App->gameWidth,
									myH,

									App->drawZ,

									0.f, y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
				if (myX > 0)
					App->drawSpriteOPT(
										myX - App->gameWidth,
										myY,

										App->gameWidth,
										myH,

										App->drawZ,

										0.f, y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
				else
					App->drawSpriteOPT(
										myX + App->gameWidth,
										myY,

										App->gameWidth,
										myH,

										App->drawZ,

										0.f, y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
			}
			else if (pos_FG - pos_BG < 0) {
				App->drawSpriteOPT(
									myX,
									myY,

									App->gameWidth,
									myH + 1,

									App->drawZ,

									0.f, 0.5f + y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
				if (myX > 0)
					App->drawSpriteOPT(
										myX - App->gameWidth,
										myY,

										App->gameWidth,
										myH + 1,

										App->drawZ,

										0.f, 0.5f + y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
				else
					App->drawSpriteOPT(
										myX + App->gameWidth,
										myY,

										App->gameWidth,
										myH + 1,

										App->drawZ,

										0.f, 0.5f + y * 0.5f / pieces, 1.f, 0.5f / pieces, 1);
			}
		}
	}

	App->renderSpriteOPT(tex_WaterlineTop->tex, 1);

	//App->ChangeShader(App->shader_test);
}
void Scene_HCZ::HCZ2_UpdateMovingWall() {
	if (!App->realPaused && giantWallID >= 0) {
		if (giantWallX == 0) {
			objects[giantWallID]->x = 0x400 + 2 * 128;
			objects[giantWallID]->y = 0x600 + 2 * 128;
			if (player->X > 0x680 && player->Y > 0x600)
				giantWallX++;
		}
		if (giantWallX > 0 && giantWallX < 0x600) {
			giantWallX += (float)0xE0 / 0x100;
            if ((int)(0x500 + giantWallX) + 0x100 < cameraX[App->CurrentViewport]) {
                giantWallX += (float)(0x140 - 0xE0) / 0x100;  // 140 / 100
            }
            objects[giantWallID]->x = (int)(0x500 + giantWallX);

            if (frameAnim % 16 == 0) {
                aud[0x6F]->Play(4);
            }
		}

		if (giantWallX > 0 && objects[giantWallID]->x < 0xB00) {
			objects[giantWallID]->data2 = 0xFF;
			objects[giantWallID]->prior = true;

			cameraY[App->CurrentViewport] += (float)((frameAnim / 4) % 2);
		}
		else {
			objects[giantWallID]->data2 = 0;
			objects[giantWallID]->prior = false;
		}

        bool allPast = true;
        for (int i = 0; i < player_count; i++) {
            allPast &= cameraX[App->CurrentViewport] > 0xC00;
        }
        if (allPast) {
            giantWallX = 0x600;
            objects[giantWallID]->x = (int)(0x500 + giantWallX);
        }
	}
}
void Scene_HCZ::HCZ2_DrawMovingWall() {
	for (int x = 0; x < 4; x++) {
		for (int y = 2; y < 6; y++) {
			int us = level[(level[y * 4 + 10] & 0xFF) * 0x100 + (level[y * 4 + 11] & 0xFF) - 0x8000 + (x + 4)] & 0xFF;
            DrawChunk(0, 0, us, objects[giantWallID]->x - 2 * 128 + x * 128, 0x500 + y * 128, 16, 16, -0.4f);
		}
	}
	App->renderSpriteOPT(tex_ChunksFront->tex, 46);
}

// OVERRIDES
void Scene_HCZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;

	if ((act & 0xF) == 1) {
		if (MyPlayer->X > 0x35B0) {
			cameraMinX = cameraX[App->CurrentViewport];

			cameraMaxX = (float)(levelW * 128 - 24 - App->gameWidth + (App->gameWidth - App->renderWidth) / 2);

            if (cameraY[App->CurrentViewport] > 0x400) {
                cameraMinY = cameraY[App->CurrentViewport];
    			cameraMaxY = (float)(0x0718 - App->gameHeight);
            }
            else {
                cameraMinY = 0x300;
    			cameraMaxY = (float)(0x0718 - App->gameHeight);
            }
		}
	}
	else {
		cameraMaxX = (float)(levelW * 128 - App->renderWidth);
		cameraMaxY = (float)(levelH * 128 - App->gameHeight);
	}
}
void Scene_HCZ::DoPaletteWaterStuff() {
    LevelScene::DoPaletteWaterStuff();
    return;

	if ((act & 0xF) == 1) {
		// Fix pallette at Beginning of HCZ1 water
		glUniform4f(App->locData, 1, 0, 0, 0);
	}
	else {
		glUniform4f(App->locData, 0, 0, 0, 0);
	}
}
void Scene_HCZ::DoBackgroundStuff() {
    return;
	LevelScene::DoBackgroundStuff();

	// Background and Parallax Rendering
	if ((act & 0xF) == 2)
		HCZ2_DrawMovingWall();
	App->drawZ = 0.6f;

	// HCZ Waterline Rendering
	HCZ1_RenderWaterline();
	App->drawZ = 0.4f;
}
void Scene_HCZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();

    return;

	int waveOff = (int)std::sin(frameAnim / 360.0f * Math_PI) * 10;
	cameraX[App->CurrentViewport] += waveOff;
	int waveFr = (frameAnim / 10) % 3;
	if ((act & 0xF) == 1) {
		for (unsigned int o = 0; o < 3; o++) {
			App->drawSpriteOPT(
				256 * o + ((int)(cameraX[App->CurrentViewport]) / 256) * 256 + (0) * 32 - cameraX[App->CurrentViewport], waterLevel - 16 - cameraY[App->CurrentViewport],
				256, 24,

				App->drawZ,

				waveFr / 3.0f, 0, 1 / 3.0f, 1,

				2);
			App->drawSpriteOPT(
				256 * o + ((int)(cameraX[App->CurrentViewport]) / 256) * 256 + (1) * 32 - cameraX[App->CurrentViewport], waterLevel - 16 - cameraY[App->CurrentViewport],
				256, 24,

				App->drawZ,

				waveFr / 3.0f, 0, 1 / 3.0f, 1,

				2);
		}
	}
	else {
		for (unsigned int o = 0; o < 3; o++) {
			App->drawSpriteOPT(
				256 * o + ((int)(cameraX[App->CurrentViewport]) / 256) * 256 + (0) * 32 - cameraX[App->CurrentViewport], visualWaterHeight - 16 - cameraY[App->CurrentViewport],
				256, 24,

				App->drawZ,

				waveFr / 3.0f, 0, 1 / 3.0f, 1,

				2);
			App->drawSpriteOPT(
				256 * o + ((int)(cameraX[App->CurrentViewport]) / 256) * 256 + (1) * 32 - cameraX[App->CurrentViewport], visualWaterHeight - 16 - cameraY[App->CurrentViewport],
				256, 24,

				App->drawZ,

				waveFr / 3.0f, 0, 1 / 3.0f, 1,

				2);
		}
	}
	App->renderSpriteOPT(tex_Waves->tex, 2);
	cameraX[App->CurrentViewport] -= waveOff;
}
void Scene_HCZ::Render() {
	LevelScene::Render();
}
void Scene_HCZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
