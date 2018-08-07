#ifndef RESOURCES_H
#define RESOURCES_H

#if defined(PLATFORM_MAC)
    #include "mach-o/getsect.h"
    #include "mach-o/ldsyms.h"

    #ifdef BUILD_SHARED_LIBS
        #if !defined(__LP64__)
            const struct mach_header _mh_execute_header;
        #else
            const struct mach_header_64 _mh_execute_header;
        #endif
    #else
        #if !defined(__LP64__)
            extern const struct mach_header _mh_execute_header;
        #else
            extern const struct mach_header_64 _mh_execute_header;
        #endif
    #endif

    int macDataLen(const char* oldStr);
    unsigned char* macDataPtr(const char* oldStr);
    #define EXTLD(NAME) /*extern unsigned char __DATA__ ## NAME;*/
    #define LDVAR(NAME) macDataPtr(#NAME)
    #define LDLEN(NAME) macDataLen(#NAME)

    #if defined(DEBUGGLE)
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "../../../source/" SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "../../../source/" SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio("../../../source/" SRC, false)
        #define newMusic(NAME, SRC) new Audio("../../../source/" SRC,  true)
        #define newMusic2(NAME, SRC) new Audio("../../../source/" SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile("../../../source/" SRC, "rb")
    #else
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio(SRC, false)
        #define newMusic(NAME, SRC) new Audio(SRC,  true)
        #define newMusic2(NAME, SRC) new Audio(SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile(SRC, "rb")
    #endif
#elif defined(PLATFORM_IOS)
    #define EXTLD(NAME) ;
    #define LDVAR(NAME) (NULL)
    #define LDLEN(NAME) (0)

    #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, false)
    #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, true)
    #define newSound(NAME, SRC) new Audio(SRC, false)
    #define newMusic(NAME, SRC) new Audio(SRC,  true)
    #define newMusic2(NAME, SRC) new Audio(SRC,    2)
    #define newFile(NAME, SRC) SDL_RWFromFile(SRC, "rb")
#elif defined(PLATFORM_ANDROID)
    #define EXTLD(NAME) ;
    #define LDVAR(NAME) (NULL)
    #define LDLEN(NAME) (0)

    #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, false)
    #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, true)
    #define newSound(NAME, SRC) new Audio(SRC, false)
    #define newMusic(NAME, SRC) new Audio(SRC,  true)
    #define newMusic2(NAME, SRC) new Audio(SRC,    2)
    #define newFile(NAME, SRC) SDL_RWFromFile(SRC, "rb")
#else // PLATFORM_WINDOWS, PLATFORM_VITA
    #define EXTLD(NAME) \
        extern unsigned char _binary_source_##NAME##_start; \
        extern unsigned char _binary_source_##NAME##_size; \
        extern unsigned char _binary_source_##NAME##_end;
    #define LDVAR(NAME) &_binary_source_##NAME##_start
    #define LDLEN(NAME) (int)&_binary_source_##NAME##_size

    #ifdef DEBUGGLE
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "source/" SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "source/" SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio("source/" SRC, false)
        #define newMusic(NAME, SRC) new Audio("source/" SRC,  true)
        #define newMusic2(NAME, SRC) new Audio("source/" SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile("source/" SRC, "rb")
        #define testtest(NAME, SRC) ("source/" SRC)
    #else
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, LDVAR(NAME), LDLEN(NAME), FRAMES, FRAMESH, SRC, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, LDVAR(NAME), LDLEN(NAME), FRAMES, FRAMESH, SRC, true)
        #define newSound(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME), false)
        #define newMusic(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME),  true)
        #define newMusic2(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME),    2)
        #define newFile(NAME, SRC) SDL_RWFromMem(LDVAR(NAME), LDLEN(NAME))
    #endif
#endif

EXTLD(res_basicFS_bin);
EXTLD(res_basicVS_bin);
EXTLD(res_bwFS_bin);
EXTLD(res_bwVS_bin);
EXTLD(res_bw_bin);
EXTLD(res_Common_AirCountdown8x2_png);
EXTLD(res_Common_Effect_BigWave6_png);
EXTLD(res_Common_Effect_DashDust7_png);
EXTLD(res_Common_Effect_EnemyExplosion5_png);
EXTLD(res_Common_Effect_Explosion6_png);
EXTLD(res_Common_Effect_SkidDust5_png);
EXTLD(res_Common_Effect_WaterRun5_png);
EXTLD(res_Common_Effect_WaterSplash10_png);
EXTLD(res_Common_EnemyPoints7_png);
EXTLD(res_Common_GrayButton3_png);
EXTLD(res_Common_HUD_Act2_png);
EXTLD(res_Common_HUD_S3K_png);
EXTLD(res_Common_HUD_SmallNumbers10_png);
EXTLD(res_Common_HUD_STR_png);
EXTLD(res_Common_HUD_ZoneNameFont26_png);
EXTLD(res_Common_Lives_Knuckles_png);
EXTLD(res_Common_Lives_Sonic_png);
EXTLD(res_Common_Lives_Tails_png);
EXTLD(res_Common_LVL_Angles_bin);
EXTLD(res_Common_LVL_Heights_bin);
EXTLD(res_Common_Monitor8x2_png);
EXTLD(res_Common_Results_BonuTotal2_png);
EXTLD(res_Common_Results_GotThrough2_png);
EXTLD(res_Common_Results_Player_strip5_png);
EXTLD(res_Common_Ring8_png);
EXTLD(res_Common_Shield_Bubble8x2_png);
EXTLD(res_Common_Shield_Fire8x3_png);
EXTLD(res_Common_Shield_Insta7_png);
EXTLD(res_Common_Shield_Invincibility8_png);
EXTLD(res_Common_Shield_Lightning8x2_png);
EXTLD(res_Common_Signpost7_png);
EXTLD(res_Common_SignpostKnux7_png);
EXTLD(res_Common_SignpostStub_png);
EXTLD(res_Common_Sonic_png);
EXTLD(res_Common_SpecialRing8x2_png);
EXTLD(res_Common_Spikes_png);
EXTLD(res_Common_SpringRed3_png);
EXTLD(res_Common_SpringYellow3_png);
EXTLD(res_Common_Starpost5_png);
EXTLD(res_Common_Tails_png);
EXTLD(res_Common_Tails_Tails_png);
EXTLD(res_HUD_Buttons6_png);
EXTLD(res_crtFS_bin);
EXTLD(res_crtVS_bin);
EXTLD(res_Font40x2_png);

// AIZ
EXTLD(Resource_Stages_AIZ_level_lvl);
EXTLD(Resource_Stages_AIZ_Music_Act_1_ogg);
EXTLD(Resource_Stages_AIZ_Music_Act_2_ogg);
EXTLD(Resource_Stages_AIZ_Data_Layout_1_bin);
EXTLD(Resource_Stages_AIZ_Data_Layout_2_bin);
EXTLD(Resource_Stages_AIZ_Data_Chunks_1_bin);
EXTLD(Resource_Stages_AIZ_Data_Chunks_2_bin);
EXTLD(Resource_Stages_AIZ_Data_Tiles_0_bin);
EXTLD(Resource_Stages_AIZ_Data_Tiles_1_bin);
EXTLD(Resource_Stages_AIZ_Data_Tiles_2_bin);
EXTLD(Resource_Stages_AIZ_Data_Tiles_3_bin);
EXTLD(Resource_Stages_AIZ_Data_Rings_1_bin);
EXTLD(Resource_Stages_AIZ_Data_Rings_2_bin);
EXTLD(Resource_Stages_AIZ_Data_Objects_1_bin);
EXTLD(Resource_Stages_AIZ_Data_Objects_2_bin);
EXTLD(Resource_Stages_AIZ_PAL0_DRY_png);
EXTLD(Resource_Stages_AIZ_PAL1_DRY_png);
EXTLD(Resource_Stages_AIZ_PAL2_DRY_png);
EXTLD(Resource_Stages_AIZ_PAL1_WET_png);
EXTLD(Resource_Stages_AIZ_PAL2_WET_png);
EXTLD(res_AIZ_PAL1_DRY_png);


// HCZ
EXTLD(res_HCZ_level_lvl);
EXTLD(res_HCZ_act_1_ogg);
EXTLD(res_HCZ_act_2_ogg);
EXTLD(res_HCZ_tiles_0_bin);
EXTLD(res_HCZ_tiles_1_bin);
EXTLD(res_HCZ_tiles_2_bin);
EXTLD(res_HCZ_anim1_28x303_png);
EXTLD(res_HCZ_anim1_29x303_png);
EXTLD(res_HCZ_anim1_2Ax303_png);
EXTLD(res_HCZ_anim1_2Dx913_png);
EXTLD(res_HCZ_anim1_2Ex913_png);
EXTLD(res_HCZ_anim1_5Fx913_png);
EXTLD(res_HCZ_anim1_8Bx303_png);
EXTLD(res_HCZ_anim1_8Cx303_png);
EXTLD(res_HCZ_anim1_8Dx303_png);
EXTLD(res_HCZ_anim1_D8x913_png);
EXTLD(res_HCZ_anim1_D9x913_png);
EXTLD(res_HCZ_anim1_DAx913_png);
EXTLD(res_HCZ_anim1_DBx913_png);
EXTLD(res_HCZ_anim1_DDx913_png);
EXTLD(res_HCZ_anim1_DEx913_png);
EXTLD(res_HCZ_anim1_DFx913_png);
EXTLD(res_HCZ_anim2_1Cx401_png);
EXTLD(res_HCZ_anim2_28x913_png);
EXTLD(res_HCZ_anim2_2Cx913_png);
EXTLD(res_HCZ_anim2_58x401_png);
EXTLD(res_HCZ_anim2_84x401_png);
EXTLD(res_HCZ_anim2_9Bx401_png);
EXTLD(res_HCZ_anim2_A2x913_png);
EXTLD(res_HCZ_anim2_A3x401_png);
EXTLD(res_HCZ_anim2_CAx828_png);
EXTLD(res_HCZ_anim2_CBx828_png);
EXTLD(res_HCZ_anim2_CCx828_png);
EXTLD(res_HCZ_anim2_CEx828_png);
EXTLD(res_HCZ_anim2_D0x913_png);
EXTLD(res_HCZ_anim2_D1x913_png);
EXTLD(res_HCZ_anim2_D2x913_png);
EXTLD(res_HCZ_anim2_D3x913_png);
EXTLD(res_HCZ_anim2_D7x913_png);
EXTLD(res_HCZ_anim2_D8x913_png);
EXTLD(res_HCZ_anim2_D9x913_png);
EXTLD(res_HCZ_anim2_DAx913_png);
EXTLD(res_HCZ_anim2_DBx913_png);
EXTLD(res_HCZ_anim2_DDx913_png);
EXTLD(res_HCZ_anim2_DEx913_png);
EXTLD(res_HCZ_anim2_DFx913_png);
EXTLD(res_HCZ_Blastoid4_png);
EXTLD(res_HCZ_Block_png);
EXTLD(res_HCZ_BossEffects41_png);
EXTLD(res_HCZ_BossExtra8_png);
EXTLD(res_HCZ_BreakableBar1x3_png);
EXTLD(res_HCZ_BreakablePlatforms8x2_png);
EXTLD(res_HCZ_Bridge_png);
EXTLD(res_HCZ_Bubble8x2_png);
EXTLD(res_HCZ_Bubbler3_png);
EXTLD(res_HCZ_Buggernaut6_png);
EXTLD(res_HCZ_Button3_png);
EXTLD(res_HCZ_chunks_1_bin);
EXTLD(res_HCZ_chunks_2_bin);
EXTLD(res_HCZ_Chunks2Back16x16_png);
EXTLD(res_HCZ_Chunks2Front16x16_png);
EXTLD(res_HCZ_ChunksBack16x16_png);
EXTLD(res_HCZ_ChunksFront16x16_png);
EXTLD(res_HCZ_Fan5x1_png);
EXTLD(res_HCZ_FloatingPlatform_png);
EXTLD(res_HCZ_Geyser_png);
EXTLD(res_HCZ_GeyserParticles1_8_png);
EXTLD(res_HCZ_GeyserParticles2_2_png);
EXTLD(res_HCZ_HandLauncher8_png);
EXTLD(res_HCZ_Jawz2_png);
EXTLD(res_HCZ_LargeFan5_png);
EXTLD(res_HCZ_layout_1_bin);
EXTLD(res_HCZ_layout_2_bin);
EXTLD(res_HCZ_MegaChopper4_png);
EXTLD(res_HCZ_Miniboss8x3_png);
EXTLD(res_HCZ_MinibossPole3_png);
EXTLD(res_HCZ_NonAnim1_5_png);
EXTLD(res_HCZ_NonAnim2_5_png);
EXTLD(res_HCZ_objects_1_bin);
EXTLD(res_HCZ_objects_2_bin);
EXTLD(res_HCZ_PAL1_DRY_png);
EXTLD(res_HCZ_PAL1_WET_png);
EXTLD(res_HCZ_PAL2_DRY_png);
EXTLD(res_HCZ_PAL2_WET_png);
EXTLD(res_HCZ_PALm_DRY_png);
EXTLD(res_HCZ_PALm_WET_png);
EXTLD(res_HCZ_Pointdexter3_png);
EXTLD(res_HCZ_rings_1_bin);
EXTLD(res_HCZ_rings_2_bin);
EXTLD(res_HCZ_SpinningColumn3_png);
EXTLD(res_HCZ_StarpostWater5_png);
EXTLD(res_HCZ_TurboSpiker8x2_png);
EXTLD(res_HCZ_WaterDrop6_png);
EXTLD(res_HCZ_WaterlineTop_png);
EXTLD(res_HCZ_WaterRush4_png);
EXTLD(res_HCZ_Waves3_png);

// CNZ
EXTLD(res_CNZ_level_lvl);
EXTLD(res_CNZ_act_1_ogg);
EXTLD(res_CNZ_act_2_ogg);
EXTLD(res_CNZ_tiles_0_bin);
EXTLD(res_CNZ_Balloon8x3_png);
EXTLD(res_CNZ_Barrel4_png);
EXTLD(res_CNZ_BreakableWall_png);
EXTLD(res_CNZ_Bumper2_png);
EXTLD(res_CNZ_chunks_1_bin);
EXTLD(res_CNZ_chunks_2_bin);
EXTLD(res_CNZ_CorkFloor_png);
EXTLD(res_CNZ_Door_png);
EXTLD(res_CNZ_HoverFan_png);
EXTLD(res_CNZ_layout_1_bin);
EXTLD(res_CNZ_layout_2_bin);
EXTLD(res_CNZ_Lightbulb_png);
EXTLD(res_CNZ_objects_1_bin);
EXTLD(res_CNZ_objects_2_bin);
EXTLD(res_CNZ_PAL1_DRY_png);
EXTLD(res_CNZ_PAL1_WET_png);
EXTLD(res_CNZ_rings_1_bin);
EXTLD(res_CNZ_rings_2_bin);
EXTLD(res_CNZ_tiles_1_bin);
EXTLD(res_CNZ_tiles_2_bin);
EXTLD(res_CNZ_TILES1_png);
EXTLD(res_CNZ_TILES2_png);
EXTLD(res_CNZ_Trapdoor3_png);
EXTLD(res_CNZ_WeirdPlatform3_png);

// FBZ
EXTLD(res_FBZ_level_lvl);
EXTLD(res_FBZ_act_1_ogg);
EXTLD(res_FBZ_act_2_ogg);
EXTLD(res_FBZ_layout_1_bin);
EXTLD(res_FBZ_layout_2_bin);
EXTLD(res_FBZ_chunks_1_bin);
EXTLD(res_FBZ_chunks_2_bin);
EXTLD(res_FBZ_tiles_0_bin);
EXTLD(res_FBZ_tiles_1_bin);
EXTLD(res_FBZ_tiles_2_bin);
EXTLD(res_FBZ_rings_1_bin);
EXTLD(res_FBZ_rings_2_bin);
EXTLD(res_FBZ_objects_1_bin);
EXTLD(res_FBZ_objects_2_bin);
EXTLD(res_FBZ_PAL1_DRY_png);
EXTLD(res_FBZ_PAL2_DRY_png);
EXTLD(res_FBZ_PAL1_WET_png);
EXTLD(res_FBZ_PAL2_WET_png);
EXTLD(res_FBZ_PALm_DRY_png);
EXTLD(res_FBZ_PALm_WET_png);

// SOZ 
EXTLD(res_SOZ_level_lvl);
EXTLD(res_SOZ_act_1_ogg);
EXTLD(res_SOZ_act_2_ogg);
EXTLD(res_SOZ_layout_1_bin);
EXTLD(res_SOZ_layout_2_bin);
EXTLD(res_SOZ_chunks_1_bin);
EXTLD(res_SOZ_chunks_2_bin);
EXTLD(res_SOZ_tiles_0_bin);
EXTLD(res_SOZ_tiles_1_bin);
EXTLD(res_SOZ_tiles_2_bin);
EXTLD(res_SOZ_rings_1_bin);
EXTLD(res_SOZ_rings_2_bin);
EXTLD(res_SOZ_objects_1_bin);
EXTLD(res_SOZ_objects_2_bin);
EXTLD(res_SOZ_PAL1_DRY_png);
EXTLD(res_SOZ_PAL2_DRY_png);
EXTLD(res_SOZ_PAL1_WET_png);
EXTLD(res_SOZ_PAL2_WET_png);
EXTLD(res_SOZ_PALm_DRY_png);
EXTLD(res_SOZ_PALm_WET_png);

// LRZ
EXTLD(res_LRZ_level_lvl);
EXTLD(res_LRZ_act_1_ogg);
EXTLD(res_LRZ_act_2_ogg);
EXTLD(res_LRZ_layout_1_bin);
EXTLD(res_LRZ_layout_2_bin);
EXTLD(res_LRZ_chunks_1_bin);
EXTLD(res_LRZ_chunks_2_bin);
EXTLD(res_LRZ_tiles_0_bin);
EXTLD(res_LRZ_tiles_1_bin);
EXTLD(res_LRZ_tiles_2_bin);
EXTLD(res_LRZ_rings_1_bin);
EXTLD(res_LRZ_rings_2_bin);
EXTLD(res_LRZ_objects_1_bin);
EXTLD(res_LRZ_objects_2_bin);
EXTLD(res_LRZ_PAL1_DRY_png);
EXTLD(res_LRZ_PAL2_DRY_png);
EXTLD(res_LRZ_PAL1_WET_png);
EXTLD(res_LRZ_PAL2_WET_png);
EXTLD(res_LRZ_PALm_DRY_png);
EXTLD(res_LRZ_PALm_WET_png);

// LBZ
EXTLD(res_LBZ_level_lvl);
EXTLD(res_LBZ_act_1_ogg);
EXTLD(res_LBZ_act_2_ogg);
EXTLD(res_LBZ_layout_1_bin);
EXTLD(res_LBZ_layout_2_bin);
EXTLD(res_LBZ_chunks_1_bin);
EXTLD(res_LBZ_chunks_2_bin);
EXTLD(res_LBZ_tiles_0_bin);
EXTLD(res_LBZ_tiles_1_bin);
EXTLD(res_LBZ_tiles_2_bin);
EXTLD(res_LBZ_rings_1_bin);
EXTLD(res_LBZ_rings_2_bin);
EXTLD(res_LBZ_objects_1_bin);
EXTLD(res_LBZ_objects_2_bin);
EXTLD(res_LBZ_PAL1_DRY_png);
EXTLD(res_LBZ_PAL2_DRY_png);
EXTLD(res_LBZ_PAL1_WET_png);
EXTLD(res_LBZ_PAL2_WET_png);
EXTLD(res_LBZ_PALm_DRY_png);
EXTLD(res_LBZ_PALm_WET_png);

// MHZ
EXTLD(res_MHZ_level_lvl);
EXTLD(res_MHZ_act_1_ogg);
EXTLD(res_MHZ_act_2_ogg);
EXTLD(res_MHZ_layout_1_bin);
EXTLD(res_MHZ_layout_2_bin);
EXTLD(res_MHZ_chunks_1_bin);
EXTLD(res_MHZ_chunks_2_bin);
EXTLD(res_MHZ_tiles_0_bin);
EXTLD(res_MHZ_tiles_1_bin);
EXTLD(res_MHZ_tiles_2_bin);
EXTLD(res_MHZ_rings_1_bin);
EXTLD(res_MHZ_rings_2_bin);
EXTLD(res_MHZ_objects_1_bin);
EXTLD(res_MHZ_objects_2_bin);
EXTLD(res_MHZ_PAL1_DRY_png);
EXTLD(res_MHZ_PAL2_DRY_png);
EXTLD(res_MHZ_PAL1_WET_png);
EXTLD(res_MHZ_PAL2_WET_png);
EXTLD(res_MHZ_PALm_DRY_png);
EXTLD(res_MHZ_PALm_WET_png);

// ICZ
EXTLD(res_ICZ_level_lvl);
EXTLD(res_ICZ_act_1_ogg);
EXTLD(res_ICZ_act_2_ogg);
EXTLD(res_ICZ_layout_1_bin);
EXTLD(res_ICZ_layout_2_bin);
EXTLD(res_ICZ_chunks_1_bin);
EXTLD(res_ICZ_chunks_2_bin);
EXTLD(res_ICZ_tiles_0_bin);
EXTLD(res_ICZ_tiles_1_bin);
EXTLD(res_ICZ_tiles_2_bin);
EXTLD(res_ICZ_rings_1_bin);
EXTLD(res_ICZ_rings_2_bin);
EXTLD(res_ICZ_objects_1_bin);
EXTLD(res_ICZ_objects_2_bin);
EXTLD(res_ICZ_PAL1_DRY_png);
EXTLD(res_ICZ_PAL2_DRY_png);
EXTLD(res_ICZ_PAL1_WET_png);
EXTLD(res_ICZ_PAL2_WET_png);
EXTLD(res_ICZ_PALm_DRY_png);
EXTLD(res_ICZ_PALm_WET_png);
EXTLD(res_ICZ_TILES1_png);

// MGZ
EXTLD(res_MGZ_level_lvl);
EXTLD(res_MGZ_act_1_ogg);
EXTLD(res_MGZ_act_2_ogg);
EXTLD(res_MGZ_layout_1_bin);
EXTLD(res_MGZ_layout_2_bin);
EXTLD(res_MGZ_chunks_1_bin);
EXTLD(res_MGZ_chunks_2_bin);
EXTLD(res_MGZ_tiles_0_bin);
EXTLD(res_MGZ_tiles_1_bin);
EXTLD(res_MGZ_tiles_2_bin);
EXTLD(res_MGZ_rings_1_bin);
EXTLD(res_MGZ_rings_2_bin);
EXTLD(res_MGZ_objects_1_bin);
EXTLD(res_MGZ_objects_2_bin);
EXTLD(res_MGZ_PAL1_DRY_png);
EXTLD(res_MGZ_PAL2_DRY_png);
EXTLD(res_MGZ_PAL1_WET_png);
EXTLD(res_MGZ_PAL2_WET_png);
EXTLD(res_MGZ_PALm_DRY_png);
EXTLD(res_MGZ_PALm_WET_png);

// SSZ
EXTLD(res_SSZ_level_lvl);
EXTLD(res_SSZ_act_1_ogg);
EXTLD(res_SSZ_act_2_ogg);
EXTLD(res_SSZ_layout_1_bin);
EXTLD(res_SSZ_layout_2_bin);
EXTLD(res_SSZ_chunks_1_bin);
EXTLD(res_SSZ_chunks_2_bin);
EXTLD(res_SSZ_tiles_0_bin);
EXTLD(res_SSZ_tiles_1_bin);
EXTLD(res_SSZ_tiles_2_bin);
EXTLD(res_SSZ_rings_1_bin);
EXTLD(res_SSZ_rings_2_bin);
EXTLD(res_SSZ_objects_1_bin);
EXTLD(res_SSZ_objects_2_bin);
EXTLD(res_SSZ_PAL1_DRY_png);
EXTLD(res_SSZ_PAL2_DRY_png);
EXTLD(res_SSZ_PAL1_WET_png);
EXTLD(res_SSZ_PAL2_WET_png);
EXTLD(res_SSZ_PALm_DRY_png);
EXTLD(res_SSZ_PALm_WET_png);

// DEZ
EXTLD(res_DEZ_level_lvl);
EXTLD(res_DEZ_act_1_ogg);
EXTLD(res_DEZ_act_2_ogg);
EXTLD(res_DEZ_layout_1_bin);
EXTLD(res_DEZ_layout_2_bin);
EXTLD(res_DEZ_chunks_1_bin);
EXTLD(res_DEZ_chunks_2_bin);
EXTLD(res_DEZ_tiles_0_bin);
EXTLD(res_DEZ_tiles_1_bin);
EXTLD(res_DEZ_tiles_2_bin);
EXTLD(res_DEZ_rings_1_bin);
EXTLD(res_DEZ_rings_2_bin);
EXTLD(res_DEZ_objects_1_bin);
EXTLD(res_DEZ_objects_2_bin);
EXTLD(res_DEZ_PAL1_DRY_png);
EXTLD(res_DEZ_PAL2_DRY_png);
EXTLD(res_DEZ_PAL1_WET_png);
EXTLD(res_DEZ_PAL2_WET_png);
EXTLD(res_DEZ_PALm_DRY_png);
EXTLD(res_DEZ_PALm_WET_png);

// TDZ
EXTLD(res_TDZ_level_lvl);
EXTLD(res_TDZ_act_1_ogg);
EXTLD(res_TDZ_act_2_ogg);
EXTLD(res_TDZ_layout_1_bin);
EXTLD(res_TDZ_layout_2_bin);
EXTLD(res_TDZ_chunks_1_bin);
EXTLD(res_TDZ_chunks_2_bin);
EXTLD(res_TDZ_tiles_0_bin);
EXTLD(res_TDZ_tiles_1_bin);
EXTLD(res_TDZ_tiles_2_bin);
EXTLD(res_TDZ_rings_1_bin);
EXTLD(res_TDZ_rings_2_bin);
EXTLD(res_TDZ_objects_1_bin);
EXTLD(res_TDZ_objects_2_bin);
EXTLD(res_TDZ_PAL1_DRY_png);
EXTLD(res_TDZ_PAL2_DRY_png);
EXTLD(res_TDZ_PAL1_WET_png);
EXTLD(res_TDZ_PAL2_WET_png);
EXTLD(res_TDZ_PALm_DRY_png);
EXTLD(res_TDZ_PALm_WET_png);

// Special Stage
EXTLD(res_BlueBalls_BG_png);
EXTLD(res_BlueBalls_SpecialStage4x4_png);
EXTLD(res_BlueBalls_Arrow_png);
EXTLD(res_BlueBalls_Ball8x3_png);
EXTLD(res_BlueBalls_ChaosEmerald8x2_png);
EXTLD(res_BlueBalls_HUDPieceCenter_png);
EXTLD(res_BlueBalls_HUDPieceEnds8_png);
EXTLD(res_BlueBalls_HUDPieceNumbers10_png);
EXTLD(res_BlueBalls_Ring8x7_png);
EXTLD(res_BlueBalls_Sonic8_png);
EXTLD(res_BlueBalls_PAL_MAIN_png);
EXTLD(snd_BGM_SpecialStage_ogg);

EXTLD(res_levelHCZ1FS_bin);
EXTLD(res_levelHCZ1VS_bin);
EXTLD(res_levelHCZ1VSog_bin);
EXTLD(res_LoadingSonic4_png);
EXTLD(res_Menu_Back2_png);
EXTLD(res_Menu_BG_png);
EXTLD(res_Menu_Buttons1x5_png);
EXTLD(res_Menu_LevelPreviews5x5_png);
EXTLD(res_Menu_Side_png);
EXTLD(res_Menu_ZoneNameFont26_png);
EXTLD(res_Menu_ZoneNameFont26x2_png);
EXTLD(res_Signs4_png);
EXTLD(res_Title_BG_png);
EXTLD(res_Title_Logo_png);
EXTLD(res_Title_SonicBase_png);
EXTLD(res_Title_SonicEye2_png);
EXTLD(res_Title_SonicFinger2_png);
EXTLD(res_Title_Tails2_png);
EXTLD(res_Title_TouchToStart2_png);
EXTLD(snd_BadnikDestroyed_ogg);
EXTLD(snd_sfx_S3K_39_ogg);
EXTLD(snd_sfx_S3K_3E_ogg);
EXTLD(snd_sfx_S3K_3F_ogg);
EXTLD(snd_sfx_S3K_41_ogg);
EXTLD(snd_sfx_S3K_42_ogg);
EXTLD(snd_sfx_S3K_43_ogg);
EXTLD(snd_sfx_S3K_44_ogg);
EXTLD(snd_sfx_S3K_45_ogg);
EXTLD(snd_sfx_S3K_48_ogg);
EXTLD(snd_sfx_S3K_49_ogg);
EXTLD(snd_sfx_S3K_4A_ogg);
EXTLD(snd_sfx_S3K_4D_ogg);
EXTLD(snd_sfx_S3K_56_ogg);
EXTLD(snd_sfx_S3K_59_ogg);
EXTLD(snd_sfx_S3K_69_ogg);
EXTLD(snd_sfx_S3K_6F_ogg);
EXTLD(snd_sfx_S3K_9F_ogg);
EXTLD(snd_sfx_S3K_A9_ogg);
EXTLD(snd_sfx_S3K_B2_ogg);
EXTLD(snd_sfx_S3K_B3_ogg);
EXTLD(snd_sfx_S3K_BD_ogg);
EXTLD(snd_sfx_S3K_DB_ogg);
EXTLD(snd_BGM_ActCleared_ogg);
EXTLD(snd_BGM_Boss1_ogg);
EXTLD(snd_BGM_Boss2_ogg);
EXTLD(snd_BGM_CNZ1_ogg);
EXTLD(snd_BGM_CNZ2_ogg);
EXTLD(snd_BGM_DataSelect_ogg);
EXTLD(snd_BGM_Drowning_ogg);
EXTLD(snd_BGM_HCZ1_ogg);
EXTLD(snd_BGM_HCZ2_ogg);
EXTLD(snd_BGM_Invincibility_ogg);
EXTLD(snd_BGM_LifeGet_ogg);
EXTLD(snd_BGM_SEGA_ogg);
EXTLD(snd_BGM_SpecialStage_ogg);
EXTLD(snd_BGM_TitleScreen_ogg);
EXTLD(snd_BIGLaser_ogg);
EXTLD(snd_BombDrop_ogg);
EXTLD(snd_BossHit_ogg);
EXTLD(snd_Break_ogg);
EXTLD(snd_BreakDeeper_ogg);
EXTLD(snd_BubbleGet_ogg);
EXTLD(snd_Checkpoint_ogg);
EXTLD(snd_C_ChaosEmerald_ogg);
EXTLD(snd_C_ChaosEmeraldContinue_ogg);
EXTLD(snd_C_Drown_ogg);
EXTLD(snd_C_EnemyShot_ogg);
EXTLD(snd_C_IntoLargeRing_ogg);
EXTLD(snd_C_ResultsRings_ogg);
EXTLD(snd_C_RingLoss_ogg);
EXTLD(snd_C_ShortBeep_ogg);
EXTLD(snd_C_TailsFly_ogg);
EXTLD(snd_C_ToBonusStage_ogg);
EXTLD(snd_C_ToSpecialStage_ogg);
EXTLD(snd_Die_ogg);
EXTLD(snd_ExtraLife_ogg);
EXTLD(snd_Jump_ogg);
EXTLD(snd_Ring_ogg);
EXTLD(snd_Roll_ogg);
EXTLD(snd_ShieldBounce_ogg);
EXTLD(snd_ShieldBubble_ogg);
EXTLD(snd_ShieldEJump_ogg);
EXTLD(snd_ShieldElectric_ogg);
EXTLD(snd_ShieldFire_ogg);
EXTLD(snd_Skid_ogg);
EXTLD(snd_Spiked_ogg);
EXTLD(snd_SpikeHurt_ogg);
EXTLD(snd_SpinRev_ogg);
EXTLD(snd_Spring_ogg);


#endif /* RESOURCES_H */
