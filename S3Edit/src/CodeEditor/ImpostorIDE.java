package CodeEditor;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.layout.*;
import javafx.scene.paint.*;
import javafx.stage.Stage;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ImpostorIDE extends Application {
    public String fileName;
    public ArrayList<TextEditor> editors;
    TabPane tabPane = new TabPane();

    Button rungame = new Button("Run Game");
    Button runscene = new Button("Run Scene");
    Button build = new Button("Build");
    TextEditor textEditorErrors = null;

    ExecutorService service = Executors.newFixedThreadPool(2);
    private String gameSourcePath = "/Users/justin/Dropbox/sonic3/";

    public static void main(String[] args) {
        launch(args);
    }

    public void createToolBarButtons(GridPane toolbar) {
        Canvas graphic_rungame = new Canvas(40, 40);
        Canvas graphic_runscene = new Canvas(40, 40);
        Canvas graphic_build = new Canvas(40, 40);
        Canvas graphic_package = new Canvas(40, 40);




        double w = 25;
        GraphicsContext ctx = graphic_rungame.getGraphicsContext2D();
        ctx.translate(20, 20);
        ctx.setFill(Color.valueOf("#F2D141"));
        ctx.moveTo(w / 2, 0);
        ctx.lineTo(-w / 2, -w / 2 * 1.1547);
        ctx.lineTo(-w / 2, +w / 2 * 1.1547);
        ctx.fill();

        ctx = graphic_runscene.getGraphicsContext2D();
        ctx.save();
            ctx.translate(5, 12.5);
            ctx.rotate(-10);
            ctx.setFill(Color.valueOf("#666"));
            ctx.fillRect(0, -5, 30, 5);

            ctx.setFill(Color.valueOf("#FFF"));
            for (int n = 0; n < 3; n++) {
                ctx.beginPath();
                ctx.moveTo(n * 10 + 0, -5);
                ctx.lineTo(n * 10 + 5, -5);
                ctx.lineTo(n * 10 + 10, 0);
                ctx.lineTo(n * 10 + 5, 0);
                ctx.fill();
            }
        ctx.restore();

        ctx.save();
        ctx.translate(5, 12.5);
        ctx.scale(1, -1);
        ctx.setFill(Color.valueOf("#666"));
        ctx.fillRect(0, -5, 30, 5);

        ctx.setFill(Color.valueOf("#FFF"));
        for (int n = 0; n < 3; n++) {
            ctx.beginPath();
            ctx.moveTo(n * 10 + 0, -5);
            ctx.lineTo(n * 10 + 5, -5);
            ctx.lineTo(n * 10 + 10, 0);
            ctx.lineTo(n * 10 + 5, 0);
            ctx.fill();
        }
        ctx.restore();

        w = 10;
        ctx.translate(20, 28);
        ctx.setFill(Color.valueOf("#F2D141"));
        ctx.fillRect(-15, -10.5, 30, 21);

        ctx.setFill(Color.valueOf("#1C1F24")); //28 31 36
        ctx.beginPath();
        ctx.moveTo(w / 2, 0);
        ctx.lineTo(-w / 2, -w / 2 * 1.1547);
        ctx.lineTo(-w / 2, +w / 2 * 1.1547);
        ctx.fill();

        ctx = graphic_build.getGraphicsContext2D();
        ctx.setFill(Color.valueOf("#4181F1"));
        ctx.moveTo(5, 20);
        ctx.lineTo(20, 5);
        ctx.lineTo(27.5, 12.5);
        ctx.lineTo(22.5, 17.5);
        ctx.lineTo(35, 30);
        ctx.lineTo(30, 35);
        ctx.lineTo(17.5, 22.5);
        ctx.lineTo(12.5, 27.5);
        ctx.fill();

        ctx = graphic_package.getGraphicsContext2D();
        ctx.setFill(Color.valueOf("#4181F1"));
        ctx.moveTo(5, 20);
        ctx.lineTo(20, 5);
        ctx.lineTo(27.5, 12.5);
        ctx.lineTo(22.5, 17.5);
        ctx.lineTo(35, 30);
        ctx.lineTo(30, 35);
        ctx.lineTo(17.5, 22.5);
        ctx.lineTo(12.5, 27.5);
        ctx.fill();

        rungame.setId("clearbutton");
        rungame.setGraphic(graphic_rungame);
        rungame.setContentDisplay(ContentDisplay.TOP);
        rungame.setPrefSize(85, 80);
        rungame.setOnMouseReleased(e -> {
            command_RunGame("");
        });
        toolbar.add(rungame, 0, 0, 1, 1);

        runscene.setId("clearbutton");
        runscene.setGraphic(graphic_runscene);
        runscene.setContentDisplay(ContentDisplay.TOP);
        runscene.setDisable(true);
        runscene.setPrefSize(85, 80);
        runscene.setOnMouseReleased(e -> {
            command_RunScene(e.isShiftDown() ? "2" : e.isControlDown() ? "0" : "1");
        });
        toolbar.add(runscene, 1, 0, 1, 1);

        build.setId("clearbutton");
        build.setGraphic(graphic_build);
        build.setContentDisplay(ContentDisplay.TOP);
        build.setPrefSize(85, 80);
        build.setOnMouseReleased(e -> {
            command_BuildGame("");
        });
        toolbar.add(build, 2, 0, 1, 1);
    }

    public void tabOpenFile(String fileName, int lineNumber) {
        for (int n = 0; n < editors.size(); n++) {
            if (editors.get(n).fileName.equals(fileName)) {
                if (lineNumber >= 0)
                    editors.get(n).goToLine(lineNumber);
                tabPane.getSelectionModel().select(editors.get(n));
                return;
            }
        }

        TextEditor textEditor = new TextEditor();
        textEditor.openFile(fileName);
        textEditor.setOnCloseRequest(e -> editors.remove(textEditor));
        if (lineNumber < 0)
            textEditor.goToLine(0);
        else
            textEditor.goToLine(lineNumber);
        tabPane.getTabs().add(textEditor);
        tabPane.getSelectionModel().select(textEditor);
        editors.add(textEditor);
    }
    public void checkForScene(TextEditor newvalue) {
        runscene.setDisable(true);
        if (newvalue != null) {
            if (newvalue.fileName != null) {
                if (!(newvalue.fileName).isEmpty()) {
                    if (new File(newvalue.fileName).getName().startsWith("Scene_")) {
                        runscene.setDisable(false);
                    }
                }
            }
        }
    }

    public void command_RunGame(String arguments) {
        System.out.println("");
        System.out.println("==========================================================================");
        System.out.println("Attempting to build " + gameSourcePath + "Sonic.exe...");
        System.out.println("==========================================================================");
        service.submit(new Runnable() {
            boolean error = false;

            public void run() {
                if (!error) {
                    try {
                        String line;
                        Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "MAKEdebugger.bat \"" + arguments + "\"", null, new File(gameSourcePath));
                        BufferedReader input =
                                new BufferedReader
                                        (new InputStreamReader(p.getInputStream()));
                        while ((line = input.readLine()) != null) {
                            System.out.println(line);
                        }
                        input.close();
                        p.destroy();
                    }
                    catch (Exception err) {
                        err.printStackTrace();
                    }
                }
                else {
                    try {
                        String line;
                        Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "Sonic.exe \"" + arguments + "\"", null, new File(gameSourcePath));
                        BufferedReader input =
                                new BufferedReader
                                        (new InputStreamReader(p.getInputStream()));
                        while ((line = input.readLine()) != null) {
                            System.out.println(line);
                        }
                        input.close();
                        p.destroy();
                    }
                    catch (Exception err) {
                        err.printStackTrace();
                    }
                }
            }
        });
    }
    public void command_RunScene(String arguments) {
        File folder = new File("../sonic3/source/");
        File[] listOfFiles = folder.listFiles();
        TextEditor textEditor = (TextEditor)tabPane.getSelectionModel().getSelectedItem();
        if (textEditor == null) return;

        if (arguments.isEmpty()) arguments = "1";

        int n = 0;
        int sceneToPick = -1;
        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                String name = listOfFiles[i].getName();
                if (name.startsWith("Scene_")) {
                    System.out.println((n == 0 ? "if " : "else if ") + "(DEBUG_START_SCENE == " + n + ")");
                    System.out.println("    currentScene = new " + name.substring(0, name.length() - 4) + "(this, DEBUG_START_DATA1, DEBUG_START_DATA2);");
                    if (new File(textEditor.fileName).getName().equalsIgnoreCase(name)) {
                        sceneToPick = n;
                    }
                    n++;
                }
            }
        }
        if (sceneToPick != -1)
            command_RunGame("STARTSCENE " + sceneToPick + " " + arguments + " 0");
    }
    public void command_BuildGame(String arguments) {
        System.out.println("");
        System.out.println("==========================================================================");
        System.out.println("Attempting to build " + gameSourcePath + "Sonic.exe...");
        System.out.println("==========================================================================");

        for (int i = 0; i < editors.size(); i++) {
            editors.get(i).clearErrorLines();
        }

        textEditorErrors.codeArea.replaceText(0, textEditorErrors.codeArea.getText().length(), "");

        build.setDisable(true);
        service.submit(new Runnable() {
            boolean error = false;
            String erlog = "";



            public void run() {
                try {
                    String line;
                    Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "MAKE.bat", null, new File(gameSourcePath));
                    BufferedReader input =
                            new BufferedReader
                                    (new InputStreamReader(p.getErrorStream()));

                    ArrayList<String> errors = new ArrayList<String>();

                    while ((line = input.readLine()) != null) {
                        if (line.contains(" error: ")) {
                            error = true;

                            String[] errorSplit = line.split(":");
                            String file = errorSplit[0].split("/")[1];
                            int lineNo = Integer.parseInt(errorSplit[1]) - 1;
                            String reason = errorSplit[4];

                            Platform.runLater(new Runnable() {
                                @Override
                                public void run() {
                                    for (int i = 0; i < editors.size(); i++) {
                                        if (editors.get(i).fileName.contains(file)) {
                                            editors.get(i).setErrorLine(lineNo, reason);
                                            System.out.println("(" + lineNo + ") - " + reason + "   " + editors.get(i).fileName);
                                        }
                                    }
                                }
                            });
                        }
                        System.out.println(line);
                        erlog += line + "\n";

                        Platform.runLater(new Runnable() {
                            @Override
                            public void run() {
                                textEditorErrors.codeArea.replaceText(0, textEditorErrors.codeArea.getText().length(), erlog);
                            }
                        });
                        /*Platform.runLater(new Runnable() {
                            @Override
                            public void run() {
                                textEditorErrors.codeArea.replaceText(0, textEditorErrors.codeArea.getText().length(), erlog);

                                for (int i = 0; i < editors.size(); i++) {
                                    editors.get(i).clearErrorLines();
                                }
                            }
                        });*/
                    }
                    input.close();
                    p.destroy();
                }
                catch (Exception err) {
                    err.printStackTrace();
                }

                System.out.println("Done!");
                erlog += "Done!" + "\n";
                Platform.runLater(new Runnable() {
                    @Override
                    public void run() {
                        textEditorErrors.codeArea.replaceText(0, textEditorErrors.codeArea.getText().length(), erlog);
                    }
                });

                /*if (!error) {
                    try {
                        String line;
                        Process p = Runtime.getRuntime().exec("cmd /c " + gameSourcePath + "Sonic.exe \"" + arguments + "\"", null, new File(gameSourcePath));
                        BufferedReader input =
                                new BufferedReader
                                        (new InputStreamReader(p.getInputStream()));
                        while ((line = input.readLine()) != null) {
                            System.out.println(line);
                        }
                        input.close();
                        p.destroy();
                    }
                    catch (Exception err) {
                        err.printStackTrace();
                    }
                }*/

                Platform.runLater(new Runnable() {
                    @Override
                    public void run() {
                        build.setDisable(false);
                    }
                });
            }
        });
    }

    @Override
    public void start(Stage primaryStage) {
        if (System.getProperty("os.name").toLowerCase().indexOf("win") >= 0) {
            gameSourcePath = "C:" + gameSourcePath;
        }

        System.setProperty("prism.lcdtext", "false");
        System.setProperty("prism.text", "t2k");

        GridPane root = new GridPane();

        //
        // !!!!!!!!!!!!!!!! CODE FOLDING !!!!!!!!!!!
        //

        editors = new ArrayList<>();

        textEditorErrors = new TextEditor();
        textEditorErrors.openFile("");
        textEditorErrors.setClosable(false);
        tabPane.getTabs().add(textEditorErrors);
        tabPane.getSelectionModel().select(textEditorErrors);
        textEditorErrors.setText("Console Log");
        editors.add(textEditorErrors);

        GridPane.setHgrow(tabPane, Priority.ALWAYS);
        GridPane.setVgrow(tabPane, Priority.ALWAYS);
        tabPane.getSelectionModel().selectedItemProperty().addListener((ov, oldvalue, newvalue) -> {
            checkForScene((TextEditor)newvalue);
        });
        root.add(tabPane, 1, 1, 1, 1);

        GridPane toolbar = new GridPane();
        toolbar.setId("toolbar");
        createToolBarButtons(toolbar);
        root.add(toolbar, 0, 0, 1, 1);

        TreeItem<String> rootDummy = new TreeItem<>("Dummy");
        rootDummy.setExpanded(true);

        TreeItem<String> rootItem = new TreeItem<>("Source Code");
        TreeItem<String> rootScenes = new TreeItem<>("Scenes");
        rootItem.setExpanded(true);
        rootScenes.setExpanded(true);
        rootDummy.getChildren().addAll(rootItem, rootScenes);

        File folder = new File("../sonic3/source/");
        File[] listOfFiles = folder.listFiles();

        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                if (listOfFiles[i].getName().startsWith("Scene"))
                    rootScenes.getChildren().add(new TreeItem<>(listOfFiles[i].getName()));
                else
                    rootItem.getChildren().add(new TreeItem<>(listOfFiles[i].getName()));
            }
        }

        TreeView<String> months = new TreeView<>(rootDummy);
        months.setPrefSize(240, 120);
        months.getSelectionModel().setSelectionMode(SelectionMode.SINGLE);
        months.getSelectionModel().selectedItemProperty().addListener((ov, oldvalue, newvalue) -> {
            System.out.println(newvalue);
            if (newvalue.isLeaf())
                tabOpenFile("../sonic3/source/" + newvalue.getValue(), -1);
        });
        months.setEditable(false);
        months.setFocusTraversable(false);
        months.setShowRoot(false);
        root.add(months, 0, 1, 1, 1);


        Scene scene = new Scene(root, 1280, 720);
        //scene.getStylesheets().add("modena_dark.css");
        scene.getStylesheets().add("java-keywords.css");
        primaryStage.setScene(scene);
        primaryStage.setTitle("ImpostorIDE - Test.java");
        primaryStage.show();
    }
}
