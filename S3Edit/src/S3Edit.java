import CodeEditor.ImageUtilities;
import CodeEditor.PaletteTone;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.embed.swing.SwingFXUtils;
import javafx.geometry.*;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextField;
import javafx.scene.effect.MotionBlur;
import javafx.scene.effect.SepiaTone;
import javafx.scene.image.Image;
import javafx.scene.image.PixelFormat;
import javafx.scene.image.PixelReader;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.*;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.ini4j.Profile;
import org.ini4j.Wini;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.Buffer;
import java.nio.file.CopyOption;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class S3Edit extends Application {

    public boolean showObjectsOverAll = true;
    public EditMode currentEditMode = EditMode.TILES;
    public ArrayList<AnimatedTiles> dataAnimTiles = new ArrayList<>();
    public byte[] dataLayout;
    public byte[] dataTiledata;
    public byte[] dataBlocks;
    public byte[] dataTilePixels;
    public byte[] dataTilePixelsAnim;
    public byte[] dataCollisionIndex;
    public byte[] dataCollisionHeights;
    public byte[] dataCollisionAngles;
    public int    dataCollisionDataSize = 1;
    public byte[] bPalette;
    public byte[] bPaletteWater;
    public byte[] angles;
    public boolean paletted = false;
    public boolean showGrid = true;
    public ArrayList<ArrayList<Integer>> liveDataChunks;
    public ArrayList<ArrayList<Integer>> liveDataChunks16x16;
    public int liveDataLevelWidth;
    public int liveDataLevelHeight;
    Label guiStatusLabel;
    ProgressBar guiStatusProgressBar;
    ExecutorService service = Executors.newFixedThreadPool(2);
    Scene scene;




    int lvlIndex = 0;
    int ACT = 1;


    String[] lvlIDs = new String[] {
            "AIZ",
            "HCZ",
            "MGZ",
            "CNZ",
            "ICZ",
            "LBZ",

            "MHZ",
            "FBZ",
            "SOZ",
            "LRZ",
            "HPZ",
            "SSZ",
            "DEZ",
            "TDZ",
            "SSZ",
    };
    String[] lvlINInames = new String[] {
            "Angel Island Zone Act #",
            "Hydrocity Zone Act #",
            "Marble Garden Zone Act #",
            "Carnival Night Zone Act #",
            "IceCap Zone Act #",
            "Launch Base Zone Act #",

            "Mushroom Hill Zone Act #",
            "Flying Battery Zone Act #",
            "Sandopolis Zone Act #",
            "Lava Reef Zone Act #",
            "Hidden Palace Zone",
            "Sky Sanctuary Zone",
            "Death Egg Zone Act #",
            "Doomsday Zone",
            "SSZ",
    };
    String[] lvlTitles = new String[] {
            "ANGEL ISLAND",
            "HYDROCITY",
            "MARBLE GARDEN",
            "CARNIVAL NIGHT",
            "ICE CAP",
            "LAUNCH BASE",

            "MUSHROOM VALLEY",
            "FLYING BATTERY",
            "SANDOPOLIS",
            "LAVA REEF",
            "HIDDEN PALACE",
            "SKY SANCTUARY",
            "DEATH EGG",
            "THE DOOMSDAY",
    };

    String levelID = lvlIDs[lvlIndex];
    String sect = lvlINInames[lvlIndex].replaceAll("#", ACT + "");

    int viewX = 0;
    int viewY = 0;
    int mouseX = -1;
    int mouseY = -1;
    boolean mouseDown = false;
    boolean chunkHandActive = false;
    boolean chunkHandUp = false;
    int chunkHandX = -1;
    int chunkHandY = -1;
    int chunkHandData = -1;
    ArrayList<String> unimplementedOnes = new ArrayList<>();
    String addrPlayer = "A1";
    String addrObject = "A0";
    private MenuBar menuBar = new MenuBar();
    private Menu menuFile = new Menu("File");
    private Menu menuView = new Menu("View");
    private Menu menuExport = new Menu("Export");
    private Menu menuGenerate = new Menu("Generate");
    private EditMode[] editModes = new EditMode[]{
            EditMode.OBJECTS,
            EditMode.CHUNKS,
            EditMode.TILES,
            EditMode.CAMERA,
            EditMode.WATER,
            EditMode.BACKGROUND,
    };
    private Canvas canvas = new Canvas();
    private Image tempChunkFUnpaletted;
    private Image tempChunkBUnpaletted;
    private Image tempChunkFPaletted;
    private Image tempChunkBPaletted;
    private Image tempStaticTiles;
    private Image tempAnimatTiles;
    private Image tempActTileSheet;
    private Image tempActTileSheetPal;
    private Image tempActTileSheetPal1;
    private Image tempActTileSheetPal2;
    private Image tempActTileSheetPal3;
    private TextField objectPropName = new TextField();
    private TextField objectPropID = new TextField();
    private TextField objectPropX = new TextField();
    private TextField objectPropY = new TextField();
    private TextField objectPropSubType = new TextField();
    private String gameSourcePath = "/Users/justin/Dropbox/sonic3/";
    private Map<Integer, ObjectFact> objectFacts = new HashMap<>();
    private ArrayList<Obj> liveDataObjects = new ArrayList<>();

    public static Color getRGB(int argb) {
        int a = (argb >> 24) & 0xff;
        int r = (argb >> 16) & 0xff;
        int g = (argb >> 8) & 0xff;
        int b = argb & 0xff;
        return Color.rgb(r, g, b, a / 255.0);
    }

    public static boolean readImageThing() {
        if (!false)
            return false;

        try {
            Image image = SwingFXUtils.toFXImage(ImageIO.read(new File("C:\\Users\\Justin\\Dropbox\\help progress.png")), null);
            int width = (int) image.getWidth();
            int height = (int) image.getHeight();

            int[] rgb = new int[width * height];
            PixelReader reader = image.getPixelReader();
            reader.getPixels(0, 0, width, height, PixelFormat.getIntArgbInstance(), rgb, 0, width);

            int magentas = 0;
            int yellow = 0;
            int cyan = 0;
            int blue = 0;
            int white = 0;

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Color pixel = getRGB(rgb[x + y * width]);

                    if (pixel.getRed() > 0.5 && pixel.getGreen() < 0.5 && pixel.getBlue() > 0.5 && pixel.getOpacity() > 0.5) {
                        //System.out.println("Magenta: (" + x + ", " + y + ") " + pixel.toString());
                        System.out.print(y + ", ");

                        magentas++;


                        if (magentas == 3)
                            System.out.print("  ");

                        if ((magentas - 3 - 11) % 16 == 0)
                            System.out.print("  ");
                    }
                    if (pixel.getRed() > 0.5 && pixel.getGreen() > 0.5 && pixel.getBlue() < 0.5 && pixel.getOpacity() > 0.5) {
                        //System.out.println("Yellow: (" + x + ", " + y + ") " + pixel.toString());
                        //System.out.println("" + (512 - x - 256) + " " + (y) + " ");
                        yellow++;
                    }
                    if (pixel.getRed() < 0.5 && pixel.getGreen() > 0.5 && pixel.getBlue() > 0.5 && pixel.getOpacity() > 0.5) {
                        //System.out.println("Cyan: (" + x + ", " + y + ") " + pixel.toString());
                        //System.out.println("" + (512 - x - 256) + " " + (y) + " ");
                        cyan++;
                    }
                    if (pixel.getRed() < 0.5 && pixel.getGreen() < 0.5 && pixel.getBlue() > 0.5 && pixel.getOpacity() > 0.5) {
                        //System.out.println("Blue: (" + x + ", " + y + ") " + pixel.toString());
                        //System.out.println("" + (512 - x - 256) + " " + (y) + " ");
                        blue++;
                    }
                    if (pixel.getRed() > 0.5 && pixel.getGreen() > 0.5 && pixel.getBlue() > 0.5 && pixel.getOpacity() > 0.5) {
                        //System.out.println("Blue: (" + x + ", " + y + ") " + pixel.toString());
                        //System.out.println("" + (512 - x - 256) + " " + (y) + " ");
                        white++;
                    }
                }
            }
            System.out.println();
            System.out.println("Magentas: " + magentas);
            System.out.println("Yellow: " + yellow);
            System.out.println("Cyans: " + cyan);
            System.out.println("Blue: " + blue);
            System.out.println("White: " + white);

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public static void main(String[] args) {
        if (readImageThing()) {
            //return;
        }

        launch(args);
    }

    public int[] findDimensions(String texName) {
        int num = 1;
        int numH = 1;
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
        } catch (Exception e) {
        }
        return new int[]{num, numH};
    }

    String cleanName(String filename) {
        String texName = new File(filename).getName().replace(".", "_").replace("\\", "_").replace("source_", "").replace("_png", "");
        /*try {
            texName = texName.substring(0, texName.length() - 1);

            if (!texName.substring(texName.length() - 1).equals("x")) {
                texName = texName.substring(0, texName.length() - 1);
            }

            if (texName.substring(texName.length() - 1).equals("x") && Character.isDigit(texName.charAt(texName.length() - 2))) {
                texName = texName.substring(0, texName.length() - 1);
            }

            texName = texName.substring(0, texName.length() - 1);

            texName = texName.substring(0, texName.length() - 1);
            return texName;
        } catch (Exception e) { }*/
        return texName;
    }

    ObjectFact makeOF(int ID, String image, ObjectFact obj) {
        BufferedImage bufferedImage;
        try {
            String str = gameSourcePath + image.substring(0, image.lastIndexOf("/")) + "/olds" + image.substring(image.lastIndexOf("/"));
            File oldfile = new File(str);
            if (!oldfile.exists())
                bufferedImage = ImageIO.read(new File(gameSourcePath + image));
            else
                bufferedImage = ImageIO.read(oldfile);
            obj.imageSrc = image;
            obj.image = SwingFXUtils.toFXImage(bufferedImage, null);

            if (!image.contains("Common"))
                obj.image = ImageUtilities.filter(obj.image, new PaletteTone(bPalette, bPaletteWater));//bPalette, bPalette2

            obj.imgW = findDimensions(image)[0];
            obj.imgH = findDimensions(image)[1];
            bufferedImage.flush();
            if (objectFacts.get(ID) == null)
                objectFacts.put(ID, obj);
            return obj;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    ObjectFact makeOF(int ID, String image, int w, int h, int d) {
        BufferedImage bufferedImage = null;
        try {
            ObjectFact obj = new ObjectFact();
            String str = gameSourcePath;
            if (!image.isEmpty()) {
                str += image.substring(0, image.lastIndexOf("/")) + "/olds" + image.substring(image.lastIndexOf("/"));
                File oldfile = new File(str);
                if (new File(gameSourcePath + image).exists()) {
                    if (!oldfile.exists())
                        bufferedImage = ImageIO.read(new File(gameSourcePath + image));
                    else
                        bufferedImage = ImageIO.read(oldfile);
                }
            }
            obj.imageSrc = image;
            if (bufferedImage != null) {
                obj.image = SwingFXUtils.toFXImage(bufferedImage, null);
                bufferedImage.flush();

                if (!image.contains("Common"))
                    obj.image = ImageUtilities.filter(obj.image, new PaletteTone(bPalette, bPaletteWater));//bPalette, bPalette2
            }
            obj.w = w;
            obj.h = h;
            obj.d = d;
            obj.imgW = findDimensions(image)[0];
            obj.imgH = findDimensions(image)[1];
            if (objectFacts.get(ID) == null)
                objectFacts.put(ID, obj);
            return obj;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    int parseInt(String num, int def) {
        try {
            return Integer.parseInt(num);
        } catch (NumberFormatException e) {
            return def;
        }
    }

    boolean parseBool(String num, boolean def) {
        try {
            return Boolean.parseBoolean(num);
        } catch (Exception e) {
            return def;
        }
    }

    void populateObjectFacts() {
        try {
            makeOF(0x01, "source/res/Common/Monitor8x2.png", new OF.Monitor());
            makeOF(0x07, "source/res/Common/SpringRed3.png", new OF.Spring());
            makeOF(0x08, "source/res/Common/Spikes.png", new OF.Spikes());

            Wini ini = new Wini(new File(gameSourcePath + "S3Edit/objects.ini"));

            for (int i = 0; i < 256; i++) {
                Profile.Section section = ini.get("x" + Integer.toHexString(i).toUpperCase());
                if (section == null)
                    continue;

                String finalimg = "";
                String name = section.get("name");
                String image = section.get("image");
                if (image != null) {
                    for (int img = 0; img < image.split("\\|").length; img++) {
                        if (image.split("\\|")[img].startsWith(levelID)) {
                            finalimg = image.split("\\|")[img].split(":")[1];
                            finalimg = "source/res/" + finalimg;
                            break;
                        } else {
                            finalimg = "source/res/" + image.split("\\|")[img];
                        }
                    }
                    if (!(new File(gameSourcePath + finalimg).exists())) {
                        //System.out.println("File \"" + gameSourcePath + finalimg + "\" does not exist.");
                    }
                }

                int width = parseInt(section.get("width"), 32);
                int height = parseInt(section.get("height"), 32);
                int offX = parseInt(section.get("offX"), 0);
                int offY = parseInt(section.get("offY"), 0);
                int displayFrame = parseInt(section.get("displayFrame"), 0);
                boolean needBubbleFx = parseBool(section.get("imageNeedBubbleFX"), false);
                int needExplosion = parseInt(section.get("imageNeedExplosion"), 0);

                makeOF(i, finalimg, width, height, displayFrame);

                if (objectFacts.get(i) != null) {
                    objectFacts.get(i).name = name;
                    objectFacts.get(i).imageSrc = finalimg;
                    objectFacts.get(i).offX = offX;
                    objectFacts.get(i).offY = offY;
                    objectFacts.get(i).needBubbleFx = needBubbleFx;
                    objectFacts.get(i).needExplosion = needExplosion;
                }
            }

            /*makeOF(0x34, "source/res/Common/Starpost5.png", 16, 64, 0);

            makeOF(0x85, "source/res/Common/SpecialRing8x2.png", 64, 64, 8);



            makeOF(0xF, "source/res/HCZ/BreakablePlatforms12.png", 160, 80, 0);


            makeOF(0x93, "source/res/HCZ/Jawz2.png", 40, 24, 0);
            makeOF(0x94, "source/res/HCZ/Blastoid4.png", 40, 24, 0);
            makeOF(0x95, "source/res/HCZ/Buggernaut6.png", 24, 24, 1);
            makeOF(0x96, "source/res/HCZ/TurboSpiker16.png", 48, 48, 0);
            makeOF(0x97, "source/res/HCZ/MegaChopper4.png", 16, 16, 0);
            makeOF(0x98, "source/res/HCZ/Pointdexter3.png", 36, 32, 0);

            makeOF(0x33, "source/res/HCZ/Button3.png", 32, 24, 0);
            makeOF(0x38, "source/res/HCZ/Fan5x2.png", 32, 32, 0);
            makeOF(0x3A, "source/res/HCZ/HandLauncher8.png", 64, 78, 6);
            makeOF(0x40, "source/res/HCZ/Block2.png", 32, 32, 0);

            makeOF(0x6E, "source/res/HCZ/WaterDrop6.png", 16, 16, 0);*/
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //private void startAdd(GridPane root) {}
    //private void startAdd(GridPane root) {}
    //private void startAdd(GridPane root) {}

    void generateS3EditDirectories() {
        if (!new File(gameSourcePath + "S3Edit/backups").exists()) {
            new File(gameSourcePath + "S3Edit/backups").mkdirs();
        }
    }

    String getBackupFilename(File file) {
        String backupFilename = gameSourcePath + "S3Edit/backups/" + file.getParent().substring(gameSourcePath.length()) + "/" + file.getName().split("/.")[0];
        DateFormat dateFormat = new SimpleDateFormat("MM-dd-yy_HH-mm-ss");
        Date date = new Date();
        backupFilename += " (Backup " + dateFormat.format(date) + ")";
        if (file.getName().split("/.").length > 1)
            backupFilename += "." + file.getName().split("/.")[1];

        return backupFilename;
    }

    public String listFilesForFolder(final File folder, int doWhat) {
        String println = "";
        for (final File fileEntry : folder.listFiles()) {
            if (fileEntry.getName().startsWith("."))
                continue;
            if (fileEntry.getName().equalsIgnoreCase("olds"))
                continue;
            if (fileEntry.isDirectory()) {
                println += listFilesForFolder(fileEntry, doWhat);
            } else {
                String binaryName = fileEntry.getAbsolutePath().substring(gameSourcePath.length()).replace(".", "_").replace("/", "_").replace("\\", "_").replace("source_", "");
                if (doWhat == 0) {
                    println += "EXTLD(" + binaryName + ");" + "\n";
                } else if (doWhat == 1) {
                    binaryName = fileEntry.getName().replace(".", "_").replace("/", "_").replace("\\", "_").replace("_ogg", "");
                    println += "    Audio* aud_" + binaryName + " = NULL;" + "\n";
                }
            }
        }
        return println;
    }

    boolean generateResourcesFile() {
        File resourcesFile = new File(gameSourcePath + "source/Resources.h");

        String HEADER = "";
        HEADER += listFilesForFolder(new File(gameSourcePath + "source/res"), 0);
        HEADER += listFilesForFolder(new File(gameSourcePath + "source/snd"), 0);


        try {
            String text = "";
            List<String> lines = Files.readAllLines(Paths.get(gameSourcePath + "S3Edit/Resources.h"));
            for (String line : lines)
                text += line + "\n";

            text = text.replace("### RESOURCES_H_AUTOFILL ###", HEADER);

            System.out.print(text);

            PrintWriter writer = new PrintWriter(resourcesFile, "UTF-8");
            writer.println(text);
            writer.close();
        } catch (IOException e) {
        }


        generateS3EditDirectories();

        if (resourcesFile.exists()) {
            new File(getBackupFilename(resourcesFile)).getParentFile().mkdirs();
            guiStatusLabel.setText(getBackupFilename(resourcesFile));
            try {
                Files.copy(Paths.get(resourcesFile.getAbsolutePath()), Paths.get(getBackupFilename(resourcesFile)), StandardCopyOption.REPLACE_EXISTING);
            } catch (Exception e) {
            }
        }
        return true;
    }

    boolean generateSceneHFile() {
        File resourcesFile = new File(gameSourcePath + "source/Scene.h");

        String HEADER = "";
        HEADER += listFilesForFolder(new File(gameSourcePath + "source/snd"), 1);


        try {
            String text = "";
            List<String> lines = Files.readAllLines(Paths.get(gameSourcePath + "S3Edit/Scene.h"));
            for (String line : lines)
                text += line + "\n";

            text = text.replace("### SCENE_H_AUTOFILL ###", HEADER);

            System.out.print(text);

            PrintWriter writer = new PrintWriter(resourcesFile, "UTF-8");
            writer.println(text);
            writer.close();
        } catch (IOException e) {
        }


        generateS3EditDirectories();

        if (resourcesFile.exists()) {
            new File(getBackupFilename(resourcesFile)).getParentFile().mkdirs();
            guiStatusLabel.setText(getBackupFilename(resourcesFile));
            try {
                Files.copy(Paths.get(resourcesFile.getAbsolutePath()), Paths.get(getBackupFilename(resourcesFile)), StandardCopyOption.REPLACE_EXISTING);
            } catch (Exception e) {
            }
        }
        return true;
    }

    boolean generateObjectCreate() {
        String output = "";

        for (int i = 1; i < 256; i++) {
            if (objectFacts.get(i) != null) {
                ObjectFact of = objectFacts.get(i);
                if (!of.imageSrc.contains(levelID)) continue;

                String name = of.name.replaceAll(" ", "");
                int[] size = findDimensions(cleanName(of.imageSrc));
                output += "    tex_objects[0x" + Integer.toHexString(i).toUpperCase() + "] = newTexture(res_" + levelID + "_objects_" + cleanName(of.imageSrc) + ", " + size[0] + ", " + size[1] + ", \"" + of.imageSrc.substring(7) + "\"); // " + name + "\n";
            }
        }

        output += "\n";
        output += "===========================================================\n";
        output += "\n";
        output += "===========================================================\n";
        output += "\n";

        output += "    for (int i = 0; i < objs_length; i += 6) {\n";
        output += "        int data = (objs[i + 2] & 0xF0) / 0x10;\n";
        output += "        int X = (objs[i + 0] & 0xFF) * 0x100 + (objs[i + 1] & 0xFF);\n";
        output += "        int Y = (objs[i + 2] & 0x0F) * 0x100 + (objs[i + 3] & 0xFF);\n";
        output += "        int PRIORITY = (data >> 3) & 0x01;\n";
        output += "        int FLIPX = (data >> 1) & 0x01;\n";
        output += "        int FLIPY = (data >> 2) & 0x01;\n";
        output += "        \n";

        boolean firstDone = false;
        for (int i = 1; i < 256; i++) {
            if (objectFacts.get(i) != null) {
                ObjectFact of = objectFacts.get(i);
                String name = of.name.replaceAll(" ", "");
                output += "        " + (!firstDone ? "" : "else ") + "if (objs[i + 4] == 0x" + Integer.toHexString(i).toUpperCase() + ") {\n";
                output += "            " + name + "* obj = new " + name + "(objs[i + 5]);\n";
                output += "            obj->x = X;\n";
                output += "            obj->y = Y;\n";
                output += "            obj->flipX = FLIPX;\n";
                output += "            obj->flipY = FLIPY;\n";
                output += "            obj->prior = PRIORITY;\n";
                output += "            obj->data1 = objs[i + 4];\n";
                output += "            \n";
                output += "            obj->tex = tex_objects[0x" + Integer.toHexString(i).toUpperCase() + "];\n";
                output += "            \n";
                output += "            obj->sc = this;\n";
                output += "            obj->app = app;\n";
                output += "            objects[objects_count++] = obj;\n";
                output += "        }\n";
                firstDone = true;
            }
        }

        output += "    }\n";

        System.out.print(output);
        return true;
    }

    public byte[] readBytes(String file) {
        try {
            InputStream is = new FileInputStream(file);
            int count = is.available();

            DataInputStream dis = new DataInputStream(is);
            byte[] buffer = new byte[count];
            dis.read(buffer);
            dis.close();

            return buffer;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new byte[0];
    }

    public byte[] decodeKosinski(byte[] inputOlde) {
        int[] lookupTable = new int[]{
                0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
                0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
                0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
                0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
                0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
                0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
                0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
                0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
                0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
                0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
                0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
                0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
                0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
                0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
                0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
                0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
        };

        int pointer = 0;
        int lastPoint = 0;
        int decompressedBytes = 0;

        short[] inputOld = new short[inputOlde.length];
        for (int g = 0; g < inputOlde.length; g++) {
            inputOld[g] = (short) (inputOlde[g] & 0xFF);
        }

        short[] input = new short[inputOld.length];
        for (int g = 0; g < inputOld.length; g++) {
            input[g] = (short) (lookupTable[inputOld[g] & 0xFF]);
        }

        String bits = "";

        for (int i = pointer; i < input.length; i++) {
            String toAdd = Integer.toBinaryString(input[i]);
            toAdd = ("00000000" + toAdd).substring(toAdd.length());
            bits += toAdd;
        }

        ArrayList<Integer> out = new ArrayList<Integer>();

        String bitField = "";
        bitField = bits.substring(0, 16);
        bits = bits.substring(16);
        //print("");
        System.out.println("Loaded new Description Field at " + (pointer) + " of " + bitField);
        pointer++;
        pointer++;

        while (bitField.length() > 0) {
            int pop = Integer.parseInt(Character.toString(bitField.charAt(0)));
            bitField = bitField.substring(1);
            if (bitField.length() == 0) {
                bitField = bits.substring(0, 16);
                bits = bits.substring(16);
                pointer++;
                pointer++;
            }

            if (pop == 1) {
                if (pointer + 1 >= inputOld.length) break;
                out.add(inputOld[pointer++] & 0xFF);
                bits = bits.substring(8);
                decompressedBytes++;
            } else {
                pop = Integer.parseInt(Character.toString(bitField.charAt(0)));
                bitField = bitField.substring(1);
                if (bitField.length() == 0) {
                    bitField = bits.substring(0, 16);
                    bits = bits.substring(16);
                    pointer++;
                    pointer++;
                }

                long count = 0;
                long offset = 0;

                if (pop == 1) {
                    if (pointer + 1 >= inputOld.length) break;
                    int low = (inputOld[pointer++] & 0xFF);
                    if (pointer + 1 >= inputOld.length) break;
                    int high = (inputOld[pointer++] & 0xFF);
                    bits = bits.substring(16);
                    count = high & 0x07;
                    if (count == 0) {
                        if (pointer + 1 >= inputOld.length) break;
                        count = (inputOld[pointer++] & 0xFF);
                        bits = bits.substring(8);
                        if (count == 0) {
                            break;
                        }

                        if (count == 1) {
                            continue;
                        }
                    } else {
                        ++count;
                    }

                    offset = -8192 + (high >> 3) * 256 + low;
                } else {
                    pop = Integer.parseInt(Character.toString(bitField.charAt(0)));
                    bitField = bitField.substring(1);
                    if (bitField.length() == 0) {
                        bitField = bits.substring(0, 16);
                        bits = bits.substring(16);
                        pointer++;
                        pointer++;
                    }
                    int low = pop;

                    pop = Integer.parseInt(Character.toString(bitField.charAt(0)));
                    bitField = bitField.substring(1);
                    if (bitField.length() == 0) {
                        bitField = bits.substring(0, 16);
                        bits = bits.substring(16);
                        pointer++;
                        pointer++;
                    }
                    int high = pop;

                    count = (low << 1 | high) + 1;

                    if (pointer + 1 >= inputOld.length) break;
                    offset = inputOld[pointer++];
                    bits = bits.substring(8);
                    offset -= 256;
                }

                for (long i = 0; i <= count; i++) {
                    try {
                        out.add(out.get(out.size() + (int) offset));
                    } catch (Exception e) {
                    }
                }

                decompressedBytes += count + 1;
            }

            if (decompressedBytes > 232 && decompressedBytes < 240) {
                System.out.println(decompressedBytes + ": " + bitField + " / " + pointer);
            }

        }

        byte[] artOut = new byte[out.size()];
        for (int i = 0; i < out.size(); i += 1) {
            artOut[i] = (byte) (out.get(i) & 0xFF);
        }

        lastPoint = pointer;

        return artOut;
    }

    public byte[] decodeKosinskiM(byte[] inputOld) {
        int decompressedBytes = 0;
        int lastPoint = 0;
        long fullSize = 0;

        fullSize = (inputOld[lastPoint++] & 0xFF) * 0x100 + (inputOld[lastPoint++] & 0xFF);

        int moduleCount = (int) (fullSize & 0xF000) / 0x1000 + 1;
        int dataSize = (int) (fullSize & 0xFFF);

        //print("");
        //print("Number of Modules: " + moduleCount);
        //print("Last Module Data Size: " + dataSize);
        //print("");

        byte[] input = inputOld;

        ArrayList<Integer> out = new ArrayList<Integer>();

        int currentModule = 0;
        for (; ; ) {
            int size = 0x1000;
            if (currentModule == moduleCount - 1) {
                size = dataSize;
            }

            //print("Current Module: " + currentModule);
            //print("Current Size: " + size);
            //System.out.print("lastPoint before: " + lastPoint);
            byte[] input2 = new byte[Math.min(size, inputOld.length - lastPoint)];
            for (int g = 0; g < input2.length; g++) {
                input2[g] = inputOld[g + lastPoint];
            }
            input = input2;

            byte[] firstOut = decodeKosinski(input);
            for (int g = 0; g < firstOut.length; g++) {
                out.add(firstOut[g] & 0xFF);
            }

            //print("lastPoint after: " + lastPoint);

            currentModule++;

            if (decompressedBytes >= fullSize) {
                break;
            }

            // Skip the padding between modules
            int b;
            long paddingEnd = (((lastPoint - 2) + 0xF) & ~0xF) + 2;
            while (lastPoint < paddingEnd) {
                if (lastPoint + 1 >= inputOld.length) break;
                b = inputOld[lastPoint++];

                if (b == -1) {
                    byte[] artOut = new byte[out.size()];
                    for (int i = 0; i < out.size(); i += 1) {
                        artOut[i] = (byte) (out.get(i) & 0xFF);
                    }

                    return artOut;
                    //throw new EndOfStreamException();
                }
            }
        }

        byte[] artOut = new byte[out.size()];
        for (int i = 0; i < out.size(); i += 1) {
            artOut[i] = (byte) (out.get(i) & 0xFF);
        }

        return artOut;
    }

    private void startAddMenus(GridPane root) {
        menuBar.getMenus().addAll(menuFile, menuView, menuGenerate, menuExport);
        root.add(menuBar, 0, 0, 1, 1);

        CheckMenuItem cvItem1 = new CheckMenuItem("Show Grid");
        cvItem1.setSelected(showGrid);
        cvItem1.setOnAction(e -> {
            showGrid = !showGrid;
            refreshCanvas();
        });
        menuView.getItems().add(cvItem1);


        MenuItem cmItem1 = new MenuItem("Generate Resources File...");
        cmItem1.setOnAction(e -> {
            generateResourcesFile();
            /*Clipboard clipboard = Clipboard.getSystemClipboard();
            ClipboardContent content = new ClipboardContent();
            content.putImage(pic.getImage());
            clipboard.setContent(content);*/
        });
        menuGenerate.getItems().add(cmItem1);

        MenuItem cmItem2 = new MenuItem("Generate Scene.h File...");
        cmItem2.setOnAction(e -> {
            generateSceneHFile();
        });
        menuGenerate.getItems().add(cmItem2);

        MenuItem cmItem3 = new MenuItem("Generate Object Creation Code...");
        cmItem3.setOnAction(e -> {
            generateObjectCreate();
        });
        menuGenerate.getItems().add(cmItem3);
    }

    private void startAddStatusBar(GridPane root) {
        HBox hbStatusBar = new HBox();
        hbStatusBar.setSpacing(10);
        hbStatusBar.setAlignment(Pos.CENTER_LEFT);
        hbStatusBar.setPadding(new Insets(5, 5, 5, 5));
        root.add(hbStatusBar, 0, 3, 1, 1);

        HBox hbStatusBar1 = new HBox();
        HBox hbStatusBar2 = new HBox();
        hbStatusBar1.setAlignment(Pos.CENTER_LEFT);
        hbStatusBar2.setAlignment(Pos.CENTER_RIGHT);
        HBox.setHgrow(hbStatusBar1, Priority.ALWAYS);
        hbStatusBar.getChildren().addAll(hbStatusBar1, hbStatusBar2);

        // Create status bar
        guiStatusLabel = new Label("");
        guiStatusProgressBar = new ProgressBar(0);
        hbStatusBar1.getChildren().add(guiStatusLabel);
        hbStatusBar2.getChildren().add(guiStatusProgressBar);
    }

    private void startAddCanvas(GridPane root) {
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

        canvas.setOnScroll((ScrollEvent event) -> {
            viewX -= (int) event.getDeltaX();
            viewY -= (int) event.getDeltaY();

            viewX = Math.max(0, Math.min(liveDataLevelWidth * 128 - (int) canvas.getWidth(), viewX));
            viewY = Math.max(0, Math.min(liveDataLevelHeight * 128 - (int) canvas.getHeight(), viewY));

            refreshCanvas();
        });
        canvas.setOnMousePressed((MouseEvent event) -> {
            mouseX = (int) event.getX();
            mouseY = (int) event.getY();
            mouseDown = true;

            selectChunk();
            selectObject();


            refreshCanvas();
        });
        canvas.setOnMouseMoved((MouseEvent event) -> {
            mouseX = (int) event.getX();
            mouseY = (int) event.getY();

            moveChunk();

            refreshCanvas();
        });
        canvas.setOnMouseDragged((MouseEvent event) -> {
            mouseX = (int) event.getX();
            mouseY = (int) event.getY();

            moveChunk();

            refreshCanvas();
        });
        canvas.setOnMouseReleased((MouseEvent event) -> {
            mouseX = (int) event.getX();
            mouseY = (int) event.getY();
            mouseDown = false;

            dropChunk();

            refreshCanvas();
        });
        canvas.setOnMouseExited((MouseEvent event) -> {
            mouseX = (int) event.getX();
            mouseY = (int) event.getY();
            mouseDown = false;

            dropChunk();

            refreshCanvas();
        });

        // Create Object Information Panel
        TitledPane gridTitlePane = new TitledPane();
        GridPane grid = new GridPane();
        grid.setVgap(4);
        grid.setPadding(new Insets(5, 5, 5, 5));
        grid.add(new Label("Name: "), 0, 0);
        grid.add(objectPropName, 1, 0);
        grid.add(new Label("ID: "), 0, 1);
        grid.add(objectPropID, 1, 1);
        grid.add(new Label("X: "), 0, 2);
        grid.add(objectPropX, 1, 2);
        grid.add(new Label("Y: "), 0, 3);
        grid.add(objectPropY, 1, 3);
        grid.add(new Label("Subtype: "), 0, 4);
        grid.add(objectPropSubType, 1, 4);
        gridTitlePane.setText("Object Properties");
        gridTitlePane.setContent(grid);

        canvasWrapper.getChildren().add(gridTitlePane);
    }

    private void ensureFileWillBeThere(String filename) {
        //try {
        if (!(new File(filename).exists())) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle("File does not exist!");
            alert.setHeaderText("File does not exist!");
            alert.setContentText("Cannot find file for \"" + filename + "\". Press OK to find a replacement file.");
            alert.showAndWait().ifPresent(rs -> {
                if (rs == ButtonType.OK) {
                    FileChooser chooser = new FileChooser();
                    chooser.setTitle("Open File for \"" + filename + "\"");
                    String f = gameSourcePath + "skdisasm-master/levels/" + filename.substring((gameSourcePath + "source/res/").length());
                    if (new File(f).getParentFile() == null || !new File(f).getParentFile().exists()) {
                        f = new File(f).getParentFile().getParentFile().getAbsolutePath();
                    }
                    chooser.setInitialDirectory(new File(f).getParentFile());
                    chooser.setInitialFileName(new File(f).getName());
                    File file = chooser.showOpenDialog(scene.getWindow());

                    new File(filename).getParentFile().mkdirs();

                    if (file == null)
                        System.exit(0);

                    try {
                        Files.copy(Paths.get(file.getAbsolutePath()), Paths.get(filename), StandardCopyOption.REPLACE_EXISTING);
                    } catch (Exception e) {
                    }
                }
            });
        }
        //}
        //catch (IOException e) {}
    }

    @Override
    public void start(Stage stage) {
        //ReadASM();

        if (System.getProperty("os.name").toLowerCase().indexOf("win") >= 0) {
            gameSourcePath = "C:" + gameSourcePath;
        }

        GridPane root = new GridPane();
        scene = new Scene(root, 1280, 720);

        ColumnConstraints column = new ColumnConstraints();
        column.setPercentWidth(100);
        root.getColumnConstraints().add(column);

        //root.setPrefSize(640, 720);
        //root.setMaxSize(Region.USE_COMPUTED_SIZE, Region.USE_COMPUTED_SIZE);

        //// Add menus
        startAddMenus(root);

        //// Create layouts
        HBox hb = new HBox();
        hb.setSpacing(10);
        hb.setAlignment(Pos.CENTER_LEFT);
        hb.setPadding(new Insets(10, 10, 10, 10));
        root.add(hb, 0, 1, 1, 1);

        startAddStatusBar(root);

        //// Create GUI items

        // Create "Edit Mode" option box
        ChoiceBox<String> guiModeBox;
        Label guiModeLabel;
        ArrayList<String> editModeStringList = new ArrayList<>();
        for (EditMode editMode : editModes)
            editModeStringList.add(editMode.title);

        guiModeLabel = new Label("Edit Mode:");
        guiModeBox = new ChoiceBox<>(FXCollections.observableArrayList(editModeStringList));
        guiModeBox.getSelectionModel().selectedIndexProperty().addListener((ov, value, new_value) -> {
            currentEditMode = editModes[new_value.intValue()];
            refreshCanvas();
        });

        Button guiBuildGame = new Button("Build Game");
        guiBuildGame.setOnMousePressed((MouseEvent event) -> {
            System.out.println("");
            System.out.println("==========================================================================");
            System.out.println("Attempting to build " + gameSourcePath + "Sonic.exe...");
            System.out.println("==========================================================================");
            guiStatusLabel.setText("Attempting to build " + gameSourcePath + "Sonic.exe...");
            guiBuildGame.setDisable(true);
            service.submit(new Runnable() {
                boolean error = false;

                public void run() {
                    try {
                        String line;
                        Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "MAKE.bat", null, new File(gameSourcePath));
                        BufferedReader input =
                                new BufferedReader
                                        (new InputStreamReader(p.getErrorStream()));
                        while ((line = input.readLine()) != null) {
                            if (line.contains(" error: ")) {
                                error = true;
                            }
                            System.out.println(line);
                        }
                        input.close();
                        p.destroy();
                    } catch (Exception err) {
                        err.printStackTrace();
                    }

                    System.out.println("Done!");

                    if (!error) {
                        try {
                            String line;
                            Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "Sonic.exe \"STARTPOS 7464 960\"", null, new File(gameSourcePath));
                            BufferedReader input =
                                    new BufferedReader
                                            (new InputStreamReader(p.getInputStream()));
                            while ((line = input.readLine()) != null) {
                                System.out.println(line);
                            }
                            input.close();
                            p.destroy();
                        } catch (Exception err) {
                            err.printStackTrace();
                        }
                    }

                    Platform.runLater(new Runnable() {
                        @Override
                        public void run() {
                            guiBuildGame.setDisable(false);

                            if (!error) {
                                guiStatusLabel.setText("Compiled " + gameSourcePath + "Sonic.exe.");
                            } else {
                                guiStatusLabel.setText("Error while compiling " + gameSourcePath + "Sonic.exe.");
                            }

                            refreshCanvas();
                        }
                    });
                }
            });

        });

        Button guiRunGame = new Button("Run Game");
        guiRunGame.setOnMousePressed((MouseEvent event) -> {
            System.out.println("");
            System.out.println("==========================================================================");
            System.out.println("Attempting to run " + gameSourcePath + "Sonic.exe...");
            System.out.println("==========================================================================");
            try {
                String line;
                Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "Sonic.exe", null, new File(gameSourcePath));
                BufferedReader input =
                        new BufferedReader
                                (new InputStreamReader(p.getInputStream()));
                while ((line = input.readLine()) != null) {
                    System.out.println(line);
                }
                input.close();
            } catch (Exception err) {
                err.printStackTrace();
            }

            refreshCanvas();
        });

        hb.getChildren().addAll(guiModeLabel, guiModeBox, guiBuildGame, guiRunGame);

        // Create canvas
        startAddCanvas(root);

        //// Restore State
        guiModeBox.getSelectionModel().select(0);

        // Load Settings and Options
        int lID = 0;
        for (String lvl : lvlIDs) {
            if (!lvl.equals(levelID))
                lID++;
            else
                break;
        }
        if (lID < 6)
            sect = "Sonic 3\\\\" + sect;
        else
            sect = "Sonic & Knuckles\\\\" + sect;

        String rootD = gameSourcePath + "skdisasm-master/";
        String iniFile = gameSourcePath + "skdisasm-master/SonLVL INI Files/SonLVL.ini";
        HashMap<String, String> ini = new HashMap<>();
        String currentSection = "";
        try {
            Scanner in = new Scanner(new File(iniFile));
            while (in.hasNextLine()) {
                String line = in.nextLine();
                if (line.startsWith("[")) {
                    currentSection = line.substring(1, line.length() - 1);
                } else {
                    String[] data = line.split("=");
                    ini.put(currentSection + data[0], data[1]);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }


        // Load data
        ensureFileWillBeThere(gameSourcePath + "source/res/" + levelID + "/layout/" + ACT + ".bin");
        try {
            new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Layout)/" + ACT + ".bin").getParentFile().mkdirs();
            Files.copy(Paths.get(gameSourcePath + "source/res/" + levelID + "/layout/" + ACT + ".bin"), Paths.get(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Layout)/" + ACT + ".bin"));
        } catch (IOException e) {
            //e.printStackTrace();
        }
        dataLayout = readBytes(gameSourcePath + "source/res/" + levelID + "/layout/" + ACT + ".bin");
        liveDataChunks = new ArrayList<>();
        liveDataLevelWidth = (dataLayout[0] & 0xFF) * 0x100 + (dataLayout[1] & 0xFF);
        liveDataLevelHeight = (dataLayout[4] & 0xFF) * 0x100 + (dataLayout[5] & 0xFF);
        for (int x = 0; x < liveDataLevelWidth; x++) {
            liveDataChunks.add(new ArrayList<>());
            for (int y = 0; y < liveDataLevelHeight; y++) {
                int us = dataLayout[(dataLayout[8 + y * 4] & 0xFF) * 0x100 + (dataLayout[8 + y * 4 + 1] & 0xFF) - 0x8000 + x] & 0xFF;
                liveDataChunks.get(x).add(y, us);
            }
        }

        liveDataChunks16x16 = new ArrayList<>();
        for (int x = 0; x < liveDataLevelWidth * 16; x++) {
            liveDataChunks16x16.add(new ArrayList<>());
            for (int y = 0; y < liveDataLevelHeight * 16; y++) {
                liveDataChunks16x16.get(x).add(y, 0);
            }
        }

        angles = readBytes(gameSourcePath + "source/res/Common/LVL_Angles.bin");

        long sysTime = System.currentTimeMillis();

        String[] files;

        //bPalette = (readBytes(gameSourcePath + "skdisasm-master/General/Sprites/Sonic/Palettes/SonicAndTails.bin"));
        /*bPalette = (readBytes(gameSourcePath + "skdisasm-master/Levels/" + levelID + "/Palettes/Main.bin"));
        if (new File(gameSourcePath + "skdisasm-master/Levels/" + levelID + "/Palettes/" + ACT + ".bin").exists())
            bPalette = concat(bPalette, (readBytes(gameSourcePath + "skdisasm-master/Levels/" + levelID + "/Palettes/" + ACT + ".bin")));*/

        bPalette = new byte[0];
        files = ini.get(sect + "palette").split("\\|");
        int h = 0;
        for (String file : files) {
            h++;
            if (h == 0) continue;

            if (file.equalsIgnoreCase("water")) continue;

            byte[] bytes = readBytes(rootD + file.substring(3, file.indexOf(":")));

            int size = Integer.parseInt(file.split(":")[3]) * 2;
            byte[] bytesNew = new byte[size];
            for (int i = 0; i < size; i++) {
                bytesNew[i] = bytes[i];
            }
            bPalette = concat(bPalette, bytesNew);
        }

        bPaletteWater = new byte[0];
        try {
            if (ini.get(sect + "palette2") != null) {
                files = ini.get(sect + "palette2").split("\\|");
                h = 0;
                for (String file : files) {
                    h++;
                    if (h == 0) continue;

                    if (file.equalsIgnoreCase("water")) continue;

                    byte[] bytes = readBytes(rootD + file.substring(3, file.indexOf(":")));

                    int size = Integer.parseInt(file.split(":")[3]) * 2;
                    byte[] bytesNew = new byte[size];
                    for (int i = 0; i < size; i++) {
                        bytesNew[i] = bytes[i];
                    }
                    bPaletteWater = concat(bPaletteWater, bytesNew);
                }
            }
        }
        catch (Exception e) { }

        System.out.println(">>>>> Palettes loaded.");

        dataTiledata = new byte[0];
        files = ini.get(sect + "chunks").split("\\|");
        for (String file : files) {
            if (new File(rootD + file.substring(3, file.length() - 3) + "unc").exists()) {
                dataTiledata = concat(dataTiledata, readBytes(rootD + file.substring(3, file.length() - 3) + "unc"));
                System.out.println("Loading from file: " + rootD + file.substring(3, file.length() - 3) + "unc");
            }
            else
                dataTiledata = concat(dataTiledata, decodeKosinski(readBytes(rootD + file.substring(3))));
        }

        System.out.println(">>>>> Tiles loaded.");

        dataBlocks = new byte[0];
        files = ini.get(sect + "blocks").split("\\|");
        for (String file : files) {
            if (new File(rootD + file.substring(3, file.length() - 3) + "unc").exists()) {
                System.out.println("Loading from file: " + rootD + file.substring(3, file.length() - 3) + "unc");
                dataBlocks = concat(dataBlocks, readBytes(rootD + file.substring(3, file.length() - 3) + "unc"));
            }
            else
                dataBlocks = concat(dataBlocks, decodeKosinski(readBytes(rootD + file.substring(3))));
        }

        System.out.println(">>>>> Blocks loaded.");

        dataTilePixels = new byte[0];
        files = ini.get(sect + "tiles").split("\\|");
        for (String file : files) {
            String fileName = rootD + file.substring(3);
            if (fileName.endsWith("unbin")) {
                dataTilePixels = concat(dataTilePixels, readBytes(fileName));
            } else {
                if (new File(fileName.replace("bin", "unc")).exists()) {
                    System.out.println("Loading from file: " + fileName.replace("bin", "unc"));
                    dataTilePixels = concat(dataTilePixels, readBytes(fileName.replace("bin", "unc")));
                } else {
                    dataTilePixels = concat(dataTilePixels, decodeKosinskiM(readBytes(fileName)));
                }
            }
        }
        System.out.println(Integer.toHexString(dataTilePixels.length / 32).toUpperCase());
        byte[] buff = readBytes(gameSourcePath + "skdisasm-master/Levels/AIZ/Tiles/Act 1 Fire Overlay.unc");
        byte[] padding = new byte[0x500 * 32 - dataTilePixels.length];
        for (int b = 0; b < padding.length; b++) {
            padding[b] = dataTilePixels[0];
        }
        dataTilePixels = concat(dataTilePixels, padding);
        dataTilePixels = concat(dataTilePixels, buff);

        System.out.println(">>>>> Tile pixels loaded.");

        /*dataTilePixelsAnim = new byte[0];
        for (int i = 0; i < 9; i++) {
            String normal = String.format(gameSourcePath + "skdisasm-master/Levels/%s/Animated Tiles/%s.bin", levelID, i + "");
            String actSpec = String.format(gameSourcePath + "skdisasm-master/Levels/%s/Animated Tiles/Act" + ACT + " %s.bin", levelID, i + "");

            if (new File( normal ).exists())
                dataTilePixelsAnim = concat(dataTilePixelsAnim, (readBytes( normal )));
            else if (new File( actSpec ).exists())
                dataTilePixelsAnim = concat(dataTilePixelsAnim, (readBytes( actSpec )));
        }*/

        dataTilePixelsAnim = new byte[0];
        File folder = new File(String.format(gameSourcePath + "skdisasm-master/Levels/%s/Animated Tiles/", levelID));
        File[] listOfFiles = folder.listFiles();
        if (listOfFiles != null) {
            for (int i = 0; i < listOfFiles.length; i++) {
                if (listOfFiles[i].isFile()) {
                    String name = listOfFiles[i].getName();
                    if (ACT == 1 && name.contains("Act2"))
                        continue;
                    if (ACT == 2 && name.contains("Act1"))
                        continue;
                    dataTilePixelsAnim = concat(dataTilePixelsAnim, (readBytes(listOfFiles[i].getAbsolutePath())));
                }
            }
        }

        dataCollisionIndex = readBytes(rootD + ini.get(sect + "colind").substring(3));
        System.out.println("Loading from file: " + (rootD + ini.get(sect + "colind").substring(3)));

        if (ini.get(sect + "colindsz") != null) {
            dataCollisionDataSize = 2;
        }
        else {
            dataCollisionDataSize = 1;
        }
        System.out.println("dataCollisionDataSize: " + dataCollisionDataSize);

        dataCollisionHeights = readBytes(gameSourcePath + "skdisasm-master/Levels/Misc/Height Maps.bin");
        dataCollisionAngles = readBytes(gameSourcePath + "skdisasm-master/Levels/Misc/angles.bin");

        try {
            new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Rings)/" + ACT + ".bin").getParentFile().mkdirs();
            Files.copy(Paths.get(gameSourcePath + "skdisasm-master/Levels/" + levelID + "/Ring Pos/" + ACT + ".bin"), Paths.get(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Rings)/" + ACT + ".bin"));
        } catch (IOException e) {
            //e.printStackTrace();
        }

        System.out.println("Data Load Time: " + (System.currentTimeMillis() - sysTime) / 1000 + " seconds.");

        new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Sprites/TILES" + ACT + ".bin").getParentFile().mkdirs();

        new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Palettes/PAL" + ACT + ".bin").getParentFile().mkdirs();

        GeneratePaletteImage(bPalette, gameSourcePath + "source/Resource/Stages/" + levelID + "/Palettes/PAL" + ACT + "_DRY.png");
        if (bPaletteWater.length > 0)
            GeneratePaletteImage(bPaletteWater, gameSourcePath + "source/Resource/Stages/" + levelID + "/Palettes/PAL" + ACT + "_WET.png");
        else
            GeneratePaletteImage(bPalette, gameSourcePath + "source/Resource/Stages/" + levelID + "/Palettes/PAL" + ACT + "_WET.png");

        GenerateTileIDs(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Tiles)/" + ACT + ".bin");
        GenerateChunkCollisions(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Chunks)/" + ACT + ".bin");

        GenerateStaticTilesSheet(gameSourcePath + "S3Edit/", levelID, ACT);
        try {
            Files.copy(Paths.get(gameSourcePath + "S3Edit/" + levelID + "_TILES" + ACT + ".png"), Paths.get(gameSourcePath + "source/Resource/Stages/" + levelID + "/Sprites/TILES" + ACT + ".png"));
        } catch (IOException e) {
            //e.printStackTrace();
        }

        new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Music/" + ACT + ".bin").getParentFile().mkdirs();


        ensureFileWillBeThere(gameSourcePath + "source/res/" + levelID + "/objects/" + ACT + ".bin");
        try {
            new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Objects)/" + ACT + ".bin").getParentFile().mkdirs();
            Files.copy(Paths.get(gameSourcePath + "source/res/" + levelID + "/objects/" + ACT + ".bin"), Paths.get(gameSourcePath + "source/Resource/Stages/" + levelID + "/Data (Objects)/" + ACT + ".bin"));
        } catch (IOException e) {
            //e.printStackTrace();
        }
        byte[] objects = readBytes(gameSourcePath + "source/res/" + levelID + "/objects/" + ACT + ".bin");
        for (int x = 0; x < objects.length; x += 6) {
            Obj obj = new Obj();

            obj.X = (objects[x + 0] & 0xFF) * 0x100 + (objects[x + 1] & 0xFF);
            obj.Y = ((objects[x + 2] & 0xFF) * 0x100 + (objects[x + 3] & 0xFF)) & 0xFFF;
            obj.Priority = ((objects[x + 2] & 0xFF) & 0x80) == 0x80 ? true : false;
            obj.DoVerticalFlip = ((objects[x + 2] & 0xFF) & 0x40) == 0x40 ? true : false;
            obj.DoHorizontalFlip = ((objects[x + 2] & 0xFF) & 0x20) == 0x20 ? true : false;
            obj.ID = (objects[x + 4] & 0xFF);
            obj.SubType = (objects[x + 5] & 0xFF);

            liveDataObjects.add(obj);
        }


        GenerateSourceFiles(gameSourcePath + "source/Resource/Stages/" + levelID + "/");


        // Load Proper Images
        try {
            String U = "";
            //if (!paletted)
            //U = "U";

            if (!new File(gameSourcePath + "S3Edit/" + levelID + "" + ACT + "_" + U + "ChunksFront.png").exists()) {
                GenerateChunkImages(gameSourcePath + "S3Edit/", levelID, ACT);
            }
            tempChunkFUnpaletted = SwingFXUtils.toFXImage(ImageIO.read(new File(gameSourcePath + "S3Edit/" + levelID + "" + ACT + "_" + U + "ChunksFront.png")), null);
            tempChunkBUnpaletted = SwingFXUtils.toFXImage(ImageIO.read(new File(gameSourcePath + "S3Edit/" + levelID + "" + ACT + "_" + U + "ChunksBack.png")), null);
        } catch (Exception e) {
            e.printStackTrace();
        }

        populateObjectFacts();


        //// Begin
        scene.setFill(Color.OLDLACE);
        scene.getStylesheets().add("modena_dark.css");
        stage.setTitle("ImpostorEditor - " + gameSourcePath);
        stage.setScene(scene);
        stage.show();
    }

    public String GetASM() {
        return "Draw_BG:\n" +
                "\n" +
                "\t\tmovem.l\td5/a4-a5,-(sp)\n" +
                "\t\tlea\t(Camera_Y_pos_BG_copy).w,a6\n" +
                "\t\tjsr\tGet_DeformDrawPosVert(pc)\n" +
                "\t\tlea\t(Camera_Y_pos_BG_rounded).w,a5\n" +
                "\t\tjsr\tDraw_TileRow2(pc)\n" +
                "\t\tmovem.l\t(sp)+,d5/a4/a6\n" +
                "\t\tmove.w\t(Camera_Y_pos_BG_rounded).w,d6\n" +
                "\t\ttst.w\t(Camera_Y_pos_BG_copy).w\n" +
                "\t\tbpl.s\tDraw_BGNoVert\n" +
                "\t\tmove.w\t(Camera_Y_pos_BG_copy).w,d6\n" +
                "\t\tandi.w\t#-$10,d6\n" +
                "\n" +
                "Draw_BGNoVert:\n" +
                "\n" +
                "\t\tmove.w\td6,d1\n" +
                "\n" +
                "loc_4EE22:\n" +
                "\t\tsub.w\t(a4)+,d6\n" +
                "\t\tbmi.s\tloc_4EE32\n" +
                "\t\tmove.w\t(a6)+,d0\n" +
                "\t\tandi.w\t#-$10,d0\n" +
                "\t\tmove.w\td0,(a6)+\n" +
                "\t\tsubq.w\t#1,d5\n" +
                "\t\tbra.s\tloc_4EE22\n" +
                "; ---------------------------------------------------------------------------\n" +
                "\n" +
                "loc_4EE32:\n" +
                "\t\tneg.w\td6\n" +
                "\t\tlsr.w\t#4,d6\n" +
                "\t\tmoveq\t#$F,d4\n" +
                "\t\tsub.w\td6,d4\n" +
                "\t\tbcc.s\tloc_4EE40\n" +
                "\t\tmoveq\t#0,d4\n" +
                "\t\tmoveq\t#$F,d6\n" +
                "\n" +
                "loc_4EE40:\n" +
                "\n" +
                "\t\tmovem.w\td1/d4-d6,-(sp)\n" +
                "\t\tmovem.l\ta4/a6,-(sp)\n" +
                "\t\tlea\t2(a6),a5\n" +
                "\t\tjsr\tDraw_TileColumn(pc)\n" +
                "\t\tmovem.l\t(sp)+,a4/a6\n" +
                "\t\tmovem.w\t(sp)+,d1/d4-d6\n" +
                "\t\taddq.w\t#4,a6\n" +
                "\t\ttst.w\td4\n" +
                "\t\tbeq.s\tloc_4EE74\n" +
                "\t\tlsl.w\t#4,d6\n" +
                "\t\tadd.w\td6,d1\n" +
                "\t\tsubq.w\t#1,d5\n" +
                "\t\tmove.w\t(a4)+,d6\n" +
                "\t\tlsr.w\t#4,d6\n" +
                "\t\tmove.w\td4,d0\n" +
                "\t\tsub.w\td6,d4\n" +
                "\t\tbpl.s\tloc_4EE40\n" +
                "\t\tmove.w\td0,d6\n" +
                "\t\tmoveq\t#0,d4\n" +
                "\t\tbra.s\tloc_4EE40\n" +
                "; ---------------------------------------------------------------------------\n" +
                "\n" +
                "loc_4EE74:\n" +
                "\n" +
                "\t\tsubq.w\t#1,d5\n" +
                "\t\tbeq.s\tlocret_4EE82\n" +
                "\t\tmove.w\t(a6)+,d0\n" +
                "\t\tandi.w\t#-$10,d0\n" +
                "\t\tmove.w\td0,(a6)+\n" +
                "\t\tbra.s\tloc_4EE74\n" +
                "; ---------------------------------------------------------------------------\n" +
                "\n" +
                "locret_4EE82:\n" +
                "\t\trts";

    }

    public void ReadASM() {
        ArrayList<Routine> routines = new ArrayList<>();
        Routine currentRoutine = null;
        String input = GetASM();
        String[] lines = input.split("\n");
        for (int i = 0; i < lines.length; i++) {
            String line = lines[i].trim();
            if (line.isEmpty()) {
                // Do nothing
            } else if (line.charAt(0) == ';') {
                // Do nothing, it's a comment
            } else if (line.contains(":")) {
                if (currentRoutine != null)
                    routines.add(currentRoutine);

                currentRoutine = new Routine();
                currentRoutine.label = line.substring(0, line.indexOf(":"));
            } else {
                String[] chunks = line.split("\\s");
                String[] typeSplit = chunks[0].split("\\.");

                String[] argumentSplit = new String[0];
                if (chunks.length > 1) {
                    argumentSplit = chunks[1].split(",");
                }

                String type = typeSplit[0];
                String subtype = "";
                String[] arguments = new String[argumentSplit.length];
                if (typeSplit.length > 1) subtype = typeSplit[1];
                for (int n = 0; n < argumentSplit.length; n++) {
                    String arg = argumentSplit[n];
                    boolean number = false;
                    String value = "";
                    if (arg.charAt(0) == '#' &&
                            (   // If hexadecimal
                                    (arg.charAt(1) == '$' && (Character.isDigit(arg.charAt(2)) || (arg.charAt(2) >= 'A' && arg.charAt(2) <= 'F'))
                                    ) ||
                                            // If normal number
                                            (arg.charAt(1) != '$' && Character.isDigit(arg.charAt(1)))
                            )) {
                        if (arg.charAt(1) == '$') {
                            value = "0x" + Integer.toHexString(Integer.parseInt(arg.substring(2), 16)).toUpperCase();
                        } else {
                            value = "0x" + Integer.toHexString(Integer.parseInt(arg.substring(1), 10)).toUpperCase();
                        }
                    } else if (arg.contains("(") && arg.contains(")") && !arg.contains(".")) {
                        String[] sides = arg.split("\\(");
                        String index = "";
                        boolean override = false;
                        if (sides[0].length() == 0) {
                            index = "0x0";
                        } else if (sides[0].charAt(0) == '$') {
                            index = "0x" + Integer.toHexString(Integer.parseInt(sides[0].substring(1), 16)).toUpperCase();
                        } else if (sides[0].charAt(0) == '-') {
                            index = "-1";
                        } else if (sides[0].charAt(0) == '+') {
                            index = "+1";
                        } else if (Character.isDigit(sides[0].charAt(0))) {
                            try {
                                index = "0x" + Integer.toHexString(Integer.parseInt(sides[0].substring(0), 10)).toUpperCase();
                            } catch (Exception e) {
                                System.out.println("ERROR: " + sides[0].substring(0) + " /// " + line);
                            }
                        } else {
                            override = true;
                        }
                        value = sides[1].substring(0, sides[1].length() - 1).toUpperCase() + "{" + index + "}";

                        if (override) {
                            value = sides[0];
                        }
                    } else {
                        value = arg;
                    }
                    arguments[n] = value;
                }

                Instruction instruction = new Instruction();
                instruction.type = type;
                instruction.subtype = subtype;
                instruction.arguments = arguments;

                currentRoutine.instructions.add(instruction);
            }
        }
        if (currentRoutine != null)
            routines.add(currentRoutine);

        ConvertASM(routines);
    }

    public String NamedVariable(String var) {
        HashMap<String, String> codes = new HashMap<>();
        codes.put("VARIABLE0", "");
        codes.put("VARIABLE1", "");
        codes.put("VARIABLE2", "");
        codes.put("VARIABLE3", "");
        codes.put("VARIABLE4", "");
        codes.put("VARIABLE5", "");
        codes.put("VARIABLE6", "");
        codes.put("VARIABLE7", "");

        String str = codes.get(var);
        if (str != null)
            if (!str.isEmpty())
                return str;

        return var;
    }

    public String AddressIndexToObjectDef(String input, ArrayList<Routine> routines) {
        String output = input;

        for (int r = 0; r < routines.size(); r++) {
            Routine routine = routines.get(r);
            if (input.startsWith("#")) {
                if (input.substring(1).equalsIgnoreCase(routine.label)) {
                    return routine.label + "()";
                }
            } else {
                if (input.equalsIgnoreCase(routine.label)) {
                    return routine.label + "()";
                }
            }
        }

        if (input.startsWith("d") && Character.isDigit(input.charAt(1)))
            return NamedVariable("VARIABLE" + input.charAt(1));

        if (!input.startsWith("A"))
            return input;

        String obj = "";

        if (input.substring(0, 2).equalsIgnoreCase(addrPlayer)) {
            obj = "Player";
        } else if (input.substring(0, 2).equalsIgnoreCase(addrObject)) {
            obj = "Object";
        }

        if (!obj.isEmpty()) {
            String where = input.substring(3, input.indexOf("}"));
            String var = where;
            HashMap<String, String> codes = new HashMap<>();
            codes.put("0x0", "Update");
            codes.put("0x4", "RenderFlags");
            codes.put("0x6", "HalfHeight");
            codes.put("0x7", "HalfWidth");
            codes.put("0x8", "SpritePriority");
            codes.put("0x9", "SpritePriority");
            codes.put("0xA", "StartingArtBlock");
            codes.put("0xB", "StartingArtBlock");
            codes.put("0xC", "SpriteMappings");
            codes.put("0xD", "SpriteMappings");
            codes.put("0xE", "SpriteMappings");
            codes.put("0xF", "SpriteMappings");
            codes.put("0x10", "X");
            codes.put("0x11", "X");
            codes.put("0x12", "SubX");
            codes.put("0x13", "SubX");
            codes.put("0x14", "Y");
            codes.put("0x15", "Y");
            codes.put("0x16", "SubY");
            codes.put("0x17", "SubY");
            codes.put("0x18", "XSpeed");
            codes.put("0x19", "XSpeed");
            codes.put("0x1A", "YSpeed");
            codes.put("0x1B", "YSpeed");
            codes.put("0x1C", "PotentialSpeed");
            codes.put("0x1D", "PotentialSpeed");
            codes.put("0x1E", "HalfHeight");
            codes.put("0x1F", "HalfWidth");
            codes.put("0x20", "AnimationNumber");
            codes.put("0x21", "RestartAnimationFlag");
            codes.put("0x22", "CurrentAnimationFrame");
            codes.put("0x23", "CurrentFrameInAnimScript");
            codes.put("0x24", "AnimationFrameDuration");
            codes.put("0x25", "AnimationCounter");
            codes.put("0x26", "Angle");
            codes.put("0x27", "SecondAngle");
            codes.put("0x28", "CollisionBitfield");
            codes.put("0x29", "CustomCollision");
            codes.put("0x2A", "Status");
            codes.put("0x2B", "ObjectShieldReactionBitfield");
            codes.put("0x2C", "Subtype");
            //codes.put("0x2D", "XXXXXXXXXXX");
            codes.put("0x2E", "Counter");
            codes.put("0x2F", "Counter");

            if (obj.equals("Player")) {
                codes.put("0x25", "FlyingFramesLeft");
                codes.put("0x2B", "SecondaryStatus");
                codes.put("0x2C", "SecondsOfAirLeft"); // Usually $1E; it decrements every second while the player is underwater. Beeps on $18, $13, and $0E. Countdown starts on $0B.
                codes.put("0x2D", "InvertFlippingFlag");
                codes.put("0x2E", "ObjectControlFlagBit7JumpOut");
                codes.put("0x2F", "DoubleJumpFlag");
                codes.put("0x30", "FlipRevolutionsLeft");
                codes.put("0x31", "FlipRevolutionsPerFrameDividedBy256");
                codes.put("0x32", "HorizontalControlLock");
                codes.put("0x33", "HorizontalControlLock");
                codes.put("0x34", "InvurlnerabilityTime");
                codes.put("0x35", "InvincibilityTimeLeft");
                codes.put("0x36", "SpeedShoesTimeLeft");
                codes.put("0x37", "");
                codes.put("0x38", "CharacterID");
                codes.put("0x39", "SXXXXXXXXXX");
                codes.put("0x3A", "XXXXXXXXXXX");
                codes.put("0x3B", "XXXXXXXXXXX");
                codes.put("0x3C", "XXXXXXXXXXX");
                codes.put("0x3D", "XXXXXXXXXXX");
                codes.put("0x3E", "XXXXXXXXXXX");
                codes.put("0x3F", "XXXXXXXXXXX");

                /*
                codes.put("0x20", "XXXXXXXXXXX");
                codes.put("0x21", "XXXXXXXXXXX");
                codes.put("0x22", "XXXXXXXXXXX");
                codes.put("0x23", "XXXXXXXXXXX");
                codes.put("0x24", "XXXXXXXXXXX");
                codes.put("0x25", "XXXXXXXXXXX");
                codes.put("0x26", "XXXXXXXXXXX");
                codes.put("0x27", "XXXXXXXXXXX");
                codes.put("0x28", "XXXXXXXXXXX");
                codes.put("0x29", "SXXXXXXXXXX");
                codes.put("0x2A", "XXXXXXXXXXX");
                codes.put("0x2B", "XXXXXXXXXXX");
                codes.put("0x2C", "XXXXXXXXXXX");
                codes.put("0x2D", "XXXXXXXXXXX");
                codes.put("0x2E", "XXXXXXXXXXX");
                codes.put("0x2F", "XXXXXXXXXXX");*/
            }

            if (codes.get(var) != null)
                var = codes.get(var);

            output = obj + "." + var;
        }


        return output;
    }

    public void ConvertASM(ArrayList<Routine> routines) {
        System.out.println("// Angle = D0; GetSineCosine(Angle) -> D0 = sin(Angle), D1 = cos(Angle)");
        for (int r = 0; r < routines.size(); r++) {
            Routine routine = routines.get(r);
            System.out.println("void " + routine.label + " () {");
            for (int i = 0; i < routine.instructions.size(); i++) {
                boolean skipNext = false;
                Instruction instruction = routine.instructions.get(i);

                switch (instruction.type) {
                    // Basic Maths
                    case "move":
                    case "moveq":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " = " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "add":
                    case "addi":
                    case "addq":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " += " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "sub":
                    case "subi":
                    case "subq":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " -= " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "muls":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " *= " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "divs":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " *= " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "and":
                    case "andi":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " &= " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    // Stranger maths
                    case "neg":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[0], routines) + " = -" + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "asl":
                    case "lsl":
                        String var = AddressIndexToObjectDef(instruction.arguments[1], routines);
                        String v = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        String uhhh = "FF";
                        if (instruction.subtype.equalsIgnoreCase("w"))
                            uhhh = "FFFF";
                        if (instruction.subtype.equalsIgnoreCase("l"))
                            uhhh = "FFFFFFFF";
                        String additional = " + ((0x" + uhhh + " << " + v + ") ^ 0x" + uhhh + ")";
                        if (!false)
                            additional = "";
                        System.out.println("    " + var + " = ((" + var + " << " + v + ")" + additional + ") & 0x" + uhhh + ";");
                        break;

                    case "asr":
                    case "lsr":
                        var = AddressIndexToObjectDef(instruction.arguments[1], routines);
                        v = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        uhhh = "FF";
                        if (instruction.subtype.equalsIgnoreCase("w"))
                            uhhh = "FFFF";
                        if (instruction.subtype.equalsIgnoreCase("l"))
                            uhhh = "FFFFFFFF";
                        additional = " + ((0x" + uhhh + " >> " + v + ") ^ 0x" + uhhh + ")";
                        if (!false)
                            additional = "";
                        System.out.println("    " + var + " = ((" + var + " >> " + v + ")" + additional + ") & 0x" + uhhh + ";");
                        break;

                    // More Weird
                    case "swap":
                        var = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        System.out.println("    " + var + " = ((" + var + " & 0xFFFF0000) / 0x10000 + (" + var + " & 0xFFFF));");
                        break;

                    // Comparing and IF statements
                    case "cmp":
                    case "cmpi":
                        System.out.println("    IF = " + AddressIndexToObjectDef(instruction.arguments[1], routines) + " - " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "tst":
                        System.out.println("    IF = " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "btst":
                        var = AddressIndexToObjectDef(instruction.arguments[1], routines);
                        v = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        System.out.println("    IF = ((" + var + " >> " + v + ") & 0x1) != 0); // Check bit " + v + "");
                        break;

                    case "bset":
                        var = AddressIndexToObjectDef(instruction.arguments[1], routines);
                        v = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        System.out.println("    " + var + " |= (0x1 << " + v + "); // Set bit " + v + "");
                        break;

                    case "bclr":
                        var = AddressIndexToObjectDef(instruction.arguments[1], routines);
                        v = AddressIndexToObjectDef(instruction.arguments[0], routines);
                        System.out.println("    " + var + " &= (0x1 << " + v + "); // Clear bit " + v + "");
                        break;

                    case "bne":
                        System.out.println("    if (IF != 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "beq":
                        System.out.println("    if (IF != 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "bmi":
                    case "bcs":
                    case "blo":
                        System.out.println("    if (IF < 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "bpl":
                    case "bhi":
                        System.out.println("    if (IF > 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "bls":
                        System.out.println("    if (IF <= 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    case "bcc":
                    case "bhs":
                        System.out.println("    if (IF >= 0)\n        " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ";");
                        break;

                    // Subroutines
                    case "bsr":
                    case "jsr":
                        System.out.println("    goto " + AddressIndexToObjectDef(instruction.arguments[0], routines) + "();");
                        break;

                    case "bra":
                    case "jmp":
                        System.out.println("    " + AddressIndexToObjectDef(instruction.arguments[0], routines) + "; return; ");
                        break;

                    case "rts":
                        System.out.println("    returnToSubRoutine; ");
                        break;


                    case "lea":
                        System.out.println("    loadIntoAddress(" + AddressIndexToObjectDef(instruction.arguments[1], routines) + ", " + AddressIndexToObjectDef(instruction.arguments[0], routines) + ");");
                        break;

                    default:
                        unimplementedOnes.add(instruction.type);
                        break;
                }

                if (skipNext) {
                    i++;
                }
            }
            if (r < routines.size() - 1) {
                System.out.println("    " + routines.get(r + 1).label + "();");
            }
            System.out.println("}");
        }
        System.out.println(unimplementedOnes.toString());

    }

    private void selectChunk() {
        if (currentEditMode == EditMode.CHUNKS) {
            int x = Math.max(0, (viewX + mouseX) / 128);
            int y = Math.max(0, (viewY + mouseY) / 128);
            if (chunkHandActive) {
                if (chunkHandX != x || chunkHandY != y) {
                    liveDataChunks.get(chunkHandX).set(chunkHandY, chunkHandData);
                    chunkHandX = x;
                    chunkHandY = y;
                    chunkHandData = liveDataChunks.get(x).get(y);
                    liveDataChunks.get(x).set(y, 0);
                    chunkHandUp = true;
                } else {
                    chunkHandUp = true;
                }
            } else {
                chunkHandActive = true;
                chunkHandX = x;
                chunkHandY = y;
                chunkHandData = liveDataChunks.get(x).get(y);
                liveDataChunks.get(x).set(y, 0);
                chunkHandUp = true;
            }
        }
    }

    private void moveChunk() {
        if (chunkHandActive) {
            if (chunkHandUp) {
                int x = Math.max(0, (viewX + mouseX) / 128);
                int y = Math.max(0, (viewY + mouseY) / 128);
                chunkHandX = x;
                chunkHandY = y;
            }
        }
    }

    private void dropChunk() {
        if (chunkHandActive) {
            chunkHandUp = false;
        }
    }

    private void selectObject() {
        for (int i = 0; i < liveDataObjects.size(); i++) {
            Obj obj = liveDataObjects.get(i);
            int w = 24;
            int h = 24;
            if (objectFacts.get(obj.ID) != null) {
                w = objectFacts.get(obj.ID).getW(obj);
                h = objectFacts.get(obj.ID).getH(obj);
            }
            if (mouseX >= obj.X - viewX - w / 2 &&
                    mouseY >= obj.Y - viewY - h / 2 &&
                    mouseX < obj.X - viewX + w / 2 &&
                    mouseY < obj.Y - viewY + h / 2) {

                if (objectFacts.get(obj.ID) != null) {
                    objectPropName.setText(objectFacts.get(obj.ID).name);
                } else {
                    objectPropName.setText("[No ObjectEntry found.]");
                }
                objectPropID.setEditable(false);
                objectPropID.setText(Integer.toHexString(obj.ID).toUpperCase());

                objectPropX.setText(Integer.toHexString(obj.X).toUpperCase());
                objectPropY.setText(Integer.toHexString(obj.Y).toUpperCase());

                objectPropSubType.setText(Integer.toHexString(obj.SubType).toUpperCase());

                break;
            }
        }
    }

    private void refreshCanvas() {
        GraphicsContext gc = canvas.getGraphicsContext2D();

        SepiaTone mb = new SepiaTone();


        gc.setFill(Color.BLACK);
        gc.fillRect(0, 0, canvas.getWidth() - 0, canvas.getHeight() - 0);

        if (currentEditMode == EditMode.BACKGROUND) {
            for (int
                 x = Math.max(0, viewX / 128);
                 x < Math.min(liveDataLevelWidth, (viewX + canvas.getWidth()) / 128);
                 x++) {
                for (int
                     y = Math.max(0, viewY / 128);
                     y < Math.min(liveDataLevelHeight, (viewY + canvas.getHeight()) / 128);
                     y++) {
                    int us = liveDataChunks.get(x).get(y);
                    int sx = us % 16;
                    int sy = us / 16;

                    gc.drawImage(tempChunkBUnpaletted, sx * 128, sy * 128, 128, 128, x * 128 - viewX, y * 128 - viewY, 128, 128);
                }
            }
            return;
        }

        if (tempChunkFUnpaletted != null) {
            for (int
                 x = Math.max(0, viewX / 128);
                 x < Math.min(liveDataLevelWidth, (viewX + canvas.getWidth()) / 128);
                 x++) {
                for (int
                     y = Math.max(0, viewY / 128);
                     y < Math.min(liveDataLevelHeight, (viewY + canvas.getHeight()) / 128);
                     y++) {
                    int us = liveDataChunks.get(x).get(y);
                    int sx = us % 16;
                    int sy = us / 16;

                    gc.drawImage(tempChunkBUnpaletted, sx * 128, sy * 128, 128, 128, x * 128 - viewX, y * 128 - viewY, 128, 128);
                }
            }

            if (!showObjectsOverAll) {
                for (int i = 0; i < liveDataObjects.size(); i++) {
                    Obj obj = liveDataObjects.get(i);
                    if (objectFacts.containsKey(obj.ID)) {
                        objectFacts.get(obj.ID).Render(gc, obj, viewX, viewY);
                    } else {
                        gc.setFill(Paint.valueOf("#00FF00"));
                        gc.fillRoundRect(obj.X - 8 - viewX, obj.Y - 8 - viewY, 16, 16, 16, 16);

                        gc.setFill(Paint.valueOf("#FF00FF"));
                        gc.fillText(Integer.toHexString(obj.ID).toUpperCase(), obj.X - viewX, obj.Y - viewY);
                    }
                }
            }

            for (int
                 x = Math.max(0, viewX / 128);
                 x < Math.min(liveDataLevelWidth, (viewX + canvas.getWidth()) / 128);
                 x++) {
                for (int
                     y = Math.max(0, viewY / 128);
                     y < Math.min(liveDataLevelHeight, (viewY + canvas.getHeight()) / 128);
                     y++) {
                    int us = liveDataChunks.get(x).get(y);
                    int sx = us % 16;
                    int sy = us / 16;

                    gc.drawImage(tempChunkFUnpaletted, sx * 128, sy * 128, 128, 128, x * 128 - viewX, y * 128 - viewY, 128, 128);

                    if (x == Math.max(0, viewX / 128) && false) {
                        us = us * 128;

                        for (int i = us; i < us + 128; i += 2) {
                            int word = (dataTiledata[i] & 0xFF) * 0x100 + (dataTiledata[i + 1] & 0xFF);
                            String bitmask = Integer.toBinaryString(word);
                            bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                            int solid1 = Integer.parseInt(bitmask.substring(0, 2), 2);
                            int solid2 = Integer.parseInt(bitmask.substring(2, 4), 2);
                            int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
                            int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
                            int til = Integer.parseInt(bitmask.substring(6), 2);

                            //par.bCollisionIndex


                            int heightInd = angles[til * 2 + 1] & 0xFF;
                            if (1 == 1) {
                                heightInd = angles[til * 2 + 1 + 0x600] & 0xFF;
                            }

                            gc.fillText(Integer.toHexString(heightInd).toUpperCase(), x * 128 - viewX, y * 128 + 16 - viewY);
                        }
                    }
                }
            }

            if (showObjectsOverAll) {
                for (int i = 0; i < liveDataObjects.size(); i++) {
                    Obj obj = liveDataObjects.get(i);
                    if (objectFacts.containsKey(obj.ID)) {
                        objectFacts.get(obj.ID).Render(gc, obj, viewX, viewY);
                    } else {
                        gc.setFill(Paint.valueOf("#00FF00"));
                        gc.fillRoundRect(obj.X - 8 - viewX, obj.Y - 8 - viewY, 16, 16, 16, 16);

                        gc.setFill(Paint.valueOf("#FF00FF"));
                        gc.fillText(Integer.toHexString(obj.ID).toUpperCase(), obj.X - viewX, obj.Y - viewY);
                    }
                }
            }

            if (currentEditMode == EditMode.CHUNKS) {
                int x = Math.max(0, (viewX + mouseX) / 128);
                int y = Math.max(0, (viewY + mouseY) / 128);
                gc.setFill(Paint.valueOf("#FFF"));
                gc.setGlobalAlpha(0.5);
                gc.fillRect(x * 128 - viewX, y * 128 - viewY, 128, 128);
                gc.setGlobalAlpha(1);

                int us = liveDataChunks.get(x).get(y);
                gc.setFill(Paint.valueOf("#000"));
                gc.fillText(Integer.toHexString(us).toUpperCase(), x * 128 - viewX + 4, y * 128 - viewY + 16);

                if (chunkHandActive) {
                    us = chunkHandData;
                    int sx = us % 16;
                    int sy = us / 16;
                    x = chunkHandX;
                    y = chunkHandY;

                    gc.drawImage(tempChunkBUnpaletted, sx * 128, sy * 128, 128, 128, x * 128 - viewX, y * 128 - viewY, 128, 128);
                    gc.drawImage(tempChunkFUnpaletted, sx * 128, sy * 128, 128, 128, x * 128 - viewX, y * 128 - viewY, 128, 128);

                    gc.setFill(Paint.valueOf("#4191F2"));
                    gc.setGlobalAlpha(0.5);
                    gc.fillRect(x * 128 - viewX, y * 128 - viewY, 128, 128);
                    gc.setGlobalAlpha(1);
                    gc.setStroke(Paint.valueOf("#4191F2"));
                    gc.setLineWidth(3);
                    gc.strokeRect(x * 128 - viewX, y * 128 - viewY, 128, 128);

                    gc.setFill(Paint.valueOf("#FFF"));
                    gc.fillText(Integer.toHexString(us).toUpperCase(), x * 128 - viewX + 4, y * 128 - viewY + 16);
                }
            } else if (currentEditMode == EditMode.OBJECTS) {
                for (int i = 0; i < liveDataObjects.size(); i++) {
                    Obj obj = liveDataObjects.get(i);
                    int w = 24;
                    int h = 24;
                    if (objectFacts.get(obj.ID) != null) {
                        w = objectFacts.get(obj.ID).getW(obj);
                        h = objectFacts.get(obj.ID).getH(obj);
                    }
                    if (mouseX >= obj.X - viewX - w / 2 &&
                            mouseY >= obj.Y - viewY - h / 2 &&
                            mouseX < obj.X - viewX + w / 2 &&
                            mouseY < obj.Y - viewY + h / 2) {
                        gc.setFill(Paint.valueOf("#FFF"));
                        gc.setGlobalAlpha(0.5);
                        gc.fillRect(obj.X - viewX - w / 2, obj.Y - viewY - h / 2, w, h);
                        gc.setGlobalAlpha(1);
                        break;
                    }
                }
            }
        }


        if (bPalette != null) {
            for (int i = 0; i < bPalette.length; i += 2) {
                int r = (bPalette[i + 1] & 0xF) * 16 + (bPalette[i + 1] & 0xF);
                int g = (bPalette[i + 1] & 0xF0);
                int b = (bPalette[i] & 0xFF) * 16 + (bPalette[i] & 0xFF);
                int a = 255;

                int x = (i / 2) / 16; // palette row
                int y = (i / 2) % 16; // index

                gc.setFill(new Color(r / 255.0, g / 255.0, b / 255.0, a / 255.0));
                gc.fillRect(x * 8, y * 8 + 60, 8, 8);
            }
        }


        displayTilesOnCanvas();
    }

    public byte[] concat(byte[] a, byte[] b) {
        int aLen = a.length;
        int bLen = b.length;
        byte[] c = new byte[aLen + bLen];
        System.arraycopy(a, 0, c, 0, aLen);
        System.arraycopy(b, 0, c, aLen, bLen);
        return c;
    }

    private void GenerateStaticTilesSheet(String parentFolder, String lvl, int act) {
        BufferedImage tempStaticTiles = new BufferedImage(512, 256, BufferedImage.TYPE_INT_ARGB);
        Graphics2D cg = tempStaticTiles.createGraphics();

        boolean store = paletted;
        paletted = true;
        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 0);

        }
        paletted = store;

        this.tempStaticTiles = SwingFXUtils.toFXImage(tempStaticTiles, null);

        tempStaticTiles.flush();
        cg.dispose();


        tempStaticTiles = new BufferedImage(512, 256, BufferedImage.TYPE_INT_ARGB);
        cg = tempStaticTiles.createGraphics();

        byte[] dataTilePixelsBuf = dataTilePixels;
        dataTilePixels = dataTilePixelsAnim;

        store = paletted;
        paletted = true;
        for (int i = 0; i < dataTilePixelsAnim.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 0);

        }
        paletted = store;

        dataTilePixels = dataTilePixelsBuf;

        this.tempAnimatTiles = SwingFXUtils.toFXImage(tempStaticTiles, null);

        tempStaticTiles.flush();
        cg.dispose();


        BufferedImage finalSheet = new BufferedImage(512, 512, BufferedImage.TYPE_INT_ARGB);
        cg = finalSheet.createGraphics();

        store = paletted;
        paletted = false;

        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 0);
        }

        dataTilePixels = dataTilePixelsAnim;

        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8 + 256, i, false, false, 0);
        }

        /*tempActTileSheetPal = SwingFXUtils.toFXImage(finalSheet, null);

        cg.clearRect(0, 0, 512, 256);

        dataTilePixels = dataTilePixelsBuf;
        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 1);
        }
        tempActTileSheetPal1 = SwingFXUtils.toFXImage(finalSheet, null);

        cg.clearRect(0, 0, 512, 256);

        dataTilePixels = dataTilePixelsBuf;
        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 2);
        }
        tempActTileSheetPal2 = SwingFXUtils.toFXImage(finalSheet, null);

        cg.clearRect(0, 0, 512, 256);

        dataTilePixels = dataTilePixelsBuf;
        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 3);
        }
        tempActTileSheetPal3 = SwingFXUtils.toFXImage(finalSheet, null);*/

        try {
            ImageIO.write(finalSheet, "png", new File(parentFolder + lvl + "_TILES" + act + ".png"));
        } catch (Exception e) {
        }

        paletted = store;


        finalSheet.flush();
        cg.dispose();


        finalSheet = new BufferedImage(512, 512, BufferedImage.TYPE_INT_ARGB);
        cg = finalSheet.createGraphics();

        cg.clearRect(0, 0, 512, 512);

        store = paletted;
        paletted = true;

        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8, i, false, false, 0);
        }

        dataTilePixels = dataTilePixelsAnim;

        for (int i = 0; i < dataTilePixels.length / 32; i++) {
            int x = i % 64;
            int y = i / 64;
            drawTile(cg, x * 8, y * 8 + 256, i, false, false, 0);
        }

        paletted = store;

        dataTilePixels = dataTilePixelsBuf;

        tempActTileSheet = SwingFXUtils.toFXImage(finalSheet, null);

        finalSheet.flush();
        cg.dispose();


        finalSheet = new BufferedImage(512, 512, BufferedImage.TYPE_INT_ARGB);
        cg = finalSheet.createGraphics();

        // Creation of StageConfig
        DataOutputStream os = null;
        try {
            os = new DataOutputStream(new FileOutputStream(parentFolder + lvl + act + " StageConfig.bin"));

            // StageConfig format
            //
            // 0x00.w: Level Width in 16x16 tiles
            // 0x02.w: Level Height in 16x16 tiles
            //
            // Individual TileConfig (length: 0x22 bytes)
            // 0x00 - 0x10: Collision Heights 1
            // 0x10 - 0x20: Collision Heights 2
            // 0x20:        Tile Angle 1
            // 0x21:        Tile Angle 2

            System.out.println("Level Width: " + liveDataLevelWidth);
            System.out.println("Level Height: " + liveDataLevelHeight);

            os.writeShort(liveDataLevelWidth * 8);
            os.writeShort(liveDataLevelHeight * 8);

            for (int x = 0; x < liveDataLevelWidth; x++) {
                for (int y = 0; y < liveDataLevelHeight; y++) {
                    int us = liveDataChunks.get(x).get(y);
                    int sx = us % 16;
                    int sy = us / 16;

                    for (int i = 0; i < 128; i += 2) {
                        int word = (dataTiledata[us * 128 + i] & 0xFF) * 0x100 + (dataTiledata[us * 128 + i + 1] & 0xFF);
                        String bitmask = Integer.toBinaryString(word);
                        bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                        int solid1 = Integer.parseInt(bitmask.substring(0, 2), 2);
                        int solid2 = Integer.parseInt(bitmask.substring(2, 4), 2);
                        int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
                        int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
                        int til = Integer.parseInt(bitmask.substring(6), 2);

                        int tX = (i / 2) % 8;
                        int tY = (i / 2) / 8;

                        liveDataChunks16x16.get(x * 8 + tX).set(y * 8 + tY, word);
                    }
                }
            }

            for (int y = 0; y < liveDataLevelHeight * 8; y++) {
                for (int x = 0; x < liveDataLevelWidth * 8; x++) {
                    os.writeShort(liveDataChunks16x16.get(x).get(y));
                }
            }

            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            if (os != null) os.close();
        } catch (Exception e) {
        }

        // Creation of TileConfig
        os = null;
        try {
            os = new DataOutputStream(new FileOutputStream(parentFolder + lvl + act + " TileConfig.bin"));

            // TileConfig format
            //
            // Individual TileConfig (length: 0x22 bytes)
            // 0x00 - 0x10: Collision Heights 1
            // 0x10 - 0x20: Collision Heights 2
            // 0x20:        Tile Angle 1
            // 0x21:        Tile Angle 1

            System.out.println("16x16 Tile Count: " + dataBlocks.length / 8);

            os.writeShort(dataBlocks.length / 8);

            for (int i = 0; i < dataBlocks.length / 8; i += 1) {
                int overX = (i % 32) * 16;
                int overY = (i / 32) * 16;

                int til = i;
                int flipTX = 0;
                int flipTY = 0;

                // (til * 2 + 1 + 0x600) / dataCollisionDataSize
                // (til * 2 + 1) / dataCollisionDataSize

                int HeightIndex1 = dataCollisionIndex[(dataCollisionDataSize == 2 ? til * 2 + 1 + 0x600 : til * 2 + 1)] & 0xFF;
                int HeightIndex2 = dataCollisionIndex[(dataCollisionDataSize == 2 ? til * 2 + 1 : til * 2)] & 0xFF;

                for (int h = 0; h < 16; h++) {
                    os.writeByte(dataCollisionHeights[HeightIndex1 * 0x10 + h]);
                }
                for (int h = 0; h < 16; h++) {
                    os.writeByte(dataCollisionHeights[HeightIndex2 * 0x10 + h]);
                }
                os.writeByte(dataCollisionAngles[HeightIndex1]);
                os.writeByte(dataCollisionAngles[HeightIndex2]);

                for (int j = 0; j < 4; j++) {
                    if (til * 8 + j * 2 >= dataBlocks.length) continue;

                    int word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);

                    String bitmask = Integer.toBinaryString(word);
                    bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                    int priorit = Integer.parseInt(bitmask.substring(0, 1));
                    int CC = Integer.parseInt(bitmask.substring(1, 3), 2);
                    int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                    int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                    int ind = Integer.parseInt(bitmask.substring(5), 2);

                    int x = (j % 2) * 8;
                    int y = (j / 2) * 8;

                    if (flipTX == 1) {
                        flipX = 1 - flipX;
                        x = (1 - (j % 2)) * 8;
                    }
                    if (flipTY == 1) {
                        flipY = 1 - flipY;
                        y = (1 - (j / 2)) * 8;
                    }

                    boolean AnimTile = false;
                    for (AnimatedTiles a : dataAnimTiles) {
                        if (ind >= a.ID && ind < a.ID + a.Count) {
                            AnimTile = true;
                            break;
                        }
                    }
                    if (!AnimTile)
                        drawTile(cg, overX + x, overY + y, ind, flipX == 1, flipY == 1, CC);
                }
            }
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            if (os != null) os.close();
        } catch (Exception e) {
        }

        try {
            ImageIO.write(finalSheet, "png", new File(parentFolder + lvl + act + " 16x16Tiles.png"));
        } catch (Exception e) {
        }

        finalSheet.flush();
        cg.dispose();
    }

    private void displayTilesOnCanvas() {
        if (currentEditMode != EditMode.TILES)
            return;

        GraphicsContext gc = canvas.getGraphicsContext2D();

        gc.setFill(Color.MAGENTA);
        gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());

        if (tempStaticTiles != null) {
            for (int i = 0; i < dataTilePixels.length / 32; i++) {
                int x = i % 64;
                int y = i / 64;

                gc.drawImage(tempStaticTiles, x * 8, y * 8, 8, 8, 256 - viewX + x * 8, y * 8, 8, 8);
            }
        }

        gc.drawImage(tempStaticTiles, 256 - viewX + 512, 0);

        if (tempAnimatTiles != null) {
            for (int i = 0; i < dataTilePixelsAnim.length / 32; i++) {
                int x = i % 64;
                int y = i / 64;

                gc.drawImage(tempAnimatTiles, x * 8, y * 8, 8, 8, 256 - viewX + x * 8, 256 + y * 8, 8, 8);
            }
        }

        if (tempActTileSheet != null) {
            // gc.drawImage(tempActTileSheet, 0, 0, 512, 512, 256 - viewX, 0, 512, 512);

            for (int i = 0; i < dataTilePixels.length / 32; i++) {
                int x = (i % 64) * 8;
                int y = (i / 64) * 8;
                int xxx = Math.max(0, (viewX + mouseX));
                int yyy = Math.max(0, (viewY + mouseY));
                if (xxx >= 256 + x + 0 &&
                        yyy >= 000 + y + 0 &&
                        xxx < 256 + x + 8 &&
                        yyy < 000 + y + 8) {
                    gc.setFill(Paint.valueOf("#0F0"));
                    gc.setGlobalAlpha(0.5);
                    gc.fillRect(256 + x, y, 8, 8);
                    gc.setGlobalAlpha(1);

                    gc.setFill(Paint.valueOf("#000"));
                    gc.fillText(i + "", 20, canvas.getHeight() - 20);
                }
            }

            for (int i = 0; i < dataTilePixelsAnim.length / 32; i++) {
                int x = (i % 64) * 8;
                int y = (i / 64) * 8;
                int xxx = Math.max(0, (viewX + mouseX));
                int yyy = Math.max(0, (viewY + mouseY));
                if (xxx >= 256 + x + 0 &&
                        yyy >= 256 + y + 0 &&
                        xxx < 256 + x + 8 &&
                        yyy < 256 + y + 8) {
                    gc.setFill(Paint.valueOf("#0F0"));
                    gc.setGlobalAlpha(0.5);
                    gc.fillRect(256 + x, 256 + y, 8, 8);
                    gc.setGlobalAlpha(1);

                    gc.setFill(Paint.valueOf("#000"));
                    gc.fillText((i) + "", 20, canvas.getHeight() - 20);
                }
            }

            for (int u = 0; u < 1; u++) {
                gc.setFill(Paint.valueOf("#0F0"));
                gc.setGlobalAlpha(0.5);
                gc.fillRect(768, u * 256, 512, 512);
                gc.setGlobalAlpha(1);
            }

            for (int i = 0; i < dataBlocks.length / 8; i += 1) {
                int overX = (i % 32) * 16;
                int overY = (i / 32) * 16;

                int til = i;
                int flipTX = 0;
                int flipTY = 0;

                for (int j = 0; j < 4; j++) {
                    if (til * 8 + j * 2 >= dataBlocks.length) continue;

                    int word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);


                    String bitmask = Integer.toBinaryString(word);
                    bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                    int priorit = Integer.parseInt(bitmask.substring(0, 1));
                    int CC = Integer.parseInt(bitmask.substring(1, 3), 2);
                    int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                    int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                    int ind = Integer.parseInt(bitmask.substring(5), 2);

                    int x = (j % 2) * 8;
                    int y = (j / 2) * 8;

                    if (flipTX == 1) {
                        flipX = 1 - flipX;
                        x = (1 - (j % 2)) * 8;
                    }
                    if (flipTY == 1) {
                        flipY = 1 - flipY;
                        y = (1 - (j / 2)) * 8;
                    }

                    if (levelID.equals("CNZ") && false) {
                        if (ind >= 690 && ind < 708) {
                            ind = 2048 + 0 + ind - 690;
                        }
                        if (ind >= 708 && ind < 724) {
                            ind = 2048 + 144 + ind - 708;
                        }
                        if (ind >= 724 && ind < 756) {
                            ind = 2048 + 400 + ind - 724;
                        }
                        if (ind >= 756 && ind < 772) {
                            ind = 2048 + 656 + ind - 756;
                        }
                        if (ind >= 772 && ind < 776) {
                            ind = 2048 + 784 + ind - 772;
                        }
                        if (ind >= 776 && ind < 808) {
                            ind = 2048 + 876 + ind - 776;
                        }
                        if (ind >= 808 && ind < 828) {
                            ind = 2048 + 796 + ind - 808;
                        }
                    }

                    gc.save();
                    gc.translate(overX + 768 + x + 4 - viewX, overY + y + 4 - viewY);
                    gc.scale(flipX == 1 ? -1 : 1, flipY == 1 ? -1 : 1);
                    if (CC == 0)
                        gc.drawImage(tempActTileSheetPal, (ind % 64) * 8, (ind / 64) * 8, 8, 8, -4, -4, 8, 8);
                    else if (CC == 1)
                        gc.drawImage(tempActTileSheetPal1, (ind % 64) * 8, (ind / 64) * 8, 8, 8, -4, -4, 8, 8);
                    else if (CC == 2)
                        gc.drawImage(tempActTileSheetPal2, (ind % 64) * 8, (ind / 64) * 8, 8, 8, -4, -4, 8, 8);
                    else if (CC == 3)
                        gc.drawImage(tempActTileSheetPal3, (ind % 64) * 8, (ind / 64) * 8, 8, 8, -4, -4, 8, 8);
                    gc.restore();

                    int xxx = Math.max(0, (viewX + mouseX));
                    int yyy = Math.max(0, (viewY + mouseY));
                    if (xxx >= overX + 768 + x + 0 &&
                            yyy >= overY + y + 0 &&
                            xxx < overX + 768 + x + 8 &&
                            yyy < overY + y + 8) {
                        gc.setFill(Paint.valueOf("#0F0"));
                        gc.setGlobalAlpha(0.5);
                        gc.fillRect(overX + 768 + x - viewX, overY + y - viewY, 8, 8);
                        gc.setGlobalAlpha(1);

                        //gc.setFill(Paint.valueOf("#000"));
                        //gc.fillText(ind + "", 20, canvas.getHeight() - 20);
                        guiStatusLabel.setText("Current Tile ID: " + ind + " (0x" + Integer.toHexString(ind).toUpperCase() + "); Current Chunk ID: " +
                                (i / 128) + " (0x" + Integer.toHexString(i / 128).toUpperCase() + ")");
                    }
                }
            }

            /*
            for (int i = 0; i < dataTiledata.length * 0; i += 2) {
                int overX = (((i / 2) / 64) % 16) * 128;
                int overY = (((i / 2) / 64) / 16) * 128;

                int word = (dataTiledata[i] & 0xFF) * 0x100 + (dataTiledata[i + 1] & 0xFF);
                String bitmask = Integer.toBinaryString(word);
                bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                //int priority = Integer.parseInt(bitmask.substring(0, 1));
                int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
                int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
                int til = Integer.parseInt(bitmask.substring(6), 2);

                for (int j = 0; j < 4; j++) {
                    if (til * 8 + j * 2 >= dataBlocks.length) continue;

                    word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);


                    bitmask = Integer.toBinaryString(word);
                    bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                    int priorit = Integer.parseInt(bitmask.substring(0, 1));
                    int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                    int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                    int ind = Integer.parseInt(bitmask.substring(5), 2);

                    int x = (j % 2) * 8 + ((i / 2) % 8) * 16;
                    int y = (j / 2) * 8 + ((i / 2) / 8) % 8 * 16;

                    if (flipTX == 1) {
                        flipX = 1 - flipX;
                        x = (1 - (j % 2)) * 8 + ((i / 2) % 8) * 16;
                    }
                    if (flipTY == 1) {
                        flipY = 1 - flipY;
                        y = (1 - (j / 2)) * 8 + ((i / 2) / 8) % 8 * 16;
                    }

                    if (levelID.equals("CNZ") && false) {
                        if (ind >= 690 && ind < 708) {
                            ind = 2048 + 0 + ind - 690;
                        }
                        if (ind >= 708 && ind < 724) {
                            ind = 2048 + 144 + ind - 708;
                        }
                        if (ind >= 724 && ind < 756) {
                            ind = 2048 + 400 + ind - 724;
                        }
                        if (ind >= 756 && ind < 772) {
                            ind = 2048 + 656 + ind - 756;
                        }
                        if (ind >= 772 && ind < 776) {
                            ind = 2048 + 784 + ind - 772;
                        }
                        if (ind >= 776 && ind < 808) {
                            ind = 2048 + 876 + ind - 776;
                        }
                        if (ind >= 808 && ind < 828) {
                            ind = 2048 + 796 + ind - 808;
                        }
                    }

                    gc.save();
                        gc.translate(overX + 768 + x + 4 - viewX, overY + y + 4 - viewY);
                        gc.scale(flipX == 1 ? -1 : 1, flipY == 1 ? -1 : 1);
                            gc.drawImage(tempActTileSheet, (ind % 64) * 8, (ind / 64) * 8, 8, 8, -4, -4, 8, 8);
                    gc.restore();

                    int xxx = Math.max(0, (viewX + mouseX));
                    int yyy = Math.max(0, (viewY + mouseY));
                    if (xxx >= overX + 768 + x + 0 &&
                        yyy >= overY + y + 0 &&
                        xxx <  overX + 768 + x + 8 &&
                        yyy <  overY + y + 8) {
                        gc.setFill(Paint.valueOf("#0F0"));
                        gc.setGlobalAlpha(0.5);
                            gc.fillRect(overX + 768 + x - viewX, overY + y - viewY, 8, 8);
                        gc.setGlobalAlpha(1);

                        //gc.setFill(Paint.valueOf("#000"));
                        //gc.fillText(ind + "", 20, canvas.getHeight() - 20);
                        guiStatusLabel.setText("Current Tile ID: " + ind + " (0x" + Integer.toHexString(ind).toUpperCase() + "); Current Chunk ID: " +
                                (i / 128) + " (0x" + Integer.toHexString(i / 128).toUpperCase() + ")");
                    }
                }
            }
            //*/
        }
    }

    private void GeneratePaletteImage(byte[] pal, String filename) {
        if (pal == null) return;

        BufferedImage finalSheet = new BufferedImage(4, 16, BufferedImage.TYPE_INT_ARGB);
        Graphics2D cg = finalSheet.createGraphics();

        if (pal != null) {
            for (int i = 0; i < pal.length; i += 2) {
                int r = (pal[i + 1] & 0xF) * 16 + (pal[i + 1] & 0xF);
                int g = (pal[i + 1] & 0xF0);
                int b = (pal[i] & 0xFF) * 16 + (pal[i] & 0xFF);
                int a = 255;

                int x = (i / 2) / 16;
                int y = (i / 2) % 16;

                cg.setColor(new java.awt.Color(r, g, b, a));
                cg.fillRect(x, y, 1, 1);

                //System.out.println("Array[" + (i / 2) + "] = { " + (r / 255.0) + "f, " + (g / 255.0) + "f, " + (b / 255.0) + "f, 1.0f };");
            }
        }

        try {
            ImageIO.write(finalSheet, "png", new File(filename));
        } catch (Exception e) {
        }

        finalSheet.flush();
        cg.dispose();
    }

    private void GenerateTileIDs(String filename) {
        int max = dataTiledata.length;
        //max = 128;

        new File(filename).getParentFile().mkdirs();

        int[] words = new int[16 * 16 * 16 * 16];

        for (int i = 0; i < max; i += 2) {
            int word = (dataTiledata[i] & 0xFF) * 0x100 + (dataTiledata[i + 1] & 0xFF);
            String bitmask = Integer.toBinaryString(word);
            bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
            int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
            int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
            int til = Integer.parseInt(bitmask.substring(6), 2);

            int us = i / 128;
            int usX = (us % 16) * 16;
            int usY = (us / 16) * 16;

            for (int j = 0; j < 4; j++) {
                if (til * 8 + j * 2 >= dataBlocks.length) continue;

                word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);


                bitmask = Integer.toBinaryString(word);
                bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                int priorit = Integer.parseInt(bitmask.substring(0, 1));
                int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                int ind = Integer.parseInt(bitmask.substring(5), 2);

                int x = (j % 2) * 8 + ((i / 2) % 8) * 16;
                int y = (j / 2) * 8 + ((i / 2) / 8) % 8 * 16;

                word &= ~(1 << 16);
                word &= ~(1 << 17);

                if (flipTX == 1) {
                    flipX = 1 - flipX;

                    word |= 1 << 16;
                }
                if (flipTY == 1) {
                    flipY = 1 - flipY;

                    word |= 1 << 17;
                }

                int idX = usX + ((i / 2) % 8) * 2 + (j % 2);
                int idY = usY + (((i / 2) / 8) % 8) * 2 + (j / 2);

                int index = idX + idY * 256;

                words[index] = word;

                //if (word > 0xF && usY == 0)
                //System.out.println(String.format("For (%d, %d): %s (%d) [%d]", usX + ((i / 2) % 8) * 2 + (j % 2), usY + (((i / 2) / 8) % 8) * 2 + (j / 2), Integer.toBinaryString(word), Integer.parseInt(Integer.toBinaryString(word).substring(5), 2), us) );
            }
        }

        try {
            new File(filename).getParentFile().mkdirs();
            DataOutputStream os = new DataOutputStream(new FileOutputStream(filename));
            for (int i = 0; i < words.length; i++) {
                //os.writeByte((words[i] & 0xFF0000) >> 16);

                os.writeByte(((words[i] & 0xFF0000) >> 16) & 0xFF);
                os.writeByte(((words[i] & 0xFF00) >> 8) & 0xFF);
                os.writeByte((words[i] & 0xFF));
            }
            os.close();
        } catch (Exception e) {
        }
    }

    private void GenerateChunkCollisions(String filename) {
        int max = dataTiledata.length;
        //max = 128;

        new File(filename).getParentFile().mkdirs();

        try {
            //new File(filename).mkdirs();
            DataOutputStream os = new DataOutputStream(new FileOutputStream(filename));
            for (int i = 0; i < dataTiledata.length / 128; i += 1) {
                for (int x = 0; x < 8; x++) {
                    for (int y = 0; y < 8; y++) {
                        int word = (dataTiledata[i * 128 + x * 2 + y * 16] & 0xFF) * 0x100 + (dataTiledata[i * 128 + x * 2 + y * 16 + 1] & 0xFF);
                        String bitmask = Integer.toBinaryString(word);
                        bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                            /*int solid1 = Integer.parseInt(bitmask.substring(0, 2), 2);
                            int solid2 = Integer.parseInt(bitmask.substring(2, 4), 2);
                            int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
                            int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);*/
                        int til = Integer.parseInt(bitmask.substring(6), 2);

                        int heightInd = dataCollisionIndex[(dataCollisionDataSize == 2 ? til * 2 + 1 + 0x600 : til * 2 + 1)] & 0xFF;

                        os.writeByte(dataTiledata[i * 128 + x * 2 + y * 16] & 0xFF);
                        os.writeByte(heightInd);
                        heightInd = dataCollisionIndex[(dataCollisionDataSize == 2 ? til * 2 + 1 : til * 2)] & 0xFF;
                        os.writeByte(heightInd);
                    }
                }
            }
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void GenerateSourceFiles(String filename) {
        String sceneCode = "" + levelID + "";
        String sceneName = "Scene_" + levelID + "";
        int LvlNum = 0;
        for (int i = 0; i < lvlIDs.length; i++) {
            if (lvlIDs[i].equals(levelID)) {
                LvlNum = i + 1;
                break;
            }
        }
        String nextScene = "Scene_" + lvlIDs[LvlNum] + "";

        String FILE_source = "#include \"Standard.h\"\n" +
                "#include \"Scene_MainMenu.h\"\n" +
                "#include \"Application.h\"\n" +
                "#include \"Resources.h\"\n" +
                "#include \"Player.h\"\n" +
                "#include \"%%SceneName%%.h\"\n" +
                "#include \"S3Object.h\"\n" +
                "#include \"LevelScene.h\"\n" +
                "\n" +
                "#include \"%%SceneNameNext%%.h\"\n" +
                "\n" +
                "#define Z_MID 0\n" +
                "#define Z_FRONT -1\n" +
                "#define Z_BACK 1\n" +
                "#define T(name) App->Textures[name]\n" +
                "#define S(name) App->Sprites[name]\n" +
                "\n" +
                "// Unique\n" +
                "const char LevelID = %%LevelID%%;\n" +
                "// Modularized\n" +
                "%%SceneName%%::%%SceneName%%(Application* app, int act, int checkpoint) {\n" +
                "    SDL_RWops *_RWList[35] = {\n" +
                "        newFile(res_%%SceneCode%%_level_lvl, \"Resource/Stages/%%SceneCode%%/level.lvl\"),\n" +
                "        // Musics\n" +
                "        newFile(res_%%SceneCode%%_act_1_ogg, \"Resource/Stages/%%SceneCode%%/Music/Act 1.ogg\"),\n" +
                "        newFile(res_%%SceneCode%%_act_2_ogg, \"Resource/Stages/%%SceneCode%%/Music/Act 2.ogg\"),\n" +
                "        // Layouts\n" +
                "        newFile(res_%%SceneCode%%_layout_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Layout)/1.bin\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_layout_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Layout)/1.bin\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_layout_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Layout)/2.bin\"),\n" +
                "        newFile(res_%%SceneCode%%_layout_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Layout)/2.bin\"),\n" +
                "        // Chunks (128x128)\n" +
                "        newFile(res_%%SceneCode%%_chunks_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Chunks)/1.bin\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_chunks_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Chunks)/1.bin\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_chunks_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Chunks)/2.bin\"),\n" +
                "        newFile(res_%%SceneCode%%_chunks_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Chunks)/2.bin\"),\n" +
                "        // Tiles (16x16)\n" +
                "        newFile(res_%%SceneCode%%_tiles_0_bin, \"Resource/Stages/%%SceneCode%%/Data (Tiles)/0.bin\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_tiles_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Tiles)/1.bin\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_tiles_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Tiles)/2.bin\"),\n" +
                "        newFile(res_%%SceneCode%%_tiles_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Tiles)/2.bin\"),\n" +
                "        // Rings\n" +
                "        newFile(res_%%SceneCode%%_rings_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Rings)/1.bin\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_rings_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Rings)/1.bin\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_rings_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Rings)/2.bin\"),\n" +
                "        newFile(res_%%SceneCode%%_rings_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Rings)/2.bin\"),\n" +
                "        // Objects\n" +
                "        newFile(res_%%SceneCode%%_objects_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Objects)/1.bin\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_objects_1_bin, \"Resource/Stages/%%SceneCode%%/Data (Objects)/1.bin\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_objects_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Objects)/2.bin\"),\n" +
                "        newFile(res_%%SceneCode%%_objects_2_bin, \"Resource/Stages/%%SceneCode%%/Data (Objects)/2.bin\"),\n" +
                "        // Palettes (Dry)\n" +
                "        newFile(res_%%SceneCode%%_PAL1_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL1_DRY.png\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_PAL1_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL1_DRY.png\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_PAL2_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL2_DRY.png\"),\n" +
                "        newFile(res_%%SceneCode%%_PAL2_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL2_DRY.png\"),\n" +
                "        // Palettes (Wet)\n" +
                "        newFile(res_%%SceneCode%%_PAL1_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL1_WET.png\"), // Pre-Act\n" +
                "        newFile(res_%%SceneCode%%_PAL1_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL1_WET.png\"), // Act 1\n" +
                "        newFile(res_%%SceneCode%%_PAL2_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL2_WET.png\"),\n" +
                "        newFile(res_%%SceneCode%%_PAL2_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL2_WET.png\"),\n" +
                "        // Palettes (Other)\n" +
                "        newFile(res_%%SceneCode%%_PALm_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL_MB_DRY.png\"), // Mini-boss\n" +
                "        newFile(res_%%SceneCode%%_PALm_DRY_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL_MB_DRY.png\"), // Boss\n" +
                "        newFile(res_%%SceneCode%%_PALm_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL_MB_WET.png\"), // Extra 1 (Mini-boss)\n" +
                "        newFile(res_%%SceneCode%%_PALm_WET_png, \"Resource/Stages/%%SceneCode%%/Palettes/PAL_MB_WET.png\"), // Extra 2\n" +
                "    };\n" +
                "\n" +
                "    App = app;\n" +
                "\n" +
                "    memcpy(RWList, _RWList, sizeof(_RWList));\n" +
                "    Init(app, LevelID + act, checkpoint);\n" +
                "}\n" +
                "void %%SceneName%%::DoSwitchActZone() {\n" +
                "\tif (!doSwitch) return;\n" +
                "\tdoSwitch = false;\n" +
                "\n" +
                "    if (ActualAct == 2) {\n" +
                "        %%SceneNameNext%%* nextLevel = new %%SceneNameNext%%(App, 1, -1);\n" +
                "        nextLevel->score = score;\n" +
                "        nextLevel->lives = lives;\n" +
                "        App->nextScene = nextLevel;\n" +
                "        return;\n" +
                "    }\n" +
                "\n" +
                "    int ObjectPlayerOffsetX = %%ObjectPlayerOffsetX%%;\n" +
                "    int ObjectPlayerOffsetY = %%ObjectPlayerOffsetY%%;\n" +
                "\n" +
                "\tMyPlayer->X += ObjectPlayerOffsetX;\n" +
                "\tMyPlayer->Y += ObjectPlayerOffsetY;\n" +
                "\n" +
                "\tact = LevelID + ActualAct + 1;\n" +
                "\n" +
                "\tInitZone(true, 0, -1, true);\n" +
                "\n" +
                "\tcameraX[App->CurrentViewport] += ObjectPlayerOffsetX;\n" +
                "\tcameraY[App->CurrentViewport] += ObjectPlayerOffsetY;\n" +
                "\tcameraMinX = 0;\n" +
                "\tcameraMinY = 0;\n" +
                "}\n" +
                "// Unique\n" +
                "void %%SceneName%%::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {\n" +
                "    int a = act & 0xF;\n" +
                "\n" +
                "    if (a == 1)\n" +
                "        Tileset = new ITexture(IResources::Load(\"Stages/%%SceneCode%%/Sprites/TILES1.png\"), false);\n" +
                "    else if (a == 2)\n" +
                "        Tileset = new ITexture(IResources::Load(\"Stages/%%SceneCode%%/Sprites/TILES2.png\"), false);\n" +
                "\n" +
                "    // Parallax Sizes and Auto Scrolls\n" +
                "    if (a <= 1) {\n" +
                "        int PS[] =    {  0x0040,   0x0008,   0x0008,   0x0005,   0x0005,   0x0006,   0x00F0,   0x0006,   0x0005,   0x0005,   0x0008,   0x0008,   0x0030,   0x80C0,   0x7FFF };\n" +
                "        float PAS[] = {  0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,     };\n" +
                "        float PM[] =  {  0.250f, 0.21875f,  0.1875f, 0.15625f,   0.125f, 0.09375f,  0.0625f, 0.09375f,   0.125f, 0.15625f,  0.1875f, 0.21875f,   0.250f,   0.250f,   0.125f };\n" +
                "\n" +
                "        // $FFFFA800 - BG deformation RAM address\n" +
                "        // d0 = 1 / 4\n" +
                "        // d1 = 1 / 32\n" +
                "\n" +
                "        // 0x00: 1 / 4\n" +
                "        // 0x02: 1 / 4 - 1 / 32\n" +
                "        // 0x04: 1 / 4 - 1 / 32 - 1 / 32\n" +
                "        // 0x06: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x08: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x0A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x0C: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x0E: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x10: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x12: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // 0x14: 1 / 4 - 1 / 32 - 1 / 32\n" +
                "        // 0x16: 1 / 4 - 1 / 32\n" +
                "        // 0x18: 1 / 4\n" +
                "        //0x19A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32\n" +
                "        // d3 = 1 / 4\n" +
                "\n" +
                "        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));\n" +
                "        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));\n" +
                "        memcpy(ParallaxMult, PM, sizeof(PM));\n" +
                "        ParallaxCount = sizeof(PS) / sizeof(*PS);\n" +
                "    }\n" +
                "    else {\n" +
                "        int PS[] =    { 0x0008, 0x0008, 0x0090, 0x0010, 0x0008, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00B0, 0x0010, 0x0008, 0x7FFF };\n" +
                "        float PAS[] = { 0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,     };\n" +
                "        float PM[] =  { 0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f };\n" +
                "\n" +
                "        memcpy(ParallaxAutoScroll, PAS, sizeof(PAS));\n" +
                "        memcpy(ParallaxSizes, PS, sizeof(PS));\n" +
                "        memcpy(ParallaxMult, PM, sizeof(PM));\n" +
                "        ParallaxCount = sizeof(PS) / sizeof(*PS);\n" +
                "    }\n" +
                "\n" +
                "    // Animated Tiles\n" +
                "    AnimTilesCount = 0;\n" +
                "\n" +
                "    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);\n" +
                "}\n" +
                "void %%SceneName%%::CreateUniqueTextures(Application* app, int act) {\n" +
                "\tunsigned int nextTick = SDL_GetTicks();\n" +
                "\n" +
                "    T(\"%%SceneCode%%\") = new ITexture(IResources::Load(\"Stages/%%SceneCode%%/Sprites/Texture.png\"), false);\n" +
                //"    T(\"AIZ\") = new ITexture(IResources::Load(\"Stages/AIZ/Sprites/Texture.png\"), false);\n" +
                "\n" +
                "%%UniqueTexturesFiller%%\n" +
                "\n" +
                "\tApp->print(0, \"UNIQUE Texture load finished in %.3f seconds.\", (SDL_GetTicks() - nextTick) / 1000.0f);\n" +
                "}\n" +
                "void %%SceneName%%::UnloadUniqueTextures(Application* app, int act) {\n" +
                "\n" +
                "}\n" +
                "\n" +
                "void %%SceneName%%::Update() {\n" +
                "    BGx = 1.f;\n" +
                "    BGy = 1.f / 4.f;\n" +
                "    BGstart = 0;\n" +
                "    BGsize = 4;\n" +
                "\n" +
                "\tLevelScene::Update();\n" +
                "\n" +
                "\t// Do palette stuffs\n" +
                "    if (frameAnim % 8 == 0) {\n" +
                "    \tint len = 4;\n" +
                "        int pick;\n" +
                "\n" +
                "        // Do common palette stuffs (Super Sonic)\n" +
                "        if (MyPlayer->SuperForm || MyPlayer->SuperFormAnim != SuperFormAnimType::None) {\n" +
                "            // Super Sonic\n" +
                "            len = 4;\n" +
                "            pick = 3 * ((frameAnim / 16) % len + 6);\n" +
                "\n" +
                "            if (MyPlayer->SuperFormAnim == SuperFormAnimType::Transforming) {\n" +
                "                pick = ((MyPlayer->SuperFormAnimTimerMax - MyPlayer->SuperFormAnimTimer) / 4);\n" +
                "                if (pick < 2)\n" +
                "                    pick = 2;\n" +
                "                pick = 3 * pick;\n" +
                "            }\n" +
                "            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Deforming) {\n" +
                "                pick = 3 * (MyPlayer->SuperFormAnimTimer / 4);\n" +
                "            }\n" +
                "            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Super) {\n" +
                "                pick = 0;\n" +
                "            }\n" +
                "\n" +
                "            int *PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterHCZCNZLBZ;\n" +
                "            if (LevelID == 0x10 || LevelID == 0x50)\n" +
                "                PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterAIZICZ;\n" +
                "\n" +
                "            setPixel(paletteDry->pixels, 0, 2, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);\n" +
                "            setPixel(paletteWet->pixels, 0, 2, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);\n" +
                "            pick++;\n" +
                "            setPixel(paletteDry->pixels, 0, 3, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);\n" +
                "            setPixel(paletteWet->pixels, 0, 3, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);\n" +
                "            pick++;\n" +
                "            setPixel(paletteDry->pixels, 0, 4, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);\n" +
                "            setPixel(paletteWet->pixels, 0, 4, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);\n" +
                "        }\n" +
                "\n" +
                "        glBindTexture(GL_TEXTURE_2D, paletteDry->tex);\n" +
                "        glTexSubImage2D(GL_TEXTURE_2D, 0,\n" +
                "            0, 0, paletteDry->w, paletteDry->h,\n" +
                "            GL_RGBA, GL_UNSIGNED_BYTE,\n" +
                "            paletteDry->pixels);\n" +
                "        glBindTexture(GL_TEXTURE_2D, 0);\n" +
                "\n" +
                "        glBindTexture(GL_TEXTURE_2D, paletteWet->tex);\n" +
                "        glTexSubImage2D(GL_TEXTURE_2D, 0,\n" +
                "            0, 0, paletteWet->w, paletteWet->h,\n" +
                "            GL_RGBA, GL_UNSIGNED_BYTE,\n" +
                "            paletteWet->pixels);\n" +
                "        glBindTexture(GL_TEXTURE_2D, 0);\n" +
                "    }\n" +
                "}\n" +
                "\n" +
                "int %%SceneName%%::CustomAnimTile(int ind, int i, int parRow) {\n" +
                "\tint tileID = AnimTiles[i * 5 + 0];\n" +
                "\tint tileCount = AnimTiles[i * 5 + 1];\n" +
                "\tint aniID = AnimTiles[i * 5 + 2];\n" +
                "\tint frmCount = AnimTiles[i * 5 + 3];\n" +
                "\tint frmSpd = AnimTiles[i * 5 + 4];\n" +
                "\n" +
                "    if (ActualAct == 1) {\n" +
                "//%%AnimTiles1%%\n" +
                "    }\n" +
                "    else {\n" +
                "//%%AnimTiles2%%\n" +
                "    }\n" +
                "\treturn ind;\n" +
                "}\n" +
                "\n" +
                "// OVERRIDES\n" +
                "void %%SceneName%%::HandleCamera() {\n" +
                "\tLevelScene::HandleCamera();\n" +
                "\n" +
                "\tif (MyPlayer->Action == ActionType::Dead)\n" +
                "\t\treturn;\n" +
                "}\n" +
                "void %%SceneName%%::DrawAboveEverythingNonHUD() {\n" +
                "\tLevelScene::DrawAboveEverythingNonHUD();\n" +
                "    return;\n" +
                "}\n" +
                "void %%SceneName%%::Render() {\n" +
                "\tLevelScene::Render();\n" +
                "}\n" +
                "void %%SceneName%%::Free() {\n" +
                "\tLevelScene::Free();\n" +
                "    UnloadUniqueTextures(app, 1);\n" +
                "    UnloadUniqueTextures(app, 2);\n" +
                "    unloadAnimatedChunks(app);\n" +
                "}\n";

        String FILE_header = "#ifndef SCENE_%%SceneCode%%_H\n" +
                "#define SCENE_%%SceneCode%%_H\n" +
                "\n" +
                "#include \"Standard.h\"\n" +
                "#include \"Application.h\"\n" +
                "#include \"TextureAudio.h\"\n" +
                "#include \"LevelScene.h\"\n" +
                "#include \"Player.h\"\n" +
                "\n" +
                "class %%SceneName%% : public LevelScene {\n" +
                "public:\n" +
                "    %%SceneName%%(Application* app, int act, int checkpoint);\n" +
                "    void DoSwitchActZone();\n" +
                "\n" +
                "    void InitZone(bool resetTextures, int check, int specialRing, bool actTransition);\n" +
                "    void CreateUniqueTextures(Application* app, int act);\n" +
                "    void UnloadUniqueTextures(Application* app, int act);\n" +
                "\n" +
                "    void Update();\n" +
                "\n" +
                "    int CustomAnimTile(int ind, int i, int parRow);\n" +
                "    \n" +
                "    void HandleCamera();\n" +
                "    void DrawAboveEverythingNonHUD();\n" +
                "    void Render();\n" +
                "    void Free();\n" +
                "};\n" +
                "\n" +
                "#endif /* SCENE_%%SceneCode%%_H */\n";

        String FILE_CreateUniqueTexturesFiller = "";
        for (final File fileEntry : new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/Sprites/").listFiles()) {
            if (fileEntry.getName().startsWith("."))
                continue;
            if (fileEntry.getName().contains("png"))
                continue;
            String nameNoExtension = fileEntry.getName().replaceAll(".spr", "");
            FILE_CreateUniqueTexturesFiller += "    S(\"" + nameNoExtension + "\") = new ISprite(IResources::Load(\"Stages/" + levelID + "/Sprites/" + nameNoExtension + ".spr\"), T(\"" + levelID + "\"));\n";
            //FILE_CreateUniqueTexturesFiller += "    S(\"" + nameNoExtension + "\") = new ISprite(IResources::Load(\"Stages/AIZ/Sprites/" + nameNoExtension + ".spr\"), T(\"AIZ\"));\n";
        }



        FILE_source = FILE_source.replaceAll("%%SceneName%%", sceneName);
        FILE_source = FILE_source.replaceAll("%%SceneNameNext%%", nextScene);
        FILE_source = FILE_source.replaceAll("%%SceneCode%%", sceneCode);
        FILE_source = FILE_source.replaceAll("%%LevelID%%", "0x" + Integer.toHexString(LvlNum).toUpperCase() + "0");
        FILE_source = FILE_source.replaceAll("%%UniqueTexturesFiller%%", FILE_CreateUniqueTexturesFiller);
        FILE_source = FILE_source.replaceAll("%%ObjectPlayerOffsetX%%", "0x0000");
        FILE_source = FILE_source.replaceAll("%%ObjectPlayerOffsetY%%", "0x0000");

        FILE_header = FILE_header.replaceAll("%%SceneName%%", sceneName);
        FILE_header = FILE_header.replaceAll("%%SceneNameNext%%", nextScene);
        FILE_header = FILE_header.replaceAll("%%SceneCode%%", sceneCode);


        if (!new File(gameSourcePath + "source/" + sceneName + ".cpp").exists()) {
            System.out.println(">>>>> Level ID: 0x" + Integer.toHexString(LvlNum).toUpperCase() + "0");
            System.out.println(">>>>> File: " + sceneName + ".cpp");
            System.out.println(">>>>>");
            System.out.println(FILE_source);
            try {
                PrintWriter writer = new PrintWriter(new File(gameSourcePath + "source/" + sceneName + ".cpp"), "UTF-8");
                writer.println(FILE_source);
                writer.close();
            } catch (IOException e) {
            }
        }

        if (!new File(gameSourcePath + "source/" + sceneName + ".h").exists()) {
            System.out.println(">>>>>");
            System.out.println(">>>>> File: " + sceneName + ".h");
            System.out.println(">>>>>");
            System.out.println(FILE_header);
            try {
                PrintWriter writer = new PrintWriter(new File(gameSourcePath + "source/" + sceneName + ".h"), "UTF-8");
                writer.println(FILE_header);
                writer.close();
            } catch (IOException e) {
            }
        }


        ////////////////////////////////////////
        ////
        //// LEVEL.lvl
        ////
        ////////////////////////////////////////

        try {

            DataOutputStream dataOutputStream = new DataOutputStream(new FileOutputStream(new File(gameSourcePath + "source/Resource/Stages/" + levelID + "/level.lvl")));
            for (int i = 0; i < 0x40; i++) {
                if (i % 2 == 0)
                    dataOutputStream.writeByte(0x00);
                else
                    dataOutputStream.writeByte(0xFF);
            }

            DataInputStream dataInputStream = new DataInputStream(new FileInputStream(new File(gameSourcePath + "skdisasm-master/Levels/" + levelID.replaceAll("TDZ", "DDZ") + "/Start Location/Sonic/" + 1 + ".bin")));
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataInputStream.close();

            dataInputStream = new DataInputStream(new FileInputStream(new File(gameSourcePath + "skdisasm-master/Levels/" + levelID.replaceAll("TDZ", "DDZ") + "/Start Location/Sonic/" + 2 + ".bin")));
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataInputStream.close();

            dataInputStream = new DataInputStream(new FileInputStream(new File(gameSourcePath + "skdisasm-master/Levels/" + levelID.replaceAll("TDZ", "DDZ") + "/Start Location/Knuckles/" + 1 + ".bin")));
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataInputStream.close();

            dataInputStream = new DataInputStream(new FileInputStream(new File(gameSourcePath + "skdisasm-master/Levels/" + levelID.replaceAll("TDZ", "DDZ") + "/Start Location/Knuckles/" + 2 + ".bin")));
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataInputStream.close();

            dataInputStream = new DataInputStream(new FileInputStream(new File(gameSourcePath + "skdisasm-master/Levels/Misc/StartingWaterHeights.bin")));
            //dataInputStream.skip(lvlIndex * 4);
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(dataInputStream.read());
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataOutputStream.writeByte(0xFF);
            dataInputStream.close();

            for (int i = 0; i < 16; i++) {
                if (i < lvlTitles[LvlNum - 1].length())
                    dataOutputStream.writeByte(lvlTitles[LvlNum - 1].charAt(i));
                else
                    dataOutputStream.writeByte(0x00);
            }

            for (int i = 0; i < 0x30; i++) {
                dataOutputStream.writeByte(0x00);
            }

            dataOutputStream.close();

        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void GenerateChunkImages(String parentFolder, String lvl, int act) {
        paletted = false;

        BufferedImage chunks = new BufferedImage(128 * 16, 128 * 16, BufferedImage.TYPE_INT_ARGB);
        BufferedImage chunks2 = new BufferedImage(128 * 16, 128 * 16, BufferedImage.TYPE_INT_ARGB);
        Graphics2D cg = chunks.createGraphics();
        Graphics2D cg2 = chunks2.createGraphics();

        boolean clearChunk = false;
        int curChunk = 0;

        ArrayList<Boolean> fronts = new ArrayList<>();

        for (int i = 0; i < dataTiledata.length; i += 2) {
            int overX = (((i / 2) / 64) % 16) * 128;
            int overY = (((i / 2) / 64) / 16) * 128;

            if (i % 128 == 0) {
                clearChunk = false;
                curChunk = i / 128;
            }

            int word = (dataTiledata[i] & 0xFF) * 0x100 + (dataTiledata[i + 1] & 0xFF);
            String bitmask = Integer.toBinaryString(word);
            bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
            //int priority = Integer.parseInt(bitmask.substring(0, 1));
            int palO = Integer.parseInt(bitmask.substring(1, 3), 2);
            int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
            int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
            int til = Integer.parseInt(bitmask.substring(6), 2);

            for (int j = 0; j < 4; j++) {
                if (til * 8 + j * 2 >= dataBlocks.length) continue;

                word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);

                bitmask = Integer.toBinaryString(word);
                bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                int priorit = Integer.parseInt(bitmask.substring(0, 1));
                int pal = Integer.parseInt(bitmask.substring(1, 3), 2);
                int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                int ind = Integer.parseInt(bitmask.substring(5), 2);


                //if (priorit == 0) continue;


                int x = (j % 2) * 8 + ((i / 2) % 8) * 16;
                int y = (j / 2) * 8 + ((i / 2) / 8) % 8 * 16;

                if (flipTY == 1) {
                    flipY = 1 - flipY;
                    y = (1 - (j / 2)) * 8 + ((i / 2) / 8) % 8 * 16;
                }
                if (flipTX == 1) {
                    flipX = 1 - flipX;
                    x = (1 - (j % 2)) * 8 + ((i / 2) % 8) * 16;
                }

                int[] theTile = {0, 0};
                if (priorit != 0) {
                    theTile = drawTile(cg, overX + x * 1, overY + y * 1, ind, flipX == 1, flipY == 1, 0);
                } else {
                    theTile = drawTile(cg2, overX + x * 1, overY + y * 1, ind, flipX == 1, flipY == 1, 0);
                }
                //par.drawTile(cg3, overX + x * 1, overY + y * 1, ind, flipX == 1, flipY == 1, pal);
                if (theTile[0] > 0) {
                    if (!clearChunk) {
                        //chunksToAnimate.add(curChunk * 128);
                        //chunksToAnimateLen.add(theTile);
                        //clearChunk = true;
                    }
                }
            }

            if (clearChunk) {
                /*cg.setBackground(new Color(0, 0, 0, 0));
                cg.clearRect(overX, overY, 128, 128);
                cg2.setBackground(new Color(0, 0, 0, 0));
                cg2.clearRect(overX, overY, 128, 128);
                cg3.setBackground(new Color(0, 0, 0, 0));
                cg3.clearRect(overX, overY, 128, 128);*/
            }

            if (i == dataTiledata.length - 2) {
                try {
                    String U = "";
                    if (!paletted)
                        U = "U";
                    ImageIO.write(chunks, "png", new File(parentFolder + lvl + "" + act + "_" + U + "ChunksFront.png"));
                    ImageIO.write(chunks2, "png", new File(parentFolder + lvl + "" + act + "_" + U + "ChunksBack.png"));
                } catch (Exception e) {
                }
            }
        }

        System.out.println();
        System.out.println("FINISHED MAIN TILE CREATION");
        System.out.println();

        chunks.flush();
        chunks2.flush();
        cg.dispose();
        cg2.dispose();

        return;

        /*

        for (int c = 0; c < chunksToAnimate.size(); c++) {
            int cuChunk = chunksToAnimate.get(c);
            int[] len = chunksToAnimateLen.get(c);

            BufferedImage chunk = new BufferedImage(128 * len[0], 128 * 2, BufferedImage.TYPE_INT_ARGB);
            Graphics2D cgg = chunk.createGraphics();

            cgg.setBackground(new java.awt.Color(0, 0, 0, 0));
            cgg.clearRect(0, 0, 128 * len[0], 128 * 2);

            for (int f = 0; f < len[0]; f += 1) {
                for (int i = cuChunk; i < cuChunk + 128; i += 2) {
                    int overX = (f % 16) * 128;
                    int overY = (f / 16) * 128;
                    int memeX = (((i - cuChunk) / 2) % 8) * 16;
                    int memeY = (((i - cuChunk) / 2) / 8) * 16;

                    int word = (dataTiledata[i] & 0xFF) * 0x100 + (dataTiledata[i + 1] & 0xFF);
                    String bitmask = Integer.toBinaryString(word);
                    bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                    //int priority = Integer.parseInt(bitmask.substring(0, 1));
                    int palO = Integer.parseInt(bitmask.substring(1, 3), 2);
                    int flipTY = Integer.parseInt(bitmask.substring(4, 5), 2);
                    int flipTX = Integer.parseInt(bitmask.substring(5, 6), 2);
                    int til = Integer.parseInt(bitmask.substring(6), 2);


                    for (int j = 0; j < 4; j++) {
                        if (til * 8 + j * 2 >= dataBlocks.length) continue;

                        word = (dataBlocks[til * 8 + j * 2] & 0xFF) * 0x100 + (dataBlocks[til * 8 + j * 2 + 1] & 0xFF);

                        bitmask = Integer.toBinaryString(word);
                        bitmask = ("0000000000000000" + bitmask).substring(bitmask.length());
                        int priorit = Integer.parseInt(bitmask.substring(0, 1));
                        int pal = Integer.parseInt(bitmask.substring(1, 3), 2);
                        int flipY = Integer.parseInt(bitmask.substring(3, 4), 2);
                        int flipX = Integer.parseInt(bitmask.substring(4, 5), 2);
                        int ind = Integer.parseInt(bitmask.substring(5), 2);

                        int x = (j % 2) * 8;
                        int y = (j / 2) * 8;

                        if (flipTY == 1) {
                            flipY = 1 - flipY;
                            y = (1 - (j / 2)) * 8;
                        }
                        if (flipTX == 1) {
                            flipX = 1 - flipX;
                            x = (1 - (j % 2)) * 8;
                        }
                        if (len[1] > 0) {
                            par.drawTile(cgg, overX + x + memeX, overY + y + memeY, ind, flipX == 1, flipY == 1, pal, f, true);
                            par.drawTile(cgg, overX + x + memeX, overY + y + memeY + 128, ind, flipX == 1, flipY == 1, pal, f, false);
                        }
                        else {
                            if (priorit != 0) {
                                par.drawTile(cgg, overX + x + memeX, overY + y + memeY, ind, flipX == 1, flipY == 1, pal, f);
                            } else {
                                par.drawTile(cgg, overX + x + memeX, overY + y + memeY + 128, ind, flipX == 1, flipY == 1, pal, f);
                            }
                        }
                    }
                }
            }

            chunksAnimated.add(chunk);
            chunksAnimatedInd.add(cuChunk / 128);
            //System.out.println("Added animation for tile " + Integer.toHexString(cuChunk / 128) + " with length of " + len + " at " + (chunksAnimated.size() - 1));

            if (!new File("C:/Users/Justin/Documents/Sonic 3/Files/Level/" + par.lvl + "/Animated Chunks/").exists()) {
                new File("C:/Users/Justin/Documents/Sonic 3/Files/Level/" + par.lvl + "/Animated Chunks/").mkdirs();
            }

            try {
                ImageIO.write(chunk, "png", new File("C:/Users/Justin/Documents/Sonic 3/Files/Level/" + par.lvl + "/Animated Chunks/" + Integer.toHexString(cuChunk / 128).toUpperCase() + "x" + chunksToAnimateLen.get(c)[0] + "01.png"));
            } catch (Exception e) { }
        }*/
    }

    public int[] drawTile(Graphics gg, int xx, int yy, int i, boolean flipx, boolean flipy, int pal) {
        return drawTile(gg, xx, yy, i, flipx, flipy, pal, 0);
    }

    public int[] drawTile(Graphics gg, int xx, int yy, int i, boolean flipx, boolean flipy, int pal, int frame) {
        return drawTile(gg, xx, yy, i, flipx, flipy, pal, frame, false);
    }

    public int[] drawTile(Graphics gg, int xx, int yy, int i, boolean flipx, boolean flipy, int pal, int frame, boolean o) {
        if (gg == null) return new int[]{0, 0};

        int add = 0;
        int overrider = 0;

        int pxl = 0;
        int y = -1;

        if (paletted)
            pal = (pal + 2) % 4;

        for (int j = 0; j < 0x20; j++) {
            if (i * 0x20 + j > dataTilePixels.length - 1) {
                continue;
            }
            int pxlz = dataTilePixels[i * 0x20 + j] & 0xFF;
            int pm = (pxlz & 0xF0) / 0x10;
            int pm2 = (pxlz & 0x0F);

            if (!o && overrider == 1) {
                pxlz = 0;
            }
            if (o && overrider != 1) {
                pxlz = 0;
            }

            int pxl0 = pxlz >> 4;
            int pxl1 = pxlz & 0xF;
            if (pxl % 8 == 0) y++;

            int eeex = (pxl % 8);
            if (flipx) {
                eeex = 7 - eeex;
            }
            int eeey = y;
            if (flipy) {
                eeey = 7 - eeey;
            }

            int color = pal * 32 + pxl0 * 2;
            color %= bPalette.length;

            gg.setColor(new java.awt.Color((bPalette[color + 1] & 0xF) * 16 + (bPalette[color + 1] & 0xF),
                    (bPalette[color + 1] & 0xF0),
                    (bPalette[color] & 0xFF) * 16 + (bPalette[color] & 0xFF), color % 32 == 0 ? 0 : 255));

            if (!paletted)
                gg.setColor(new java.awt.Color(
                        (pal == 0 ? pxl0 * 16 : 0),
                        (pal == 1 ? pxl0 * 16 : 0),
                        (pal == 2 ? pxl0 * 16 : 0),
                        (pal == 3 ? pxl0 * 16 + 15 : 255) * (color % 32 == 0 ? 0 : 1)));
            gg.fillRect(eeex + xx, eeey + yy, 1, 1);
            pxl++;

            // pal is the "x"
            // pxl0 is the "y"

            eeex = (pxl % 8);
            if (flipx) {
                eeex = 7 - eeex;
            }

            color = pal * 32 + pxl1 * 2;
            color %= bPalette.length;
            gg.setColor(new java.awt.Color((bPalette[color + 1] & 0x0F) * 16 + (bPalette[color + 1] & 0x0F),
                    (bPalette[color + 1] & 0xF0),
                    (bPalette[color] & 0xFF) * 16 + (bPalette[color] & 0xFF), color % 32 == 0 ? 0 : 255));
            if (!paletted)
                gg.setColor(new java.awt.Color(
                        (pal == 0 ? pxl1 * 16 : 0),
                        (pal == 1 ? pxl1 * 16 : 0),
                        (pal == 2 ? pxl1 * 16 : 0),
                        (pal == 3 ? pxl1 * 16 + 15 : 255) * (color % 32 == 0 ? 0 : 1)));
            gg.fillRect(eeex + xx, eeey + yy, 1, 1);
            pxl++;
        }

        return new int[]{add, overrider};
    }

    public enum EditMode {
        OBJECTS("Objects"),
        CHUNKS("Chunk"),
        TILES("Tiles"),
        CAMERA("Camera Bounds"),
        WATER("Water Blocks"),
        BACKGROUND("Background");

        public final String title;

        EditMode(String title) {
            this.title = title;
        }
    }

    public class AnimatedTiles {
        public int ID = 0;
        public int Count = 0;
        public int AnimTileID = 0;
        public int Frames = 0;
        public int FrameLength = 0;

        public AnimatedTiles(int id, int count, int animTileID, int frames, int frameLength) {
            ID = id;
            Count = count;
            AnimTileID = animTileID;
            Frames = frames;
            FrameLength = frameLength;
        }
    }

    public class Obj {
        int X = -1;
        boolean Priority = false;
        boolean DoVerticalFlip = false;
        boolean DoHorizontalFlip = false;
        int Y = -1;
        int ID = 0;
        int SubType = 0x00;
    }

    public class Instruction {
        String type;
        String subtype;
        String[] arguments;
    }

    public class Routine {
        String label = "";
        ArrayList<Instruction> instructions = new ArrayList<>();
    }
}
