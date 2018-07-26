package CodeEditor;

import javafx.scene.paint.Color;

public class PaletteTone implements ImageFilter {
    byte[] bPalette = null;
    byte[] bPalette2 = null;

    public PaletteTone(byte[] bP, byte[] bP2) {
        bPalette = bP;
        bPalette2 = bP2;
    }

    @Override
    public int[] filter(int imageWidth, int imageHeight, int[] inPixels) {
        int[] filteredPixels = new int[inPixels.length];

        for(int index = 0; index < inPixels.length; index++) {
            int argb = inPixels[index];
            int a = (argb >> 24) & 0xff;
            int r = (argb >> 16) & 0xff;
            int g = (argb >> 8) & 0xff;
            int b = argb & 0xff;

            int x = 0;
            int y = 0;

            if (r > 0) {
                x = 0;
                y = r / 16;
            }
            else if (g > 0) {
                x = 1;
                y = g / 16;
            }
            else if (b > 0) {
                x = 2;
                y = b / 16;
            }

            int i = (x * 16 + y) * 2;
            r = (bPalette[i + 1] & 0xF) * 16 + (bPalette[i + 1] & 0xF);
            g = (bPalette[i + 1] & 0xF0);
            b = (bPalette[i] & 0xFF) * 16 + (bPalette[i] & 0xFF);

            Color c = Color.rgb(r, g, b, a / 255.0);
            filteredPixels[index] = ((int)(c.getOpacity() *  255.0) << 24) | ((int)(c.getRed() *  255.0) << 16) | ((int)(c.getGreen() *  255.0) << 8) | (int)(c.getBlue() *  255.0);
        }
        return filteredPixels;
    }
}
