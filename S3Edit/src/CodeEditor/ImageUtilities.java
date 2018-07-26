package CodeEditor;

import javafx.scene.image.Image;
import javafx.scene.image.*;

public class ImageUtilities {
    public static Image filter(Image image, ImageFilter filter) {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        WritableImage filteredImage = new WritableImage(width, height);
        int[] inPixels = getRGB(image);
        int[] outPixels = filter.filter(width, height, inPixels);

        setRGB(filteredImage, 0, 0, width, height, outPixels);
        return filteredImage;
    }

    public static void setRGB(WritableImage image, int[] rgb) {
        setRGB(image, 0, 0, (int) image.getWidth(), (int) image.getHeight(), rgb);
    }

    public static int[] getRGB(Image image) {
        return getRGB(image, 0, 0, (int) image.getWidth(), (int) image.getHeight());
    }

    public static void setRGB(WritableImage image, int x, int y, int width, int height, int[] rgb) {
        PixelWriter writer = image.getPixelWriter();
        writer.setPixels(x, y, width, height, PixelFormat.getIntArgbInstance(), rgb, 0, width);
    }

    public static int[] getRGB(Image image, int x, int y, int width, int height) {
        int[] rgb = new int[width * height];
        PixelReader reader = image.getPixelReader();
        reader.getPixels(x, y, width, height, PixelFormat.getIntArgbInstance(), rgb, 0, width);
        return rgb;
    }
}