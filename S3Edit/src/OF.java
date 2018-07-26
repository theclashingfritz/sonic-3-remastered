import javafx.scene.canvas.GraphicsContext;
import javafx.scene.transform.Affine;

import java.awt.geom.AffineTransform;

/**
 * Created by Justin on 9/5/2017.
 */
public class OF {
    public static class Monitor extends ObjectFact {
        public Monitor() {
            name = "Monitor";

            w = 32;
            h = 32;

            definedSubtypes.put(0x0, "Static");
            definedSubtypes.put(0x1, "1-UP");
            definedSubtypes.put(0x2, "Robotnik");
            definedSubtypes.put(0x3, "10 Rings");
            definedSubtypes.put(0x4, "Speed Shoes");
            definedSubtypes.put(0x5, "Fire Shield");
            definedSubtypes.put(0x6, "Lightning Shield");
            definedSubtypes.put(0x7, "Bubble Shield");
            definedSubtypes.put(0x8, "Invincibility");
            definedSubtypes.put(0x9, "S monitor");
            definedSubtypes.put(0xA, "Broken");

            common = true;
        }
        @Override public void Render(GraphicsContext gc, S3Edit.Obj obj, int vX, int vY) {
            if (image != null) {
                int g = obj.SubType;
                if (g == 0) {
                    g = 0;
                }
                else if (g == 1) {
                    g = 1;
                }
                else if (g >= 2) {
                    g++;
                }
                int x = g % 8;
                int y = g / 8;
                gc.drawImage(image, x * 32, y * 32, 32, 32, obj.X - 16 - vX, obj.Y - 16 - vY, 32, 32);
            }
        }
    }
    public static class Spring extends ObjectFact {
        public Spring() {
            w = 32;
            h = 32;

            definedSubtypes.put(0x0, "Red");
            definedSubtypes.put(0x2, "Yellow");

            definedSubtypes.put(0x100, "Up");
            definedSubtypes.put(0x10, "Side");
            definedSubtypes.put(0x20, "Down");
            definedSubtypes.put(0x30, "Diagonal Up");
            definedSubtypes.put(0x40, "Diagonal Down");

            common = true;
        }

        public int getW(S3Edit.Obj obj) {
            int g = (obj.SubType & 0xF0) / 0x10;
            int r = 0;
            if (g == 0)
                r = 0;
            else if (g == 1)
                r = 90;
            else if (g == 2)
                r = 180;
            else if (g == 3)
                r = -45;
            else if (g == 4)
                r = -45 - 180;
            if (r % 180 == 0)
                return 32;
            return 16;
        }
        public int getH(S3Edit.Obj obj) {
            int g = (obj.SubType & 0xF0) / 0x10;
            int r = 0;
            if (g == 0)
                r = 0;
            else if (g == 1)
                r = 90;
            else if (g == 2)
                r = 180;
            else if (g == 3)
                r = -45;
            else if (g == 4)
                r = -45 - 180;
            if (r % 180 == 0)
                return 16;
            return 32;
        }

        @Override public void Render(GraphicsContext gc, S3Edit.Obj obj, int vX, int vY) {
            if (image != null) {
                int g = (obj.SubType & 0xF0) / 0x10;
                int r = 0;
                if (g == 0)
                    r = 0;
                else if (g == 1)
                    r = 90;
                else if (g == 2)
                    r = 180;
                else if (g == 3)
                    r = -45;
                else if (g == 4)
                    r = -45 - 180;
                g = 0;
                int x = g % 3;
                int y = g / 3;

                if (obj.DoHorizontalFlip)
                    r = -r;

                gc.save();
                gc.translate(obj.X - vX, obj.Y - vY);
                gc.rotate(r);
                gc.drawImage(image, x * 32, y * 48, 32, 48, -16, -24, 32, 48);
                gc.restore();
            }
        }
    }
    public static class Spikes extends ObjectFact {
        public Spikes() {
            w = 0;
            h = 32;

            definedSubtypes.put(0x0, "Static");
            definedSubtypes.put(0x1, "In and Out (Horizontal)");
            definedSubtypes.put(0x2, "In and Out (Vertical)");
            definedSubtypes.put(0x3, "Pushable");

            common = true;
        }

        public int getW(S3Edit.Obj obj) {
            int o = ((obj.SubType & 0xF0) / 0x10) & 0x4;
            int s = ((obj.SubType & 0xF0) / 0x10) & 0x3;
            int r = 0;
            if (o == 4)
                r = -90;
            if (obj.DoHorizontalFlip)
                r = -r;
            if (r % 180 != 0)
                return 32;
            return 32 * s + 32;
        }
        public int getH(S3Edit.Obj obj) {
            int o = ((obj.SubType & 0xF0) / 0x10) & 0x4;
            int s = ((obj.SubType & 0xF0) / 0x10) & 0x3;
            int r = 0;
            if (o == 4)
                r = -90;
            if (obj.DoHorizontalFlip)
                r = -r;
            if (r % 180 == 0)
                return 32;
            return 32 * s + 32;
        }

        @Override public void Render(GraphicsContext gc, S3Edit.Obj obj, int vX, int vY) {
            if (image != null) {
                int o = ((obj.SubType & 0xF0) / 0x10) & 0x4;
                int s = ((obj.SubType & 0xF0) / 0x10) & 0x3;
                //int t = (obj.SubType & 0xF) / 0x1;
                int r = 0;
                if (o == 4)
                    r = -90;
                if (obj.DoHorizontalFlip)
                    r = -r;

                w = 32 * s + 32;

                gc.save();
                gc.translate(obj.X - vX, obj.Y - vY);
                gc.rotate(r);
                gc.scale(1.0, obj.DoVerticalFlip ? -1.0 : 1.0);
                for (int i = 0; i < s + 1; i++) {
                    gc.drawImage(image, 0, 0, 32, 32, -w / 2 + 32 * i, -16, 32, 32);
                }
                gc.restore();
            }
        }
    }
}
