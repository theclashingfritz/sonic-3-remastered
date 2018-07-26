#include "../../Standard.h"
#include "Level.h"

#include "../../Application.h"
#include "../../Resources.h"

#include "../../Engine/IO/IResources.h"
#include "../../Engine/IO/IStream.h"
#include "../../Engine/ITexture.h"
#include "../../Engine/IGraphics.h"
#include "../../Engine/ICrypt.h"
#include "../../Engine/IE.h"

#include "../../S3/LevelScene.h"
#include "../../S3/Object.h"
#include "../../S3/Player.h"
#include "../../S3/ObjectNames.h"

#include <zlib.h>
#include <time.h>

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

std::unordered_map<std::string, const char*> ObjectHashes_HCZ;

int frame_HCZ = 0;
int sceneIn_HCZ = -2;
float viewX_HCZ = 0;
float viewY_HCZ = 0;

IPlayer* player_HCZ = NULL;
SceneData* data_HCZ = new SceneData();

ISprite* buttons_HCZ = NULL;

PlaneSwitch PlaneSwitchers_HCZ[113];
int         PlaneSwitchCount_HCZ = 0;

int FGLayer_HCZ = 2;

bool CollisionAt_HCZ(int probeX, int probeY, int layer, int* angle) {
    return false;

    int tileX = probeX / 16;
    int tileY = probeY / 16;
    //int l = layer;
    for (int l = FGLayer_HCZ + 1; l >= FGLayer_HCZ; l--) {
        int tileID = data_HCZ->layers[l].Tiles[tileX + tileY * data_HCZ->layers[l].Width];
        if ((tileID & 0x3FF) != 0x3FF) {
            bool tileData1 = ((tileID >> 15) & 1) == 1;
            bool tileData2 = ((tileID >> 14) & 1) == 1;
            bool tileData3 = ((tileID >> 13) & 1) == 1;
            bool tileData4 = ((tileID >> 12) & 1) == 1;

            bool tileDatas[4] = { tileData4, tileData3, tileData2, tileData1 };

            bool flipX = ((tileID >> 10) & 1) == 1;
            bool flipY = ((tileID >> 11) & 1) == 1;

            tileID = tileID & 0x3FF;

            if (!tileDatas[player_HCZ->Layer * 2] && !tileDatas[player_HCZ->Layer * 2 + 1])
                continue;

            for (int c = 0; c < 16; c++) {
                int eex = c;
                if (flipX)
                    eex = 15 - c;

                int tX = tileX * 16;
                int tY = tileY * 16;

                unsigned int h1 = data_HCZ->tiles1[tileID].Collision[c];
                unsigned int h2 = data_HCZ->tiles2[tileID].Collision[c];

                if (flipY) {
                    h1 = 15 - h1;
                    h2 = 15 - h2;
                }

                int which = 0;
                if (player_HCZ->AngleMode == 0)
                    which = 0 + flipY * 3;
                if (player_HCZ->AngleMode == 2)
                    which = 3 - flipY * 3;

                if (player_HCZ->AngleMode == 1)
                    which = 1 + flipX;
                if (player_HCZ->AngleMode == 3)
                    which = 2 - flipX;

                if (data_HCZ->tiles1[tileID].HasCollision[c] && player_HCZ->Layer == 0) {
                    if (data_HCZ->tiles1[tileID].IsCeiling ^ flipY) {
                        if (!tileDatas[player_HCZ->Layer * 2 + 1] && (player_HCZ->Y + 14 > tY))
                            continue;

                        if (probeX == tX + eex &&
                            probeY >= tY &&
                            probeY <  tY + 1 + h1) {
                            float tempAngle = data_HCZ->tiles1[tileID].Config[which];
                            tempAngle = tempAngle * -45.f / 32;
                            if (tempAngle != 0) {
                                if (flipX) tempAngle = -tempAngle;
                                if (flipY) tempAngle = 180.f - tempAngle;
                            }
                            if (player_HCZ->Y < tY)
                                tempAngle = 0;

                            *angle = (int)wrapAngle(tempAngle);

                            return true;
                        }
                    }
                    else {
                        if (!tileDatas[player_HCZ->Layer * 2 + 1] && (player_HCZ->Y + 14 > tY + h1))
                            continue;

                        if (probeX == tX + eex &&
                            probeY >= tY + h1 &&
                            probeY <  tY + 16) {
                            float tempAngle = data_HCZ->tiles1[tileID].Config[which];
                            tempAngle = tempAngle * -45.f / 32;
                            if (tempAngle != 0) {
                                if (flipX) tempAngle = -tempAngle;
                                if (flipY) tempAngle = 180.f - tempAngle;
                            }

                            *angle = (int)wrapAngle(tempAngle);
                            return true;
                        }
                    }
                }
                if (data_HCZ->tiles2[tileID].HasCollision[c] && player_HCZ->Layer == 1) {
                    if (data_HCZ->tiles2[tileID].IsCeiling ^ flipY) {
                        if (!tileDatas[player_HCZ->Layer * 2 + 1] && (player_HCZ->Y + 14 > tY))
                            continue;

                        if (probeX == tX + eex &&
                            probeY >= tY &&
                            probeY <  tY + 1 + h2) {
                            float tempAngle = data_HCZ->tiles2[tileID].Config[which];
                            tempAngle = tempAngle * -45.f / 32;
                            if (tempAngle != 0) {
                                if (flipX) tempAngle = -tempAngle;
                                if (flipY) tempAngle = 180.f - tempAngle;
                            }

                            *angle = (int)wrapAngle(tempAngle);
                            return true;
                        }
                    }
                    else {
                        if (!tileDatas[player_HCZ->Layer * 2 + 1] && (player_HCZ->Y + 14 > tY + h2))
                            continue;

                        if (probeX == tX + eex &&
                            probeY >= tY + h2 &&
                            probeY <  tY + 16) {
                            float tempAngle = data_HCZ->tiles2[tileID].Config[which];
                            tempAngle = tempAngle * -45.f / 32;
                            if (tempAngle != 0) {
                                if (flipX) tempAngle = -tempAngle;
                                if (flipY) tempAngle = 180.f - tempAngle;
                            }

                            *angle = (int)wrapAngle(tempAngle);
                            return true;
                        }
                    }
                }
            }

            //break;
        }
    }
    return false;
}

Scene_S3HCZ::Scene_S3HCZ(Application* app, int data1, int data2) {
    App = app;
    G = app->g;

    sceneIn_HCZ = 40;

    aud_BGM_TitleScreen = newMusic(snd_BGM_TitleScreen_ogg, "res/HCZ/act_2.ogg");

    aud_C_ShortBeep = newSound(snd_C_ShortBeep_ogg, "snd/C_ShortBeep.ogg");

    App->Stack->Push("Act 2", aud_BGM_TitleScreen, 0, 0, 0, true, false, false, 179390 / 44100.f);

    S("Background") = new ISprite(IResources::Load("Sprites/Common/Title_BG.spr"), T("Common"));

    S("SmallFont") = new ISprite(IResources::Load("Sprites/Common/Font40x2.spr"), T("Common"));

    if (!S("Buttons"))
        S("Buttons") = new ISprite(IResources::Load("Sprites/Common/HUD_Buttons6.spr"), T("Common"));
    buttons_HCZ = S("Buttons");

    //S("Sonic") = new ISprite(IResources::Load("Sprites/Player/Sonic.spr"), new ITexture(IResources::Load("Sprites/Player/Texture.png"), false));

    S("Sonic") = new ISprite(IResources::Load("Sprites/Player/ManiaSonic.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaSonic.png"), false));
    S("Tails") = new ISprite(IResources::Load("Sprites/Player/ManiaTails.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaTails.png"), false));
    S("Knuckles") = new ISprite(IResources::Load("Sprites/Player/ManiaKnux.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaKnux.png"), false));

    player_HCZ = new IPlayer();
    player_HCZ->App = App;
    player_HCZ->G = G;
    //player_HCZ->CollisionAt = CollisionAt_HCZ;
    player_HCZ->ForegroundLayer = FGLayer_HCZ;

    srand(time(NULL));

    player_HCZ->Sprite = S("Knuckles");
    player_HCZ->Character = CharacterType::Knuckles;

    player_HCZ->Create();

    SDL_RWops *RWList[1 + 2 + 20 + 12] = {
        newFile(res_HCZ_level_lvl, "res/HCZ/level.lvl"),
        // Musics
        newFile(res_HCZ_act_1_ogg, "res/HCZ/act_1.ogg"),
        newFile(res_HCZ_act_2_ogg, "res/HCZ/act_2.ogg"),
        // Layouts
        newFile(res_HCZ_layout_1_bin, "res/HCZ/layout/1.bin"), // Pre-Act
        newFile(res_HCZ_layout_1_bin, "res/HCZ/layout/1.bin"), // Act 1
        newFile(res_HCZ_layout_2_bin, "res/HCZ/layout/2.bin"),
        newFile(res_HCZ_layout_2_bin, "res/HCZ/layout/2.bin"),
        // Chunks (128x128)
        newFile(res_HCZ_chunks_1_bin, "res/HCZ/chunks/1.bin"), // Pre-Act
        newFile(res_HCZ_chunks_1_bin, "res/HCZ/chunks/1.bin"), // Act 1
        newFile(res_HCZ_chunks_2_bin, "res/HCZ/chunks/2.bin"),
        newFile(res_HCZ_chunks_2_bin, "res/HCZ/chunks/2.bin"),
        // Tiles (16x16)
        newFile(res_HCZ_tiles_0_bin, "res/HCZ/tiles/0.bin"), // Pre-Act
        newFile(res_HCZ_tiles_1_bin, "res/HCZ/tiles/1.bin"), // Act 1
        newFile(res_HCZ_tiles_2_bin, "res/HCZ/tiles/2.bin"),
        newFile(res_HCZ_tiles_2_bin, "res/HCZ/tiles/2.bin"),
        // Rings
        newFile(res_HCZ_rings_2_bin, "res/HCZ/rings/1.bin"), // Pre-Act
        newFile(res_HCZ_rings_1_bin, "res/HCZ/rings/1.bin"), // Act 1
        newFile(res_HCZ_rings_2_bin, "res/HCZ/rings/2.bin"),
        newFile(res_HCZ_rings_2_bin, "res/HCZ/rings/2.bin"),
        // Objects
        newFile(res_HCZ_objects_1_bin, "res/HCZ/objects/1.bin"), // Pre-Act
        newFile(res_HCZ_objects_1_bin, "res/HCZ/objects/1.bin"), // Act 1
        newFile(res_HCZ_objects_2_bin, "res/HCZ/objects/2.bin"),
        newFile(res_HCZ_objects_2_bin, "res/HCZ/objects/2.bin"),
        // Palettes (Dry)
        newFile(res_HCZ_PAL1_DRY_png, "res/HCZ/PAL1_DRY.png"), // Pre-Act
        newFile(res_HCZ_PAL1_DRY_png, "res/HCZ/PAL1_DRY.png"), // Act 1
        newFile(res_HCZ_PAL2_DRY_png, "res/HCZ/PAL2_DRY.png"),
        newFile(res_HCZ_PAL2_DRY_png, "res/HCZ/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_HCZ_PAL1_WET_png, "res/HCZ/PAL1_WET.png"), // Pre-Act
        newFile(res_HCZ_PAL1_WET_png, "res/HCZ/PAL1_WET.png"), // Act 1
        newFile(res_HCZ_PAL2_WET_png, "res/HCZ/PAL2_WET.png"),
        newFile(res_HCZ_PAL2_WET_png, "res/HCZ/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_HCZ_PALm_DRY_png, "res/HCZ/PALm_DRY.png"), // Mini-boss
        newFile(res_HCZ_PALm_DRY_png, "res/HCZ/PALm_DRY.png"), // Boss
        newFile(res_HCZ_PALm_WET_png, "res/HCZ/PALm_WET.png"), // Extra 1 (Mini-boss)
        newFile(res_HCZ_PALm_WET_png, "res/HCZ/PALm_WET.png"), // Extra 2
    };

    /*
    unsigned char SceneBinData[0x20000];
    IResource* SceneBin = IResources::Load("Sprites/MSZ/Scene2.bin");
    size_t size = (size_t)SDL_RWsize(SceneBin->RW);
    SDL_RWread(SceneBin->RW, SceneBinData, size, 1);
    IResources::Close(SceneBin);

    IStream reader(SceneBinData);
    */

    data_HCZ->layerCount = 0;
}

void Scene_S3HCZ::Update() {
    player_HCZ->Ground = true;
    player_HCZ->Gravity = 0;

    player_HCZ->X = 960;
    player_HCZ->Y = 960;

    if (App->realPaused) return;

    frame_HCZ++;
    frame_HCZ = (frame_HCZ) % 4800000;

    for (int i = 0; i < PlaneSwitchCount_HCZ; i++) {
        int Size = 16 * PlaneSwitchers_HCZ[i].Size + 16;
        if (player_HCZ->X + 11 >= PlaneSwitchers_HCZ[i].X - 2 &&
            player_HCZ->X - 11 <  PlaneSwitchers_HCZ[i].X + 2 &&
            player_HCZ->Y + 11 >= PlaneSwitchers_HCZ[i].Y - Size / 2 &&
            player_HCZ->Y - 11 <  PlaneSwitchers_HCZ[i].Y + Size / 2) {
            if (PlaneSwitchers_HCZ[i].OnPath) {
                if (player_HCZ->Ground) {
                    if (player_HCZ->Speed * player_HCZ->Cos[(int)PlaneSwitchers_HCZ[i].Angle % 360] < 0) {
                        player_HCZ->Layer = (PlaneSwitchers_HCZ[i].Flags >> 3) & 1;
                        player_HCZ->VisualLayer = (PlaneSwitchers_HCZ[i].Flags >> 2) & 1;
                    }
                    else {
                        player_HCZ->Layer = (PlaneSwitchers_HCZ[i].Flags >> 1) & 1;
                        player_HCZ->VisualLayer = (PlaneSwitchers_HCZ[i].Flags >> 0) & 1;
                    }
                }
            }
            else {
                if (player_HCZ->Speed * player_HCZ->Cos[(int)PlaneSwitchers_HCZ[i].Angle % 360] > 0) {
                    player_HCZ->Layer = (PlaneSwitchers_HCZ[i].Flags >> 3) & 1;
                    player_HCZ->VisualLayer = (PlaneSwitchers_HCZ[i].Flags >> 2) & 1;
                }
                else {
                    player_HCZ->Layer = (PlaneSwitchers_HCZ[i].Flags >> 1) & 1;
                    player_HCZ->VisualLayer = (PlaneSwitchers_HCZ[i].Flags >> 0) & 1;
                }
            }
        }
    }

    player_HCZ->Update();

    viewX_HCZ = (int)(player_HCZ->X + player_HCZ->CameraX) - App->renderWidth / 2;
    viewY_HCZ = (int)(player_HCZ->Y + player_HCZ->CameraY) - App->renderHeight / 2;

    if (viewX_HCZ < 0)
        viewX_HCZ = 0;
    if (viewY_HCZ < 0)
        viewY_HCZ = 0;

    if (viewX_HCZ > data_HCZ->layers[data_HCZ->cameraLayer].Width * 16 - App->renderWidth)
        viewX_HCZ = data_HCZ->layers[data_HCZ->cameraLayer].Width * 16 - App->renderWidth;
    if (viewY_HCZ > data_HCZ->layers[data_HCZ->cameraLayer].Height * 16 - App->renderHeight)
        viewY_HCZ = data_HCZ->layers[data_HCZ->cameraLayer].Height * 16 - App->renderHeight;

    if (sceneIn_HCZ > -2) {
        sceneIn_HCZ--;
    }
}
void Scene_S3HCZ::Render() {
    if (sceneIn_HCZ > -2) {
        G->SetFade(sceneIn_HCZ / 40.f);
    }

    G->DrawSprite(S("Background"), 0, 0, Z_BACK, 0, IE::LeftAlign | IE::TopAlign);

    player_HCZ->Render((int)viewX_HCZ, (int)viewY_HCZ);


    if (!App->Mobile) {
        IColor color = IColor(1.f, 0.f, 0.f, 0.5f);

        int ang = -1;
        if (CollisionAt_HCZ((int)App->input->mouseX + (int)viewX_HCZ, (int)App->input->mouseY + (int)viewY_HCZ, player_HCZ->Layer + 2, &ang)) {
            color = IColor(0.f, 1.f, 1.f, 0.5f);

            char sup[60];
            sprintf(sup, "%d", ang);
            DrawSmallFont(S("SmallFont"), sup, (int)App->input->mouseX + 8, (int)App->input->mouseY - 12, 0);
        }

        G->DrawRectangle((int)App->input->mouseX, (int)App->input->mouseY - 8, Z_FRONT * 4.5, 1, 17, color);
        G->DrawRectangle((int)App->input->mouseX - 8, (int)App->input->mouseY, Z_FRONT * 4.5, 17, 1, color);

        char sup[60];
        sprintf(sup, "%d %d", (int)App->input->mouseX + (int)viewX_HCZ, (int)App->input->mouseY + (int)viewY_HCZ);
        //DrawSmallFont(S("SmallFont"), sup, (int)App->input->mouseX + 8, (int)App->input->mouseY + 8, 0);

        sprintf(sup, "X %d Y %d ANGLE %d MODE %d GROUND %d FRAME %d", (int)player_HCZ->X, (int)player_HCZ->Y, player_HCZ->Angle, player_HCZ->AngleMode, player_HCZ->Ground, (int)player_HCZ->Frame);
        DrawSmallFont(S("SmallFont"), sup, 8, 225 - 8 - 32, 0);
    }


    if (!App->Mobile) {
        for (int i = 0; i < PlaneSwitchCount_HCZ; i++) {
            int Size = 16 * PlaneSwitchers_HCZ[i].Size + 16;
            int Width = 8;
            G->DrawRectangle((int)PlaneSwitchers_HCZ[i].X - (int)viewX_HCZ - Width / 2, (int)PlaneSwitchers_HCZ[i].Y - (int)viewY_HCZ - Size / 2, Z_FRONT * 0.2f - 0.05f, Width, Size, IColor(0.0f, 1.0f, 1.0f, 1.0f));
        }

        #if 1
        //int l = player_HCZ->Layer + 2;
        int tileX = ((int)App->input->mouseX + (int)viewX_HCZ) / 16;
        int tileY = ((int)App->input->mouseY + (int)viewY_HCZ) / 16;

        for (int l = FGLayer_HCZ + 1 - player_HCZ->Layer; l < FGLayer_HCZ + 1 - player_HCZ->Layer + 1; l++) {
            int tileID = data_HCZ->layers[l].Tiles[tileX + tileY * data_HCZ->layers[l].Width];
            if ((tileID & 0x3FF) != 0x3FF) {
                bool tileData1 = ((tileID >> 15) & 1) == 1;
                bool tileData2 = ((tileID >> 14) & 1) == 1;
                bool tileData3 = ((tileID >> 13) & 1) == 1;
                bool tileData4 = ((tileID >> 12) & 1) == 1;

                bool flipX = ((tileID >> 10) & 1) == 1;
                bool flipY = ((tileID >> 11) & 1) == 1;

                tileID = tileID & 0x3FF;

                if (App->input->mouseDown) {
                    if (App->input->mousePrimary) {
                        char sup[100];
                        sprintf(sup, "%03X %d:%d    %02X %02X %02X %02X %02X     %d %d %d",
                            tileID, tileID % 32, tileID / 32,
                            data_HCZ->tiles1[tileID].Config[0], data_HCZ->tiles1[tileID].Config[1], data_HCZ->tiles1[tileID].Config[2], data_HCZ->tiles1[tileID].Config[3], data_HCZ->tiles1[tileID].Config[4],
                            flipX, flipY, data_HCZ->tiles1[tileID].IsCeiling);
                        DrawSmallFont(S("SmallFont"), sup, 8, 12 + (l - 2) * 32, 0);
                        sprintf(sup, "BITS: %d %d %d %d", tileData1, tileData2, tileData3, tileData4);
                        DrawSmallFont(S("SmallFont"), sup, 8, 28 + (l - 2) * 32, 0);
                    }
                    else {
                        char sup[100];
                        sprintf(sup, "%03X %d:%d    %02X %02X %02X %02X %02X     %d %d %d",
                            tileID, tileID % 32, tileID / 32,
                            data_HCZ->tiles2[tileID].Config[0], data_HCZ->tiles2[tileID].Config[1], data_HCZ->tiles2[tileID].Config[2], data_HCZ->tiles2[tileID].Config[3], data_HCZ->tiles2[tileID].Config[4],
                            flipX, flipY, data_HCZ->tiles2[tileID].IsCeiling);
                        DrawSmallFont(S("SmallFont"), sup, 8, 12 + (l - 2) * 32, 0);
                        sprintf(sup, "BITS: %d %d %d %d", tileData1, tileData2, tileData3, tileData4);
                        DrawSmallFont(S("SmallFont"), sup, 8, 28 + (l - 2) * 32, 0);
                    }
                }
                //*/


                for (int c = 0; c < 16; c++) {
                    int eex = c;
                    if (flipX)
                        eex = 15 - c;

                    int tX = tileX * 16 - (int)viewX_HCZ;
                    int tY = tileY * 16 - (int)viewY_HCZ;

                    unsigned int h1 = data_HCZ->tiles1[tileID].Collision[c];
                    unsigned int h2 = data_HCZ->tiles2[tileID].Collision[c];

                    if (flipY) {
                        h1 = 15 - h1;
                        h2 = 15 - h2;
                    }

                    if (App->input->mouseDown) {
                        if (App->input->mousePrimary) {
                            if (data_HCZ->tiles1[tileID].IsCeiling ^ flipY) {
                                if (data_HCZ->tiles1[tileID].HasCollision[c])
                                    G->DrawRectangle(tX + eex, tY, Z_FRONT * 2.5f, 1, h1, IColor(1.f, 0.f, 0.f, 1.0f));
                            }
                            else {
                                if (data_HCZ->tiles1[tileID].HasCollision[c])
                                    G->DrawRectangle(tX + eex, tY + h1, Z_FRONT * 2.5f, 1, 16 - h1, IColor(1.f, 0.f, 0.f, 1.0f));
                            }
                        }
                        else {
                            if (data_HCZ->tiles2[tileID].IsCeiling ^ flipY) {
                                if (data_HCZ->tiles2[tileID].HasCollision[c])
                                    G->DrawRectangle(tX + eex, tY, Z_FRONT * 2.6f, 1, h2, IColor(0.f, 0.f, 1.f, 1.0f));
                            }
                            else {
                                if (data_HCZ->tiles2[tileID].HasCollision[c])
                                    G->DrawRectangle(tX + eex, tY + h2, Z_FRONT * 2.6f, 1, 16 - h2, IColor(0.f, 0.f, 1.f, 1.0f));
                            }
                        }
                    }
                }
            }
        }
        #endif
    }

    if (!App->Mobile) {
        char jumper[100];
        sprintf(jumper, "COLLIS LAYER: %s", player_HCZ->Layer ? "HIGH" : "LOW");
        DrawSmallFont(S("SmallFont"), jumper, 8, 225 - 8 - 16, 0);
        sprintf(jumper, "VISUAL LAYER: %s", player_HCZ->VisualLayer ? "HIGH" : "LOW");
        DrawSmallFont(S("SmallFont"), jumper, 8, 225 - 8, 0);
    }

    if (App->Mobile)
        App->input->DrawControls(buttons_HCZ);
}
void Scene_S3HCZ::Free() {
    FreeAudio();
}
