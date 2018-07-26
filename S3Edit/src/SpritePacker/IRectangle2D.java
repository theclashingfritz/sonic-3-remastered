package SpritePacker;

import javafx.geometry.Rectangle2D;

/**
 * Created by justin on 1/7/18.
 */
public class IRectangle2D {
    public double X;
    public double Y;
    public double W;
    public double H;
    public int OffX;
    public int OffY;
    public int OffW;
    public int OffH;
    public IRectangle2D(double x, double y, double w, double h) {
        X = x;
        Y = y;
        W = w;
        H = h;
        OffX = 0;
        OffY = 0;
        OffW = (int)w;
        OffH = (int)h;
    }
    public IRectangle2D(double x, double y, double w, double h, int offX, int offY) {
        X = x;
        Y = y;
        W = w;
        H = h;
        OffX = offX;
        OffY = offY;
        OffW = (int)w;
        OffH = (int)h;
    }
    public IRectangle2D(double x, double y, double w, double h, int offX, int offY, int offW, int offH) {
        X = x;
        Y = y;
        W = w;
        H = h;
        OffX = offX;
        OffY = offY;
        OffW = offW;
        OffH = offH;
    }

    public double getMinX() {
        return X;
    }
    public double getMinY() {
        return Y;
    }
    public double getMaxX() {
        return X + W;
    }
    public double getMaxY() {
        return Y + H;
    }
    public double getWidth() {
        return W;
    }
    public double getHeight() {
        return H;
    }
    public String toString() {
        return "X: " + X + " Y: " + Y + " W: " + W + " H: " + H + " OffX: " + OffX + " OffY: " + OffY + " OffW: " + OffW + " OffH: " + OffH;
    }
}
