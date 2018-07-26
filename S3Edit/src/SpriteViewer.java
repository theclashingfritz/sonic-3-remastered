import SpritePacker.IRectangle2D;
import javafx.application.Application;
import javafx.embed.swing.SwingFXUtils;
import javafx.geometry.Insets;
import javafx.geometry.VPos;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.scene.paint.*;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.text.TextAlignment;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.ArrayList;

/**
 * Created by justin on 4/8/18.
 */
public class SpriteViewer extends Application {

    Canvas canvas;
    Scene scene;

    Image texture;
    ArrayList<IRectangle2D> sprites = new ArrayList<>();
    int frame = 0;

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
        } catch (Exception e) {
        }
        return new int[]{num, numH};
    }

    public static void main(String[] args) {

        launch(args);
    }

    public String FNFix(String f) {
        return (System.getProperty("os.name").toLowerCase().indexOf("win") >= 0 ? "C:" : "") + f;
    }

    public ArrayList<IRectangle2D> LoadSprite(String file) {
        return LoadSprite(new File(file));
    }
    public ArrayList<IRectangle2D> LoadSprite(File file) {
        ArrayList<IRectangle2D> rects = new ArrayList<>();

        try {
            DataInputStream is = new DataInputStream(new FileInputStream(file));
            int g = 0;
            while (is.available() > 0) {
                int X = 0;
                int Y = 0;
                int W = 0;
                int H = 0;
                int OX = 0;
                int OY = 0;
                int OW = 0;
                int OH = 0;

                X += is.readUnsignedByte() * 0x100;
                X += is.readUnsignedByte();

                Y += is.readUnsignedByte() * 0x100;
                Y += is.readUnsignedByte();

                W += is.readUnsignedByte() << 8;
                W += is.readUnsignedByte();

                H += is.readUnsignedByte() << 8;
                H += is.readUnsignedByte();

                OX += is.readUnsignedByte();
                OY += is.readUnsignedByte();

                OW += is.readUnsignedByte() << 8;
                OW += is.readUnsignedByte();

                OH += is.readUnsignedByte() << 8;
                OH += is.readUnsignedByte();

                IRectangle2D rect = new IRectangle2D(X, Y, W, H, OX, OY, OW, OH);
                System.out.print(g + ": " + rect.toString() + " \n");
                rects.add(rect);
                g++;
            }
            is.close();
        }
        catch (Exception e) {
            System.out.println(e.getMessage());
        }
        return rects;
    }

    @Override
    public void start(Stage stage) throws Exception {
        if (true) {
            FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("PNG files (.png)", "*.png");
            FileChooser.ExtensionFilter extFilter2 = new FileChooser.ExtensionFilter("SPR files (.spr)", "*.spr");

            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Open Texture...");
            fileChooser.setInitialDirectory(new File("/Users/justin/Dropbox/sonic3/source/Resource/"));
            fileChooser.getExtensionFilters().add(extFilter);
            //fileChooser.getExtensionFilters().add(extFilter2);
            fileChooser.setSelectedExtensionFilter(extFilter);

            File file = fileChooser.showOpenDialog(stage);
            System.out.println(file.getAbsolutePath().replaceAll("\\\\", "/").replaceAll("C:", ""));
            texture = new Image("file://" + file.getAbsolutePath().replaceAll("\\\\", "/").replaceAll("C:", ""));

            fileChooser = new FileChooser();
            fileChooser.setTitle("Open Sprite...");
            fileChooser.setInitialDirectory(new File("/Users/justin/Dropbox/sonic3/source/Resource/"));
            fileChooser.getExtensionFilters().add(extFilter2);
            file = fileChooser.showOpenDialog(stage);
            sprites = LoadSprite(file.getAbsolutePath());
        }
        else {
            texture = new Image("file://" + ("/Users/justin/Dropbox/sonic3/source/Resource/Sprites/AIZ/Texture.png"));
            sprites = LoadSprite(FNFix("/Users/justin/Dropbox/sonic3/source/Resource/Sprites/AIZ/Miniboss Flame.spr"));
        }

        VBox vertAlign = new VBox();

        ComboBox<String> comboBox = new ComboBox<>();
        comboBox.getItems().add("ImpostorEngine Sprites");
        comboBox.getSelectionModel().select(0);


        GridPane root = new GridPane();
        scene = new Scene(root, 640, 640);

        Label statusBar = new Label("Frame Index: 0");
        statusBar.setPadding(new Insets(8));

        Pane canvasWrapper = new Pane();
        canvas = new Canvas();
        canvasWrapper.getChildren().add(canvas);
        //GridPane.setHgrow(canvasWrapper, Priority.ALWAYS);
        VBox.setVgrow(canvasWrapper, Priority.ALWAYS);
        //GridPane.setFillWidth(canvasWrapper, true);
        //GridPane.setFillHeight(canvasWrapper, true);
        canvas.widthProperty().bind(canvasWrapper.widthProperty());
        canvas.heightProperty().bind(canvasWrapper.heightProperty());
        canvas.widthProperty().addListener(event -> refreshCanvas());
        canvas.heightProperty().addListener(event -> refreshCanvas());
        canvas.setOnKeyPressed(event -> {
            if (event.getCode() == KeyCode.LEFT)
                frame--;
            if (event.getCode() == KeyCode.RIGHT)
                frame++;
            if (frame < 0)
                frame = 0;
            if (frame > sprites.size() - 1)
                frame = sprites.size() - 1;
            statusBar.setText("Frame Index: " + frame);
            refreshCanvas();
        });
        canvas.setFocusTraversable(true);


        vertAlign.getChildren().add(comboBox);
        vertAlign.getChildren().add(canvasWrapper);
        vertAlign.getChildren().add(statusBar);

        root.add(vertAlign, 0, 0, 1, 1);
        GridPane.setHgrow(vertAlign, Priority.ALWAYS);
        GridPane.setVgrow(vertAlign, Priority.ALWAYS);


        scene.setFill(Color.OLDLACE);
        stage.setTitle("Sprite Viewer");
        stage.setScene(scene);
        stage.show();
    }

    private void refreshCanvas() {
        GraphicsContext gc = canvas.getGraphicsContext2D();

        gc.setFill(Color.BLACK);
        gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());

        boolean b = false;
        for (int x = 0; x < canvas.getWidth(); x += 8) {
            for (int y = 0; y < canvas.getHeight(); y += 8) {
                if ((x + y) % 16 == 0)
                    gc.setFill(Paint.valueOf("#333333"));
                else
                    gc.setFill(Paint.valueOf("#111111"));

                gc.fillRect(x, y, 8, 8);

                b = !b;
            }
        }

        gc.save();
        gc.translate(canvas.getWidth() / 2.0, canvas.getHeight() / 2.0);
        gc.scale(4, 4);


        IRectangle2D rect = sprites.get(frame);
        //IRectangle2D src_rect = src_rects.get(g);

        double SprX = rect.OffX - (rect.OffW / 2 - rect.W / 2);
        double SprY = rect.OffY - (rect.OffH / 2 - rect.H / 2);

        gc.drawImage(texture, rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight(), -rect.W / 2 + SprX, -rect.H / 2 + SprY, rect.W, rect.H);

        gc.restore();
    }
}
