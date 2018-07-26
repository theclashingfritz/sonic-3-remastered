#ifndef IGRAPHICS_H
#define IGRAPHICS_H

#include "IStandardHeaders.h"
#include "ITexture.h"

#define IVertexLength 10

struct IGraphicState {
    GLfloat X;
    GLfloat Y;
    GLfloat Z;
    GLfloat ScaleX;
    GLfloat ScaleY;
    GLfloat ScaleZ;
    GLfloat RotateX;
    GLfloat RotateY;
    GLfloat RotateZ;
    GLfloat PaletteShift;
    GLuint  CurrentShader;
};
struct ISpriteBatchVertex {
    // Position Information
    GLfloat X;
    GLfloat Y;
    GLfloat Z;
    // Palette Information
    GLfloat PaletteShift;
    // Texture Information
    GLfloat U;
    GLfloat V;
    // Color Information
    GLfloat R;
    GLfloat G;
    GLfloat B;
    GLfloat A;
};
class  ISpriteBatch {
public:
    ISpriteBatchVertex              Vertex[18000];
    int                             VertexCount;
    IGraphicState                   State[3000];
    int                             StateCount;
    ITexture*                       Texture = NULL;
    ITexture*                       Palette1 = NULL;
    ITexture*                       Palette2 = NULL;
    GLuint                          Buffer; // VBO is used to store the Vertex data later on, so don't delete these here.
    bool                            SaveBuffer = false;
    bool                            BufferSaved = false;
    float                           OffsetX = 0;
    float                           OffsetY = 0;
    float                           OffsetZ = 0;
};
struct IColorStruct {
    float R;
    float G;
    float B;
    float A;
};
class IColor {
public:
    float R;
    float G;
    float B;
    float A;
    IColor(float r, float g, float b, float a) {
        R = r;
        G = g;
        B = b;
        A = a;
    }
    IColor(float r, float g, float b) {
        R = r;
        G = g;
        B = b;
    }
};

class IGraphics {
public://private:
    IGraphicState                   GraphicState;
    IGraphicState                   StoredGraphicState;
    ISpriteBatch*                   SpriteBatches[30];
    int                             SpriteBatchesSize = 0;
    ISpriteBatch*                   RectangleBatch = new ISpriteBatch;
    ISpriteBatch*                   VideoBatch = new ISpriteBatch;
    float                           ZSmash = 5;

    GLint                           Loc_Position;
    GLint                           Loc_TexCoord;
    GLint                           Loc_Texture;
    GLint                           Loc_Translate;
    GLint                           Loc_Rotate;
    GLint                           Loc_Scale;
    GLint                           Loc_Color;
    GLint                           Loc_VColor;
    GLint                           Loc_UseTex;
    GLint                           Loc_Data;
    GLint                           Loc_Palette1;
    GLint                           Loc_Palette2;
    GLint                           Loc_Palette;
    GLint                           Loc_PaletteLineSize;
    GLint                           Loc_FadeToWhite;
    GLint                           Loc_Fade;

    GLuint                          CurrentShader;

    float                           Blend[4];
    bool                            BlendOverride = false;

    float                           Fade = 0.0f;

    ITexture*                       NextPalette1 = NULL;
    ITexture*                       NextPalette2 = NULL;
    GLuint                          DefaultPalette1;
    GLuint                          DefaultPalette2;
//public:
    ISpriteBatch*                   LastSpriteBatch = NULL;
    bool                            SaveNext = false;
    IGraphics() {
        GraphicState = IGraphicState { 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 };
        Blend[0] = 1.0f;
        Blend[1] = 1.0f;
        Blend[2] = 1.0f;
        Blend[3] = 1.0f;
    }
    // Draw State Manipulation
    void Save();
    void Translate(float X, float Y);
    void Translate(float X, float Y, float Z);
    void Scale(float X, float Y);
    void Scale(float X, float Y, float Z);
    void Rotate(float Angle);
    void Rotate(float X, float Y, float Z);
    void PaletteShift(int Shift);
    void Restore();
    void ChangeShader(GLuint Shader);
    void SetBlend(float R, float G, float B, float A);
    void SetFade(float F);
    void SetPalette(IColorStruct* Array, int ArraySize, int LineSize);
    void SetNextSpriteDrawPalette(ITexture* Texture1, ITexture* Texture2);

    // Drawing
    void DrawText(IFont *Font, const char* Text, float X, float Y, int Alignment);
    void DrawText(IFont *Font, const char* Text, float X, float Y, float Z, int Alignment);
    void DrawText(ITexture *Texture, const char* Text, float X, float Y, float Z, int Alignment);
    void DrawSprite(ISprite *Sprite, float X, float Y, float Z, int Frame, int Alignment);
    void DrawSprite(ISprite *Sprite, float X, float Y, float Z, float Rot, int Frame, int Alignment);
    void DrawSprite(ISprite *Sprite, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, int Frame, int Alignment);
    void DrawSprite(ISprite *Sprite, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, float Rot, int Frame, int Alignment);
    void DrawTexture(ITexture *Texture, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, float W, float H);
    void DrawVideo(IVideo* Video);
    void DrawRectangle(float X, float Y, float Z, float W, float H, IColor Color);

    void FinishFrame();
};

#endif // IGRAPHICS_H
