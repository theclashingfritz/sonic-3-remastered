package CodeEditor;

import javafx.scene.image.Image;

public interface ImageFilter {

    int[] filter(int imageWidth, int imageHeight, int[] inPixels);

}