import CodeEditor.ImageUtilities;
import SpritePacker.IRectangle2D;
import SpritePacker.Packer;
import javafx.application.Application;
import javafx.embed.swing.SwingFXUtils;
import javafx.geometry.VPos;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.Priority;
import javafx.scene.paint.Color;
import javafx.scene.text.TextAlignment;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;

/**
 * Created by justin on 1/5/18.
 */
public class SpritePacke extends Application {

    Canvas canvas;
    Scene scene;
    ArrayList<String> sources = new ArrayList<>();
    ArrayList<Image> images = new ArrayList<>();
    ArrayList<Integer> starts = new ArrayList<>();
    ArrayList<Integer> lens = new ArrayList<>();
    ArrayList<Integer> imgIds = new ArrayList<>();
    ArrayList<Integer> ids = new ArrayList<>();
    ArrayList<IRectangle2D> rects = new ArrayList<>();
    ArrayList<IRectangle2D> src_rects = new ArrayList<>();
    SpritePacker.Packer packer;
    int finalWidth = 0;
    int finalHeight = 0;

    public int[] findDimensions(String texName) {
        int num = 1;
        int numH = 1;
        texName = texName.replaceAll("\\.NOCRUSH", "");
        texName = texName.replaceAll("\\.png", "");
        try {
            num = Integer.parseInt(texName.substring(texName.length() - 1));
            texName = texName.substring(0, texName.length() - 1);

            if (!texName.substring(texName.length() - 1).equals("x")) {
                num += Integer.parseInt(texName.substring(texName.length() - 1)) * 10;
                texName = texName.substring(0, texName.length() - 1);
            }

            if (texName.substring(texName.length() - 1).equals("x") && Character.isDigit(texName.charAt(texName.length() - 2))) {
                numH = num;
                texName = texName.substring(0, texName.length() - 1);
            }

            num = Integer.parseInt(texName.substring(texName.length() - 1)) * 1;
            texName = texName.substring(0, texName.length() - 1);

            num += Integer.parseInt(texName.substring(texName.length() - 1)) * 10;
            texName = texName.substring(0, texName.length() - 1);
        } catch (Exception e) { }

        int numG = num;

        if (num > 8) {
            numH = num / 8 + 1;
            num = 8;
        }

        return new int[] { num, numH, numG };
    }
    public String cleanName(String texName) {
        int num = 1;
        int numH = 1;
        texName = texName.replaceAll("\\.NOCRUSH", "");
        texName = texName.replaceAll("\\.png", "");
        try {
            num = Integer.parseInt(texName.substring(texName.length() - 1));
            texName = texName.substring(0, texName.length() - 1);

            if (!texName.substring(texName.length() - 1).equals("x")) {
                num += Integer.parseInt(texName.substring(texName.length() - 1)) * 10;
                texName = texName.substring(0, texName.length() - 1);
            }

            if (texName.substring(texName.length() - 1).equals("x") && Character.isDigit(texName.charAt(texName.length() - 2))) {
                numH = num;
                texName = texName.substring(0, texName.length() - 1);
            }

            num = Integer.parseInt(texName.substring(texName.length() - 1)) * 1;
            texName = texName.substring(0, texName.length() - 1);

            num += Integer.parseInt(texName.substring(texName.length() - 1)) * 10;
            texName = texName.substring(0, texName.length() - 1);
        } catch (Exception e) { }
        while (texName.endsWith("_")) {
            texName = texName.substring(0, texName.length() - 1);
        }
        return texName;
    }

    public static void main(String[] args) {
        /*
        // 1D 18 18 0F 0E 0C 0A 1B 1D 12 20 9E 18 14 9E 11
        // 1D 18 18 0F 0E 0C 0A 1B 1D 12 20 9E 18 14 9E 11 9E 0A 19 18 9E 0E 11 1D 0C 12 1E 1A 00 00 FF 14

        // 9F 0B 18 0B 9E 0B 16 18 1D 1D 0A 0B 12 0F 0E 15 FF 0D 15 0E
        // 9F 0B 18 0B 9E 0B 16 15 1D 1D 0A 0E 12 0F 0E 00 FF 0D 15 20

        String input = "FOOTRACE WITH KOOPA THE QUICK";
        input = "   DONKEY KONG TROPICAL FREEZE"; // 0D 9E 9E 9E 0E 14 17 18 18 14 9E 22 1D 9E 10 17 12 19 18 1B 9E 15 0A 0C 0E 0E 1B 0F 00 FF 0E 23 00 00 00 00
        //input = "   BOB-OMB BATTLEFIELD";
        //input = "DONKEY MAN GOES TOO FAST"; // 14 17 18 0D 16 9E 22 0E 10 9E 17 0A 9E 1C 0E 18 9E 18 18 1D 1D 1C 0A 0F 00 00 00 FF 00 00 00 00

        String output = "";

        input += "\2";
        input += "\1\1\1\1";
        input += "\1\1\1\1";

        for (int batch = 0; batch + 3 < input.length(); batch += 4) {
            for (int i = 3; i >= 0; i--) {
                char letter = input.charAt(batch + i);
                if (letter >= '0' && letter <= '9')
                    output += pad(Integer.toHexString((int)(letter - '0') + 0x00)) + " ";
                if (letter >= 'A' && letter <= 'Z')
                    output += pad(Integer.toHexString((int)(letter - 'A') + 0x0A)) + " ";
                if (letter >= 'a' && letter <= 'z')
                    output += pad(Integer.toHexString((int)(letter - 'a') + 0x24)) + " ";
                if (letter == ' ')
                    output += pad(Integer.toHexString((int)(letter - ' ') + 0x9E)) + " ";
                if (letter == '-')
                    output += pad(Integer.toHexString((int)(letter - '-') + 0x9F)) + " ";
                if (letter == '\1')
                    output += "00" + " ";
                if (letter == '\2')
                    output += "FF" + " ";
            }
        }
        output = output.toUpperCase();
        //output = output.substring(0, (input.length() - 4) * 3);
        System.out.println(output);
        System.out.println(output.length() / 3);
        //*/

        ///*

        String input = "B7 73 00 03 CF FF FF 1F EF FF FF 3F EF 07 E6 47 EF FF FF 7F EF FF F7 9F EF 1F 00 A0 FF 9F 00 00 FF 3F 18 00 FF 7F 00 02 FF FF 06 00";
        input = input.replaceAll(" ", "");

        for (int i = 0; i < input.length(); i += 8) {
            String inni = "";
            inni += input.substring(i + 6, i + 8);
            inni += input.substring(i + 4, i + 6);
            inni += input.substring(i + 2, i + 4);
            inni += input.substring(i + 0, i + 2);

            long num = Long.parseLong(inni, 16);

            String binni = Long.toBinaryString(num);
            binni = ("00000000000000000000000000000000" + binni).substring(binni.length());

            System.out.println("0b" + binni + ",");
        }

        //*/

        //launch(args);
    }

    public static String pad(String str) {
        if (str.length() == 1)
            return "0" + str;
        return str;
    }

    @Override
    public void start(Stage stage) throws Exception {
        int padding = 1;

        String sourceFromText = "";

        if (sourceFromText.isEmpty()) {
            FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("PNG files (.png)", "*.png");
            FileChooser.ExtensionFilter extFilter2 = new FileChooser.ExtensionFilter("SPR files (.spr)", "*.spr");

            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Open Textures For Packing...");
            fileChooser.setInitialDirectory(new File("/Users/justin/Dropbox/sonic3/source/Resource/"));
            fileChooser.getExtensionFilters().add(extFilter);
            fileChooser.getExtensionFilters().add(extFilter2);
            fileChooser.setSelectedExtensionFilter(extFilter);

            ArrayList<File> files = new ArrayList<File>(fileChooser.showOpenMultipleDialog(stage));
            for (int i = 0; i < files.size(); i++) {
                sources.add(files.get(i).getAbsolutePath().replaceAll("\\\\", "/").replaceAll("C:", ""));
                System.out.println(files.get(i).getAbsolutePath().replaceAll("\\\\", "/").replaceAll("C:", ""));
            }
            if (files.size() <= 1) {
                System.out.println("Not enough files selected.");
                System.exit(0);
            }
            //System.exit(0);
        }
        else {
            String[] files = sourceFromText.split("\n");
            for (int i = 0; i < files.length; i++) {
                if (!files[i].isEmpty()) {
                    if (new File(files[i]).exists()) {
                        sources.add(files[i]);
                        System.out.println(files[i]);
                    }
                    else {
                        sources.add(files[i].replaceAll("\\.png", ".NOCRUSH.png"));
                        System.out.println(files[i].replaceAll("\\.png", ".NOCRUSH.png"));
                    }
                }
            }
        }


        //
        packer = new Packer();
        System.out.println();
        for (int g = 0; g < sources.size(); g++) {
            //System.out.println("Start source: " + sources.get(g));
            images.add(new Image("file://" + sources.get(g)));
            ArrayList<IRectangle2D> r = GetSpriteRectsFromImage(g, images.get(g), findDimensions(sources.get(g))[0], findDimensions(sources.get(g))[1]);
            starts.add(src_rects.size());
            src_rects.addAll(r);
            lens.add(r.size());
            for (IRectangle2D ignored : r) {
                imgIds.add(g);
            }
        }
        for (int i = 0; i < src_rects.size(); i++) {
            //System.out.print("Source Rect #" + i + ": ");
            //System.out.println(src_rects.get(i));
            packer._boxes.add(new Packer.Box((int)src_rects.get(i).getWidth(), (int)src_rects.get(i).getHeight(), imgIds.get(i)));
            packer._boxes.get(i).source = src_rects.get(i);
            packer._boxes.get(i).index = i;
        }
        packer.Do();

        src_rects.clear();
        for (int i = 0; i < packer._boxes.size(); i++) {
            Packer.Box box = packer._boxes.get(i);
            //if (box.position != null) {
                rects.add(new IRectangle2D(box.position.pos_x, box.position.pos_y, box.width, box.height));
                src_rects.add(box.source);
            //}
        }
        for (int i = 0; i < packer._boxes.size(); i++) {
            Packer.Box box = packer._boxes.get(i);
            ids.add(box.id);
            //System.out.print("Final Rect #" + box.id + ": ");
            //System.out.println(rects.get(i));
        }
        finalWidth = (int)packer.containerWidth;
        finalHeight = (int)packer.containerHeight;

        String folderName = new File(sources.get(0)).getParentFile().getName().split("\\.")[0];

        for (int i = 0; i < images.size(); i++) {
            int frame_count = lens.get(i);

            String dir = new File(sources.get(0)).getParent();
            String shortName = cleanName(new File(sources.get(i)).getName().split("\\.")[0]);

            String spr_file = dir + "/" + folderName + "/" + shortName + ".spr";

            System.out.println("Sprite: " + shortName + "," + " Frame count: " + lens.get(i));
            //System.out.println();

            int[] X = new int[frame_count];
            int[] Y = new int[frame_count];
            int[] W = new int[frame_count];
            int[] H = new int[frame_count];
            int[] OX = new int[frame_count];
            int[] OY = new int[frame_count];
            int[] OW = new int[frame_count];
            int[] OH = new int[frame_count];

            int found = 0;

            for (int g = 0; g < rects.size(); g++) {
                int frame = packer._boxes.get(g).index - starts.get(ids.get(g));
                if (ids.get(g) == i) {
                    //System.out.println(" : #" + frame + " at rects[" + g + "]");
                    X[frame] = (int)rects.get(g).getMinX();
                    Y[frame] = (int)rects.get(g).getMinY();
                    W[frame] = (int)rects.get(g).getWidth();
                    H[frame] = (int)rects.get(g).getHeight();
                    OX[frame] = (int)src_rects.get(g).OffX;
                    OY[frame] = (int)src_rects.get(g).OffY;
                    OW[frame] = (int)src_rects.get(g).OffW;
                    OH[frame] = (int)src_rects.get(g).OffH;
                    found++;
                }
                if (found >= frame_count)
                    break;
            }

            //System.out.println("  Outputting to: " + spr_file);

            try {
                new File(spr_file).getParentFile().mkdirs();
                DataOutputStream os = new DataOutputStream(new FileOutputStream(spr_file));
                for (int v = 0; v < frame_count; v++) {
                    os.writeByte(((X[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((X[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((Y[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((Y[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((W[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((W[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((H[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((H[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((OX[v] & 0x00FF) >> 0) & 0xFF);
                    os.writeByte(((OY[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((OW[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((OW[v] & 0x00FF) >> 0) & 0xFF);

                    os.writeByte(((OH[v] & 0xFF00) >> 8) & 0xFF);
                    os.writeByte(((OH[v] & 0x00FF) >> 0) & 0xFF);
                }
                os.close();
            }
            catch (Exception e) {
                e.printStackTrace();
                System.exit(0);
            }
        }

        System.out.println("finalWidth: " + finalWidth + " finalHeight: " + finalHeight);

        String dir = new File(sources.get(0)).getParent();
        String spr_file = dir + "/" + folderName + "/Texture.png";

        System.out.println("Outputting final texture to: " + spr_file);
        saveTextureToFile(spr_file);

        System.exit(0);









        GridPane root = new GridPane();
        scene = new Scene(root, finalWidth, finalHeight);

        Pane canvasWrapper = new Pane();
        canvas = new Canvas();
        canvasWrapper.getChildren().add(canvas);
        root.add(canvasWrapper, 0, 2, 1, 1);
        GridPane.setHgrow(canvasWrapper, Priority.ALWAYS);
        GridPane.setVgrow(canvasWrapper, Priority.ALWAYS);
        GridPane.setFillWidth(canvasWrapper, true);
        GridPane.setFillHeight(canvasWrapper, true);
        canvas.widthProperty().bind(canvasWrapper.widthProperty());
        canvas.heightProperty().bind(canvasWrapper.heightProperty());
        canvas.widthProperty().addListener(event -> refreshCanvas());
        canvas.heightProperty().addListener(event -> refreshCanvas());


        scene.setFill(Color.OLDLACE);
        stage.setTitle("SpritePacker");
        stage.setScene(scene);
        stage.show();


    }

    private void saveTextureToFile(String filename) {
        BufferedImage finalSheet = new BufferedImage(finalWidth, finalHeight, BufferedImage.TYPE_INT_ARGB);
        Graphics2D cg = finalSheet.createGraphics();

        for (int g = 0; g < rects.size(); g++) {
            IRectangle2D rect = rects.get(g);
            IRectangle2D src_rect = src_rects.get(g);
            //gc.setFill(new Color(1.0, 0.0, 1.0, 0.5));
            //gc.fillRect(rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());

            java.awt.Image image = SwingFXUtils.fromFXImage(images.get(ids.get(g)), null);
            //gc.drawImage(image, src_rect.getMinX(), src_rect.getMinY(), src_rect.getWidth(), src_rect.getHeight(), rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());
            cg.drawImage(image, (int)rect.getMinX(), (int)rect.getMinY(), (int)rect.getMaxX(), (int)rect.getMaxY(), (int)src_rect.getMinX(), (int)src_rect.getMinY(), (int)src_rect.getMaxX(), (int)src_rect.getMaxY(), null);


            /*
            int frame = packer._boxes.get(g).index - starts.get(ids.get(g));
            gc.setFill(new Color(1.0, 1.0, 1.0, 1.0));
            gc.setTextAlign(TextAlignment.CENTER);
            gc.setTextBaseline(VPos.CENTER);
            gc.fillText(frame + "", rect.getMinX() + rect.getWidth() * 0.5, rect.getMinY() + rect.getHeight() * 0.5);
            //*/
        }


        try {
            ImageIO.write(finalSheet, "png", new File(filename));
        } catch (Exception e) { }

        finalSheet.flush();
        cg.dispose();
    }

    private void refreshCanvas() {
        GraphicsContext gc = canvas.getGraphicsContext2D();

        gc.setFill(Color.BLACK);
        gc.fillRect(0, 0, canvas.getWidth() - 0, canvas.getHeight() - 0);

        gc.save();
        //gc.scale(2, 2);

        gc.setFill(Color.GRAY);
        gc.fillRect(0, 0, finalWidth, finalHeight);

        for (int g = 0; g < rects.size(); g++) {
            IRectangle2D rect = rects.get(g);
            IRectangle2D src_rect = src_rects.get(g);
            //gc.setFill(new Color(1.0, 0.0, 1.0, 0.5));
            //gc.fillRect(rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());

            Image image = images.get(ids.get(g));
            gc.drawImage(image, src_rect.getMinX(), src_rect.getMinY(), src_rect.getWidth(), src_rect.getHeight(), rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());

            ///*
            int frame = packer._boxes.get(g).index - starts.get(ids.get(g));
            gc.setFill(new Color(1.0, 1.0, 1.0, 1.0));
            gc.setTextAlign(TextAlignment.CENTER);
            gc.setTextBaseline(VPos.CENTER);
            gc.fillText(frame + "", rect.getMinX() + rect.getWidth() * 0.5, rect.getMinY() + rect.getHeight() * 0.5);
            //*/
        }

        gc.restore();
    }

    public ArrayList<IRectangle2D> GetSpriteRectsFromImage(int g, Image img, int HframeCount, int VframeCount) {
        int[] pixels = ImageUtilities.getRGB(img);
        int width = (int)img.getWidth();
        int height = (int)img.getHeight();
        ArrayList<IRectangle2D> list = new ArrayList<>();
        for (int y = 0; y < VframeCount; y++) {
            for (int x = 0; x < HframeCount; x++) {
                int stX = x * width / HframeCount;
                int stY = y * height / VframeCount;
                int stW = width / HframeCount;
                int stH = height / VframeCount;

                int leftStop = stX + stW - 1;
                int rightStop = stX;
                int topStop = stY + stH - 1;
                int bottomStop = stY;

                //System.out.println("frame: " + x + " stW: " + stW);

                // begin pixel checks
                boolean stopCheck = false;
                /*
                if (sources.get(g).contains("NOCRUSH")) {
                    stopCheck = true;

                    leftStop = stX;
                    rightStop = stX + stW - 1;
                    topStop = stY;
                    bottomStop = stY + stH - 1;
                }
                //*/
                for (int yy = stY; yy < stY + stH && !stopCheck; yy++) {
                    boolean skipLine = false;
                    for (int xx = stX; xx < stX + stW; xx++) { // Begin by scanning left to right
                        int toPx = (xx + yy * width);
                        int a = (pixels[toPx] >> 24) & 0xff;
                        if (a > 5) {
                            if (xx < leftStop) {
                                leftStop = xx;
                                continue;
                            }
                        }
                        if (xx >= stX + stW - 1 && leftStop == stX + stW - 1)
                            skipLine = true;
                    }
                    for (int xx = stX + stW - 1; xx >= stX + 1 && !skipLine; xx--) { // right to left
                        int toPx = (xx + yy * width);
                        int a = (pixels[toPx] >> 24) & 0xff;
                        if (a > 5) {
                            if (xx > rightStop) {
                                rightStop = xx;
                                continue;
                            }
                        }
                    }
                }
                for (int xx = stX; xx < stX + stW && !stopCheck; xx++) {
                    boolean skipLine = false;
                    for (int yy = stY; yy < stY + stH; yy++) { // Begin by scanning left to right
                        int toPx = (xx + yy * width);
                        int a = (pixels[toPx] >> 24) & 0xff;
                        if (a > 5) {
                            if (yy < topStop) {
                                topStop = yy;
                                continue;
                            }
                        }
                        if (yy >= stY + stH - 1 && topStop == stY + stH - 1)
                            skipLine = true;
                    }
                    for (int yy = stY + stH - 1; yy >= stY + 1 && !skipLine; yy--) { // right to left
                        int toPx = (xx + yy * width);
                        int a = (pixels[toPx] >> 24) & 0xff;
                        if (a > 5) {
                            if (yy > bottomStop) {
                                bottomStop = yy;
                                continue;
                            }
                        }
                    }
                }

                if (leftStop == stX + stW - 1)
                    continue;
                if (rightStop == stX)
                    continue;
                if (topStop == stY + stH - 1)
                    continue;
                if (bottomStop == stY)
                    continue;



                list.add(new IRectangle2D(leftStop, topStop, rightStop - leftStop + 1, bottomStop - topStop + 1, (int)(leftStop - stX), (int)(topStop - stY), stW, stH));
            }
        }
        return list;
    }
}
