#ifndef S3_LEVELSCENE_H
#define S3_LEVELSCENE_H

struct TileCfg {
    unsigned char Collision[16];
    bool HasCollision[16];
    bool IsCeiling;
    unsigned char Config[5];
};
struct ScrollingInfo {
    int X;
    int Y;
    char Byte1;
    char Byte2;
};
struct ScrollingIndex {
    int Index;
    int Size;
};
struct Layer {
    int Width = 0;
    int Height = 0;
    short Tiles[0xC0000];

    char Name[50];

    int InfoCount;
    int ScrollIndexCount;
    ScrollingInfo Info[0x40];
    ScrollingIndex ScrollIndexes[0x1000];
};

struct ObjectDefinition {

};

struct PlaneSwitch {
    float X;
    float Y;
    int Flags;
    int Size;
    int Angle;
    bool OnPath;
};

struct SceneData {
    short     layerCount;
    Layer     layers[10];
    TileCfg   tiles1[0x400];
    TileCfg   tiles2[0x400];
    ITexture* tileTex;

    short            objectDefinitionCount;
    ObjectDefinition objectDefinitions[0xFF];

    int       cameraLayer;
};

#endif /* S3_LEVELSCENE_H */
