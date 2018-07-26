#include "IGraphics.h"

void IGraphics::ChangeShader(GLuint Shader) {
    CurrentShader = Shader;

    Loc_Position = glGetAttribLocation(CurrentShader, "v_position");
    Loc_TexCoord = glGetAttribLocation(CurrentShader, "v_texcoord");
    Loc_VColor   = glGetAttribLocation(CurrentShader, "v_color");
    Loc_Texture  = glGetUniformLocation(CurrentShader, "u_sampler");

    Loc_Translate = glGetUniformLocation(CurrentShader, "u_translate");
    Loc_Rotate = glGetUniformLocation(CurrentShader, "u_rotate");
    Loc_Scale = glGetUniformLocation(CurrentShader, "u_scale");

    Loc_Color = glGetUniformLocation(CurrentShader, "u_color");
    Loc_UseTex = glGetUniformLocation(CurrentShader, "u_useTex");

    Loc_Data = glGetUniformLocation(CurrentShader, "u_data");

    Loc_Palette = glGetUniformLocation(CurrentShader, "u_palette");
    Loc_Palette1 = glGetUniformLocation(CurrentShader, "u_palette1");
    Loc_Palette2 = glGetUniformLocation(CurrentShader, "u_palette2");
    Loc_PaletteLineSize = glGetUniformLocation(CurrentShader, "u_paletteLineSize");

    Loc_FadeToWhite = glGetUniformLocation(CurrentShader, "u_fadeToWhite");
    Loc_Fade = glGetUniformLocation(CurrentShader, "u_fade");
}
void IGraphics::Save() {
    StoredGraphicState = GraphicState;
}
void IGraphics::Translate(float X, float Y) {
    Translate(X, Y, 0.f);
}
void IGraphics::Translate(float X, float Y, float Z) {
    GraphicState.X = X;
    GraphicState.Y = Y;
    GraphicState.Z = Z;
}
void IGraphics::Scale(float X, float Y) {
    Scale(X, Y, 1.f);
}
void IGraphics::Scale(float X, float Y, float Z) {
    GraphicState.ScaleX = X;
    GraphicState.ScaleY = Y;
    GraphicState.ScaleZ = Z;
}
void IGraphics::Rotate(float Angle) {
    Rotate(0.f, 0.f, Angle);
}
void IGraphics::Rotate(float X, float Y, float Z) {
    GraphicState.RotateX = X;
    GraphicState.RotateY = Y;
    GraphicState.RotateZ = Z;
}
void IGraphics::PaletteShift(int Shift) {
    GraphicState.PaletteShift = Shift;
}
void IGraphics::Restore() {
    GraphicState = StoredGraphicState;
}
void IGraphics::SetBlend(float R, float G, float B, float A) {
    if (B == -2) {
        BlendOverride = true;
    }
    if (B == -3) {
        BlendOverride = false;
    }
    Blend[0] = R;
    Blend[1] = G;
    if (!BlendOverride)
        Blend[2] = B;
    else
        Blend[2] = 1;
    Blend[3] = A;
}
void IGraphics::SetFade(float F) {
    Fade = F;
}
void IGraphics::SetPalette(IColorStruct* Array, int ArraySize, int LineSize) {
    glUniform1i(Loc_PaletteLineSize, LineSize);
    glUniform4fv(Loc_Palette, ArraySize, (GLfloat*)Array);
}
void IGraphics::SetNextSpriteDrawPalette(ITexture* Texture1, ITexture* Texture2) {
    NextPalette1 = Texture1;
    NextPalette2 = Texture2;
}

// Drawing
//void IGraphics::DrawText(IFont *Font, const char* Text, float X, float Y, int Alignment);
//void IGraphics::DrawText(IFont *Font, const char* Text, float X, float Y, float Z, int Alignment);
void IGraphics::DrawText(ITexture *Texture, const char* Text, float X, float Y, float Z, int Alignment) {

}
void IGraphics::DrawSprite(ISprite *Sprite, float X, float Y, float Z, int Frame, int Alignment) {
    DrawSprite(Sprite, X, Y, Z, 0, Frame, Alignment);
}
void IGraphics::DrawSprite(ISprite *Sprite, float X, float Y, float Z, float Rot, int Frame, int Alignment) {
    if (Sprite == NULL)
        return;

    ISpriteBatch* SelectedSpriteBatch = NULL;

    if (LastSpriteBatch != NULL) {
        if (LastSpriteBatch->Texture == Sprite->Texture && LastSpriteBatch->Palette1 == NextPalette1 && LastSpriteBatch->Palette2 == NextPalette2) {
            SelectedSpriteBatch = LastSpriteBatch;
        }
    }
    if (SelectedSpriteBatch == NULL) {
        for (int s = SpriteBatchesSize - 1; s >= 0; s--) {
            ISpriteBatch* i = SpriteBatches[s];
            if (i->Texture == Sprite->Texture && i->Palette1 == NextPalette1 && i->Palette2 == NextPalette2) {
                SelectedSpriteBatch = i;
                break;
            }
        }
    }
    if (SelectedSpriteBatch == NULL) {
        ISpriteBatch* newsprite = new ISpriteBatch();
        newsprite->Texture = Sprite->Texture;
        if (NextPalette1 != NULL) {
            newsprite->Palette1 = NextPalette1;
            NextPalette1 = NULL;
        }
        if (NextPalette2 != NULL) {
            newsprite->Palette2 = NextPalette2;
            NextPalette2 = NULL;
        }
        SpriteBatches[SpriteBatchesSize++] = newsprite;
        SelectedSpriteBatch = newsprite;
    }
    LastSpriteBatch = SelectedSpriteBatch;

    int frameWidth = Sprite->Frame[Frame].FullW;
    int frameHeight = Sprite->Frame[Frame].FullH;

    float W = Sprite->Frame[Frame].FullW * GraphicState.ScaleX;//Sprite->Frame[Frame].OffW * GraphicState.ScaleX;
    float H = Sprite->Frame[Frame].FullH * GraphicState.ScaleY;//Sprite->Frame[Frame].OffH * GraphicState.ScaleY;

    int halign = 0;
    int valign = 0;

    if ((Alignment & IE::LeftAlign) == IE::LeftAlign)
        halign = -1;
    else if ((Alignment & IE::RightAlign) == IE::RightAlign)
        halign = 1;
    if ((Alignment & IE::TopAlign) == IE::TopAlign)
        valign = -1;
    else if ((Alignment & IE::BottomAlign) == IE::BottomAlign)
        valign = 1;

    float rot = Rot;
    float Center_X = X;
    float Center_Y = Y;
    float Angle, cos_, sin_;
    float Offset_X, Offset_Y;
    float AALen, ABLen, BALen, BBLen;
    IPoint AA, BA, AB, BB;

    float SprX = (Sprite->Frame[Frame].OffX - (Sprite->Frame[Frame].OffW / 2.f - frameWidth / 2.f)) * GraphicState.ScaleX * (halign + 1);
    float SprY = (Sprite->Frame[Frame].OffY - (Sprite->Frame[Frame].OffH / 2.f - frameHeight / 2.f)) * GraphicState.ScaleY * (valign + 1);

    if (halign == -1)
        SprX += Sprite->Frame[Frame].OffX;
    if (valign == -1)
        SprY += Sprite->Frame[Frame].OffY;

    if (rot != 0) {
        Offset_X = (halign + 1) * -W / 2 + SprX;
        Offset_Y = (valign + 1) * -H / 2 + SprY;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        AALen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        AA = IPoint { std::round(cos_ * AALen + Center_X), std::round(sin_ * AALen + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + W + SprX;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        BALen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        BA = IPoint { std::round(cos_ * BALen + Center_X), std::round(sin_ * BALen + Center_Y) };

        Offset_Y = (valign + 1) * -H / 2 + H + SprY;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        BBLen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        BB = IPoint { std::round(cos_ * BBLen + Center_X), std::round(sin_ * BBLen + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + SprX;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        ABLen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        AB = IPoint { std::round(cos_ * ABLen + Center_X), std::round(sin_ * ABLen + Center_Y) };
    }
    else {
        Offset_X = (halign + 1) * -W / 2 + SprX;
        Offset_Y = (valign + 1) * -H / 2 + SprY;
        AA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + W + SprX;
        BA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_Y = (valign + 1) * -H / 2 + H + SprY;
        BB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + SprX;
        AB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };
    }

    float oX1 = Sprite->Frame[Frame].U_left;
    float oX2 = Sprite->Frame[Frame].U_right;
    float oY1 = Sprite->Frame[Frame].V_top;
    float oY2 = Sprite->Frame[Frame].V_bottom;

    /*SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_left,  Sprite->Frame[Frame].V_bottom, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_right, Sprite->Frame[Frame].V_bottom, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_right, Sprite->Frame[Frame].V_top, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_left,  Sprite->Frame[Frame].V_top, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_left,  Sprite->Frame[Frame].V_bottom, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, Sprite->Frame[Frame].U_right, Sprite->Frame[Frame].V_top, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->State[SelectedSpriteBatch->StateCount++] = (GraphicState);*/
    ISpriteBatchVertex ABX = ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY2, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex BBX = ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY2, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex AAX = ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY1, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex BAX = ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY1, Blend[0], Blend[1], Blend[2], Blend[3] };

    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 0] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 1] = BBX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 2] = BAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 3] = AAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 4] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 5] = BAX;
    SelectedSpriteBatch->VertexCount += 6;
    SelectedSpriteBatch->State[SelectedSpriteBatch->StateCount++] = GraphicState;
}
void IGraphics::DrawSprite(ISprite *Sprite, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, int Frame, int Alignment) {
    DrawSprite(Sprite, SourceX, SourceY, SourceW, SourceH, X, Y, Z, 0, Frame, Alignment);
}
void IGraphics::DrawSprite(ISprite *Sprite, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, float Rot, int Frame, int Alignment) {
    if (Sprite == NULL)
        return;

    ISpriteBatch* SelectedSpriteBatch = NULL;

    if (LastSpriteBatch != NULL) {
        if (LastSpriteBatch->Texture == Sprite->Texture && LastSpriteBatch->Palette1 == NextPalette1 && LastSpriteBatch->Palette2 == NextPalette2) {
            SelectedSpriteBatch = LastSpriteBatch;
        }
    }
    if (SelectedSpriteBatch == NULL) {
        for (int s = SpriteBatchesSize - 1; s >= 0; s--) {
            ISpriteBatch* i = SpriteBatches[s];
            if (i->Texture == Sprite->Texture && i->Palette1 == NextPalette1 && i->Palette2 == NextPalette2) {
                SelectedSpriteBatch = i;
                break;
            }
        }
    }
    if (SelectedSpriteBatch == NULL) {
        ISpriteBatch* newsprite = new ISpriteBatch();
        newsprite->Texture = Sprite->Texture;
        if (NextPalette1 != NULL) {
            newsprite->Palette1 = NextPalette1;
            NextPalette1 = NULL;
        }
        if (NextPalette2 != NULL) {
            newsprite->Palette2 = NextPalette2;
            NextPalette2 = NULL;
        }
        SpriteBatches[SpriteBatchesSize++] = newsprite;
        SelectedSpriteBatch = newsprite;
    }
    LastSpriteBatch = SelectedSpriteBatch;

    float frameX = Sprite->Frame[Frame].OffX;
    float frameY = Sprite->Frame[Frame].OffY;
    float frameWidth = (int)((Sprite->Frame[Frame].U_right - Sprite->Frame[Frame].U_left) * Sprite->Texture->Width);
    float frameHeight = (int)((Sprite->Frame[Frame].V_bottom - Sprite->Frame[Frame].V_top) * Sprite->Texture->Height);

    SourceW = std::min(SourceW, (frameWidth + Sprite->Frame[Frame].OffX) - SourceX);
    SourceH = std::min(SourceH, (frameHeight + Sprite->Frame[Frame].OffY) - SourceY);

    float W = SourceW * GraphicState.ScaleX;//Sprite->Frame[Frame].OffW * GraphicState.ScaleX;
    float H = SourceH * GraphicState.ScaleY;//Sprite->Frame[Frame].OffH * GraphicState.ScaleY;

    int halign = 0;
    int valign = 0;

    if ((Alignment & IE::LeftAlign) == IE::LeftAlign) {
        halign = -1;
    }
    if ((Alignment & IE::RightAlign) == IE::RightAlign) {
        halign = 1;
    }
    if ((Alignment & IE::TopAlign) == IE::TopAlign) {
        valign = -1;
    }
    if ((Alignment & IE::BottomAlign) == IE::BottomAlign) {
        valign = 1;
    }

    float SprX = (Sprite->Frame[Frame].OffX - (Sprite->Frame[Frame].OffW / 2.f - frameWidth / 2.f)) * GraphicState.ScaleX * (halign + 1);
    float SprY = (Sprite->Frame[Frame].OffY - (Sprite->Frame[Frame].OffH / 2.f - frameHeight / 2.f)) * GraphicState.ScaleY * (valign + 1);

    float rot = Rot;
    float Center_X = X;
    float Center_Y = Y;
    float Angle, cos_, sin_;
    float Offset_X, Offset_Y;
    float AALen, ABLen, BALen, BBLen;
    IPoint AA, BA, AB, BB;

    if (rot != 0) {
        Offset_X = (halign + 1) * -W / 2 + SprX;
        Offset_Y = (valign + 1) * -H / 2 + SprY;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        AALen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        AA = IPoint { std::round(cos_ * AALen + Center_X), std::round(sin_ * AALen + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + W + SprX;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        BALen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        BA = IPoint { std::round(cos_ * BALen + Center_X), std::round(sin_ * BALen + Center_Y) };

        Offset_Y = (valign + 1) * -H / 2 + H + SprY;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        BBLen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        BB = IPoint { std::round(cos_ * BBLen + Center_X), std::round(sin_ * BBLen + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + SprX;
        Angle = std::atan2(-Offset_Y, Offset_X) + rot;
        cos_ = std::cos(Angle);
        sin_ = -std::sin(Angle);
        ABLen = std::sqrt(Offset_X * Offset_X + Offset_Y * Offset_Y);
        AB = IPoint { std::round(cos_ * ABLen + Center_X), std::round(sin_ * ABLen + Center_Y) };
    }
    else {
        Offset_X = (halign + 1) * -W / 2 + SprX;
        Offset_Y = (valign + 1) * -H / 2 + SprY;
        AA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + W + SprX;
        BA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_Y = (valign + 1) * -H / 2 + H + SprY;
        BB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

        Offset_X = (halign + 1) * -W / 2 + SprX;
        AB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };
    }

    float oX1 = Sprite->Frame[Frame].U_left + (SourceX - frameX) / Sprite->Texture->Width;
    float oX2 = Sprite->Frame[Frame].U_left + (SourceX - frameX + SourceW) / Sprite->Texture->Width;
    float oY1 = Sprite->Frame[Frame].V_top + (SourceY - frameY) / Sprite->Texture->Height;
    float oY2 = Sprite->Frame[Frame].V_top + (SourceY - frameY + SourceH) / Sprite->Texture->Height;

    /*SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY2, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY2, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY1, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY1, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY2, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY1, Blend[0], Blend[1], Blend[2], Blend[3] });
    SelectedSpriteBatch->State[SelectedSpriteBatch->StateCount++] = (GraphicState);*/
    ISpriteBatchVertex ABX = ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY2, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex BBX = ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY2, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex AAX = ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, oX1, oY1, Blend[0], Blend[1], Blend[2], Blend[3] };
    ISpriteBatchVertex BAX = ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, oX2, oY1, Blend[0], Blend[1], Blend[2], Blend[3] };

    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 0] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 1] = BBX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 2] = BAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 3] = AAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 4] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 5] = BAX;
    SelectedSpriteBatch->VertexCount += 6;
    SelectedSpriteBatch->State[SelectedSpriteBatch->StateCount++] = GraphicState;
}

void IGraphics::DrawTexture(ITexture *Texture, float SourceX, float SourceY, float SourceW, float SourceH, float X, float Y, float Z, float W, float H) {
    if (Texture == NULL)
        return;

    ISpriteBatch* SelectedSpriteBatch = NULL;

    if (LastSpriteBatch != NULL) {
        if (LastSpriteBatch->Texture == Texture) {
            SelectedSpriteBatch = LastSpriteBatch;
        }
    }
    if (SelectedSpriteBatch == NULL) {
        for (int s = SpriteBatchesSize - 1; s >= 0; s--) {
            ISpriteBatch* i = SpriteBatches[s];
            if (i->Texture == Texture) {
                SelectedSpriteBatch = i;
                break;
            }
        }
    }
    if (SelectedSpriteBatch == NULL || SaveNext) {
        ISpriteBatch* newsprite = new ISpriteBatch();
        newsprite->Texture = Texture;
        SpriteBatches[SpriteBatchesSize++] = newsprite;
        SelectedSpriteBatch = newsprite;
        SaveNext = false;
    }
    LastSpriteBatch = SelectedSpriteBatch;

    if (SelectedSpriteBatch->BufferSaved)
        return;

    int _W = std::abs(W) * GraphicState.ScaleX;//Sprite->Frame[Frame].OffW * GraphicState.ScaleX;
    int _H = std::abs(H) * GraphicState.ScaleY;//Sprite->Frame[Frame].OffH * GraphicState.ScaleY;

    float oX1 = SourceX / Texture->Width;
    float oX2 = (SourceX + SourceW) / Texture->Width;
    float oY1 = SourceY / Texture->Height;
    float oY2 = (SourceY + SourceH) / Texture->Height;
    if (W < 0) {
        float temp = oX1;
        oX1 = oX2;
        oX2 = temp;
    }
    if (H < 0) {
        float temp = oY1;
        oY1 = oY2;
        oY2 = temp;
    }

    ISpriteBatchVertex ABX = ISpriteBatchVertex {
        std::floor(X),
        std::floor(Y + _H),
        Z / ZSmash,
        GraphicState.PaletteShift,
        oX1,
        oY2,
        Blend[0],
        Blend[1],
        Blend[2],
        Blend[3] };
    ISpriteBatchVertex BBX = ISpriteBatchVertex {
        std::floor(X + _W),
        std::floor(Y + _H),
        Z / ZSmash,
        GraphicState.PaletteShift,
        oX2,
        oY2,
        Blend[0],
        Blend[1],
        Blend[2],
        Blend[3] };
    ISpriteBatchVertex AAX = ISpriteBatchVertex {
        std::floor(X),
        std::floor(Y),
        Z / ZSmash,
        GraphicState.PaletteShift,
        oX1,
        oY1,
        Blend[0],
        Blend[1],
        Blend[2],
        Blend[3] };
    ISpriteBatchVertex BAX = ISpriteBatchVertex {
        std::floor(X + _W),
        std::floor(Y),
        Z / ZSmash,
        GraphicState.PaletteShift,
        oX2,
        oY1,
        Blend[0],
        Blend[1],
        Blend[2],
        Blend[3] };

    ///*
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 0] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 1] = BBX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 2] = BAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 3] = AAX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 4] = ABX;
    SelectedSpriteBatch->Vertex[SelectedSpriteBatch->VertexCount + 5] = BAX;
    SelectedSpriteBatch->VertexCount += 6;
    //*/
    //SelectedSpriteBatch->State[SelectedSpriteBatch->StateCount++] = GraphicState;
}

void IGraphics::DrawRectangle(float X, float Y, float Z, float W, float H, IColor Color) {
    float Center_X = X;
    float Center_Y = Y;
    float Offset_X, Offset_Y;
    IPoint AA, BA, AB, BB;


    Offset_X = 0;
    Offset_Y = 0;
    AA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_X = W;
    BA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_Y = H;
    BB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_X = 0;
    AB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };


    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->Vertex[RectangleBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
    RectangleBatch->State[RectangleBatch->StateCount++] = (GraphicState);
}

void IGraphics::DrawVideo(IVideo* Video) {
    // create own texture
    float Center_X = 0;
    float Center_Y = 0;
    float Z = -4.999f;
    float Offset_X, Offset_Y;
    IPoint AA, BA, AB, BB;
    IColor Color(1.0f, 1.0f, 1.0f, 1.0f);


    Offset_X = 0;
    Offset_Y = 0;
    AA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_X = 400;
    BA = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_Y = 225;
    BB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    Offset_X = 0;
    AB = IPoint { std::floor(Offset_X + Center_X), std::floor(Offset_Y + Center_Y) };

    VideoBatch->Texture = Video->Texture;
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (BB.X), (BB.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (AA.X), (AA.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (AB.X), (AB.Y), Z / ZSmash, GraphicState.PaletteShift, 0.0f, 1.0f, Color.R, Color.G, Color.B, Color.A });
    VideoBatch->Vertex[VideoBatch->VertexCount++] = (ISpriteBatchVertex { (BA.X), (BA.Y), Z / ZSmash, GraphicState.PaletteShift, 1.0f, 0.0f, Color.R, Color.G, Color.B, Color.A });
}

void IGraphics::FinishFrame() {
    glUniform1i(Loc_UseTex, 1);
    glUniform1f(Loc_Fade, Fade);
    glUniform1i(Loc_FadeToWhite, 0);

    glEnableVertexAttribArray(Loc_Position);
    glEnableVertexAttribArray(Loc_TexCoord);
    glEnableVertexAttribArray(Loc_VColor);

    for (int s = 0; s < SpriteBatchesSize; s++) {
        ISpriteBatch* i = SpriteBatches[s];

        if (!i->BufferSaved) {
            glGenBuffers(1, &i->Buffer);
            glBindBuffer(GL_ARRAY_BUFFER, i->Buffer);
            glBufferData(GL_ARRAY_BUFFER, IVertexLength * i->VertexCount * sizeof(GLfloat), i->Vertex, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (i->SaveBuffer) {
                i->BufferSaved = true;
            }
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, i->Texture->GL_Tex);

        if (i->Palette1) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, i->Palette1->GL_Tex);
        }
        else {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, DefaultPalette1);
        }
        if (i->Palette2) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, i->Palette2->GL_Tex);
        }
        else {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, DefaultPalette2);
        }

        if (i->SaveBuffer) {
            glUniform4f(Loc_Translate, i->OffsetX, i->OffsetY, i->OffsetZ, 0.f);
        }

        glBindBuffer(GL_ARRAY_BUFFER, i->Buffer);

        glVertexAttribPointer(Loc_Position, 4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
        glVertexAttribPointer(Loc_TexCoord, 2, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
        glVertexAttribPointer(Loc_VColor,   4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, i->VertexCount);

        if (i->SaveBuffer) {
            glUniform4f(Loc_Translate, 0.f, 0.f, 0.f, 0.f);
        }

        if (!i->BufferSaved) {
            glDeleteBuffers(1, &i->Buffer);
            i->VertexCount = 0;
            i->StateCount = 0;
        }
    }

    if (true) {
        glUniform1i(Loc_UseTex, 0);

        ISpriteBatch* i = RectangleBatch;

        glGenBuffers(1, &i->Buffer);
        glBindBuffer(GL_ARRAY_BUFFER, i->Buffer);
        glBufferData(GL_ARRAY_BUFFER, IVertexLength * i->VertexCount * sizeof(GLfloat), i->Vertex, GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, i->Buffer);

        glVertexAttribPointer(Loc_Position, 4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
        glVertexAttribPointer(Loc_TexCoord, 2, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
        glVertexAttribPointer(Loc_VColor,   4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, i->VertexCount);

        glDeleteBuffers(1, &i->Buffer);
        i->VertexCount = 0;
        i->StateCount = 0;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glUniform1i(Loc_UseTex, 1);

    if (true) {
        ISpriteBatch* i = VideoBatch;

        if (i->VertexCount > 0) {
            glGenBuffers(1, &i->Buffer);
            glBindBuffer(GL_ARRAY_BUFFER, i->Buffer);
            glBufferData(GL_ARRAY_BUFFER, IVertexLength * i->VertexCount * sizeof(GLfloat), i->Vertex, GL_STATIC_DRAW);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, i->Texture->GL_Tex);

            glVertexAttribPointer(Loc_Position, 4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
            glVertexAttribPointer(Loc_TexCoord, 2, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
            glVertexAttribPointer(Loc_VColor,   4, GL_FLOAT, GL_FALSE, IVertexLength * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));

            glDrawArrays(GL_TRIANGLES, 0, i->VertexCount);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDeleteBuffers(1, &i->Buffer);
        }

        i->VertexCount = 0;
        i->StateCount = 0;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glDisableVertexAttribArray(Loc_Position);
    glDisableVertexAttribArray(Loc_TexCoord);
    glDisableVertexAttribArray(Loc_VColor);

    glBindTexture(GL_TEXTURE_2D, 0);

    glUniform1f(Loc_Fade, 0.0f);
}
