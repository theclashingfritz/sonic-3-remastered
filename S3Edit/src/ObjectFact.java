import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by Justin on 9/5/2017.
 */
public class ObjectFact {
    public Image image;
    public String imageSrc = "";
    public String name = "";
    public int imgW = 1;
    public int imgH = 1;
    public Map<Integer, String> definedSubtypes = new HashMap<>();
    public int w = 32;
    public int h = 32;
    public int d = 0;
    public int offX = 0;
    public int offY = 0;
    public boolean needBubbleFx = false;
    public int needExplosion = 0;
    public boolean common = false;

    public int getW(S3Edit.Obj obj) { return w; }
    public int getH(S3Edit.Obj obj) { return h; }
    public void setName(String name) { this.name = name; }

    public void Render(GraphicsContext gc, S3Edit.Obj obj, int vX, int vY) {
        if (image != null) {
            int x = d % 8;
            int y = d / 8;
            int ww = (int)(image.getWidth() / imgW);
            int hh = (int)(image.getHeight() / imgH);
            gc.save();
            gc.translate(obj.X - vX + offX, obj.Y - vY + offY);

            gc.scale(obj.DoHorizontalFlip ? -1 : 1, obj.DoVerticalFlip ? -1 : 1);

            gc.drawImage(image, x * ww, y * hh, ww, hh, -ww / 2, -hh / 2, ww, hh);

            gc.restore();
        }
    }
}