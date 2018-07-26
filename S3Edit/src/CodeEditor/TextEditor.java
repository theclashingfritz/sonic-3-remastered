package CodeEditor;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.Duration;
import java.time.format.TextStyle;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.function.Function;
import java.util.function.IntFunction;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javafx.application.Application;
import javafx.beans.InvalidationListener;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Insets;
import javafx.geometry.Point2D;
import javafx.geometry.Pos;
import javafx.scene.CacheHint;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.IndexRange;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Tab;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;
import javafx.stage.Stage;

import org.fxmisc.flowless.VirtualizedScrollPane;
import org.fxmisc.richtext.LineNumberFactory;
import org.fxmisc.richtext.MouseOverTextEvent;
import org.fxmisc.richtext.StyleClassedTextArea;
import org.fxmisc.richtext.model.*;
import org.reactfx.util.Either;

import static org.fxmisc.richtext.model.TwoDimensional.Bias.Backward;
import static org.fxmisc.richtext.model.TwoDimensional.Bias.Forward;

public class TextEditor extends Tab {

    /*private static final String[] KEYWORDS = new String[] {
            "abstract", "assert", "boolean", "break", "byte",
            "case", "catch", "char", "class", "const",
            "continue", "default", "do", "double", "else",
            "enum", "extends", "final", "finally", "float",
            "for", "goto", "if", "implements", "import",
            "instanceof", "int", "interface", "long", "native",
            "new", "package", "private", "protected", "public",
            "return", "short", "static", "strictfp", "super",
            "switch", "synchronized", "this", "throw", "throws",
            "transient", "try", "void", "volatile", "while"
    };*/
    private static final String[] KEYWORDS = new String[] {
            "%",
            "bool",
            "char",
            "double",
            "float",
            "short",
            "int",
            "long",
            "unsigned",
            "const",
            "void",
            "virtual",
            "inline",
            "extern",
            "typedef",
            "struct",
            "uint",



            "abstract", "assert", "break", "byte",
            "case", "catch", "class", "const",
            "continue", "default", "do", "else",
            "enum", "extends", "final", "finally",
            "for", "goto", "if", "implements", "import",
            "instanceof", "interface", "native",
            "new", "package", "private", "protected", "public",
            "return", "static", "strictfp", "super",
            "switch", "synchronized", "this", "throw", "throws",
            "transient", "try", "volatile", "while",
    };

    private static final String[] KEYWORDS2 = new String[] {
            "%", "ifndef", "ifdef", "if", "elif", "else", "include", "define", "endif",
    };

    private static final String[] CLASSES = new String[] {
            "Audio",
            "Texture",
            "Player",
            "S3Object",
            "Scene",
            "Scene_AIZ",
            "Scene_HCZ",
            "Scene_MGZ",

            "SDL_RWops",

            "GLfloat",
            "GLuint",
            "GLint",
            "GLenum",
            "Platform",
            "Input",
            "OPT",
            "Application",
    };

    private static final String KEYWORD_PATTERN = "(\\b(" + String.join("|", KEYWORDS) + ")\\b)|((" + String.join("|#", KEYWORDS2) + "))";
    private static final String CLASSES_PATTERN = "\\b(" + String.join("|", CLASSES) + ")\\b";
    private static final String PAREN_PATTERN = "\\(|\\)";
    private static final String BRACE_PATTERN = "\\{|\\}";
    private static final String BRACKET_PATTERN = "\\[|\\]";
    private static final String SEMICOLON_PATTERN = ";";
    //private static final String STRING_PATTERN = "(\"([^\"\\\\]|\\\\.)*\")|(<([A-z0-9._/])*>)|(\'([^\'\\\\]|\\\\.)*\')";
    private static final String STRING_PATTERN = "((?s)\".*?\")|((?<=#include)\\s+<.*?>)";
    private static final String COMMENT_PATTERN = "//[^\n]*" + "|" + "(?s)/\\*.*?\\*/";
    private static final String METHOD_PATTERN = "([A-Za-z0-9$:_])+(\\s)*(?=\\()";
    private static final String CONSTANT_PATTERN = "\\b(NULL|0x([0-9A-Fa-f\\.]+)|([0-9][0-9\\.f]*)|([A-Z0-9_]+))\\b";

    private static final Pattern PATTERN = Pattern.compile(
              "(?<KEYWORD>" + KEYWORD_PATTERN + ")"
            + "|(?<PAREN>" + PAREN_PATTERN + ")"
            + "|(?<BRACE>" + BRACE_PATTERN + ")"
            + "|(?<BRACKET>" + BRACKET_PATTERN + ")"
            + "|(?<SEMICOLON>" + SEMICOLON_PATTERN + ")"
            + "|(?<METHOD>" + METHOD_PATTERN + ")"
            + "|(?<CLASSES>" + CLASSES_PATTERN + ")"
            + "|(?<STRING>" + STRING_PATTERN + ")"
            + "|(?<COMMENT>" + COMMENT_PATTERN + ")"
            + "|(?<CONSTANT>" + CONSTANT_PATTERN + ")"
    );

    public String language = "C";
    public String fileName;
    public boolean unsaved = false;

    private static final String sampleCode = String.join("\n", new String[] {
            "Hello!",
    });

    private static final String[] functions = new String[] {
        "uint strlen(char* string)",
        "void drawSpriteOPTeasy(Texture* texture, float x, float y, int frameX, int frameY, int alignX, int alignY, float scaleX, float scaleY, int optID)",
        "void renderSpriteOPT(Texture* texture, int optID)",

        "void drawSprite(Texture* texture, float x, float y, float zForFlips, int frame, float xa, float ya)",
        "void drawSpriteH(Texture* texture, float x, float y, int frameX, int frameY, float alignX, float alignY)",
        "void drawSpriteR(Texture* texture, float x, float y, int frameX, int frameY, float alignX, float alignY, float scaleX, float scaleY, float rotationInDegrees)",
        "void drawSpriteT(Texture* texture, GLint TBO, float x, float y, float alignX, float alignY, float width, float height, float rotationInDegrees)",

        "void drawRectangle(float x, float y, float width, float height, float red, float green, float blue, float alpha)",

        "bool AudioStop(Audio* audio)",
        "bool AudioFree(Audio* audio)",
        "bool AudioPause(Audio* audio)",
        "bool AudioResume(Audio* audio)",
        "bool AudioIsPlaying(Audio* audio)",
        "bool AudioPlay(Audio* audio, int loops)",
        "bool AudioSetVolume(Audio* audio, float percent)",

        "int SDL_RWclose(SDL_RWops* context)",
        "int SDL_RWtell(struct SDL_RWops* context)",
        "int SDL_RWseek(SDL_RWops* context, int offset, int whence)",
        "uint SDL_RWread(SDL_RWops* context, void* ptr, uint size, uint maxnum)",
        "uint SDL_RWwrite(SDL_RWops* context, void* ptr, uint size, uint num)",
        "SDL_RWops* SDL_RWFromMem(void* mem, uint size)",
        "SDL_RWops* SDL_RWFromFile(const char* file, const char* mode)",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
        "########################",
    };

    GridPane root;
    public StyleClassedTextArea codeArea;
    TextFlow guiStatusCurrentFunction;

    ArrayList<Integer> errorList = new ArrayList<Integer>();
    ArrayList<String> errorListType = new ArrayList<String>();

    private void createCodeArea(GridPane root, String sampleCode) {
        codeArea = new StyleClassedTextArea();
        codeArea.setId("codeArea");
        IntFunction<Node> numberFactory = LineNumberFactory.get(codeArea); //codeArea.currentParagraphProperty()
        ArrowFactory arrowFactory = new ArrowFactory(codeArea.currentParagraphProperty());
        IntFunction<Node> graphicFactory = line -> {
            HBox hbox = new HBox(
                numberFactory.apply(line),
                arrowFactory.apply(line, errorList, errorListType));
            hbox.setAlignment(Pos.CENTER_LEFT);
            return hbox;
        };
        codeArea.setParagraphGraphicFactory(graphicFactory);


        codeArea.richChanges()
                .filter(ch -> !ch.getInserted().equals(ch.getRemoved()))
                .subscribe(change -> {
                    setUnsaved(true);
                    codeArea.setStyleSpans(0, computeHighlighting(this, codeArea, codeArea.getText()));
                    onKey(this, codeArea.getText());
                });
        codeArea.replaceText(0, 0, sampleCode);
        setUnsaved(false);

        /*codeArea.setMouseOverTextDelay(Duration.ofSeconds(1));
        codeArea.addEventHandler(MouseOverTextEvent.MOUSE_OVER_TEXT_BEGIN, e -> {
            int chIdx = e.getCharacterIndex();
            Point2D pos = e.getScreenPosition();
            //popupMsg.setText("Character '" + area.getText(chIdx, chIdx+1) + "' at " + pos);
            //popup.show(area, pos.getX(), pos.getY() + 10);
            System.out.print(codeArea.getText(chIdx, chIdx+1) + " at " + pos);
        });
        codeArea.addEventHandler(MouseOverTextEvent.MOUSE_OVER_TEXT_END, e -> {

        });*/
        codeArea.setOnKeyPressed((e) -> {
            onCommand(this, e);

            if (e.getCode() == KeyCode.LEFT ||
                e.getCode() == KeyCode.RIGHT ||
                e.getCode() == KeyCode.UP ||
                e.getCode() == KeyCode.DOWN ||
                e.getCode() == KeyCode.HOME ||
                e.getCode() == KeyCode.END ||
                e.getCode() == KeyCode.PAGE_UP ||
                e.getCode() == KeyCode.PAGE_DOWN)
            onKey(this, codeArea.getText());

            if (e.getCode() == KeyCode.ENTER)
                onTabOrEnter(true);

            if (e.getCode() == KeyCode.TAB)
                onTabOrEnter(false);
        });
        codeArea.setOnMouseReleased(e -> {
            onKey(this, codeArea.getText());
        });

        VirtualizedScrollPane codeAreaPane = /*new StackPane*/(new VirtualizedScrollPane<>(codeArea));
        //ScrollPane codeAreaPane = new ScrollPane(codeArea);
        //codeAreaPane.shouldDisplayHorizontal = true;
        GridPane.setHgrow(codeAreaPane, Priority.ALWAYS);
        GridPane.setVgrow(codeAreaPane, Priority.ALWAYS);
        //codeAreaPane.setFitToHeight(true);
        //codeAreaPane.setFitToWidth(true);
        //codeAreaPane.setPrefSize(256, 512);
        root.add(codeAreaPane, 0, 1, 1, 1);
    }

    public void setUnsaved(boolean un) {
        unsaved = un;
        if (unsaved) {
            Circle rectangle = new Circle(4, 4, 4, Paint.valueOf("#4181F2"));
            this.setGraphic(rectangle);
        }
        else {
            this.setGraphic(null);
        }
    }
    public boolean openFile(String fileName) {
        try {
            this.fileName = fileName;
            createCodeArea(root, new String(Files.readAllBytes(Paths.get(fileName))).replaceAll("\t", "    "));
            this.setText(new File(fileName).getName());
            codeArea.getUndoManager().forgetHistory();
            this.setUnsaved(false);
        }
        catch (Exception e) { return false; }
        return true;
    }
    public void goToLine(int line) {
        codeArea.moveTo(line, 1);
        codeArea.requestFollowCaret();
    }

    public void clearErrorLines() {
        errorList.clear();
        errorListType.clear();
    }
    public void setErrorLine(int line, String error) {
        errorList.add(line);
        errorListType.add(error);
    }

    public void saveFile(String fileName) {
        try {
            PrintWriter writer = new PrintWriter(fileName, "UTF-8");
            writer.println(codeArea.getText());
            writer.close();
        }
        catch (Exception e) { }
    }

    private void updateParagraphStyleInSelection() {
        IndexRange selection = codeArea.getSelection();
        int startPar = codeArea.offsetToPosition(selection.getStart(), Forward).getMajor();
        int endPar = codeArea.offsetToPosition(selection.getEnd(), Backward).getMajor();
        startPar = 0;
        endPar = 4;
    }


    private void onCommand(TextEditor self, KeyEvent e) {
        if (e.isControlDown() &&
            e.getCode() == KeyCode.S) {
            saveFile(fileName);
            setUnsaved(false);
        }
    }
    private void onKey(TextEditor self, String text) {
        if (self.guiStatusCurrentFunction == null) return;

        self.updateParagraphStyleInSelection();

        self.guiStatusCurrentFunction.getChildren().clear();
        int pos = codeArea.getCaretPosition();

        if (pos >= text.length()) return;

        int parenLeft = -1;
        int parenRight = -1;
        int parenStack = 1;
        boolean foundIdentifier = false;
        for (int i = pos; i >= 0 && i >= pos - 256; i--) {
            if (text.charAt(i) == ')' && i != pos) {
                parenStack++;
            }
            if (text.charAt(i) == '(') {
                parenStack--;
                if (parenStack == 0) {
                    boolean isFunction = false;
                    int n;
                    for (n = i - 1; n >= i - 16; n--) {
                        if (Character.isWhitespace(text.charAt(n)))
                            continue;
                        if (!Character.isJavaIdentifierPart(text.charAt(n))) {
                            isFunction = false;
                            break;
                        }
                        if (Character.isJavaIdentifierPart(text.charAt(n))) {
                            isFunction = true;
                            break;
                        }
                    }
                    if (isFunction) {
                        parenLeft = i;
                        break;
                    }
                    else {
                        parenStack = 1;
                    }
                }
            }
        }
        if (parenLeft != -1) {
            parenStack = 0;
            for (int i = parenLeft; i < text.length() && i < parenLeft + 256; i++) {
                if (text.charAt(i) == '(') {
                    parenStack++;
                }
                if (text.charAt(i) == ')') {
                    parenStack--;
                    if (parenStack == 0) {
                        parenRight = i;
                        break;
                    }
                }
            }
        }

        // Look for function name first
        String finalIdentifier = "";
        boolean inList = false;
        int inListIndex = -1;
        if (parenRight != -1) {
            for (int i = parenLeft; i >= 0 && i >= parenLeft - 256; i--) {
                if (Character.isJavaIdentifierPart(text.charAt(i))) {
                    foundIdentifier = true;
                }
                else if (text.charAt(i) == '-' ||
                        text.charAt(i) == '+' ||
                        text.charAt(i) == '%' ||
                        text.charAt(i) == '*' ||
                        text.charAt(i) == '/' ||
                        text.charAt(i) == '^' ||
                        text.charAt(i) == '!' ||
                        text.charAt(i) == '=' ||
                        text.charAt(i) == '&' ||
                        text.charAt(i) == ',' ||
                        text.charAt(i) == ')') {
                    parenLeft = -1;
                    parenRight = -1;
                    foundIdentifier = false;
                    break;
                }
                else if ((
                        Character.isWhitespace(text.charAt(i)) ||
                        !Character.isJavaIdentifierPart(text.charAt(i)))
                        && foundIdentifier && (language == "C" && text.charAt(i) == ':' ? false : true)) {
                    finalIdentifier = text.substring(i + 1, parenLeft).trim();
                    break;
                }
            }

            if (Arrays.asList(KEYWORDS).indexOf(finalIdentifier) >= 0) {
                return;
            }

            Text text_identifier = new Text(finalIdentifier);
            for (int n = 0; n < functions.length; n++) {
                if (functions[n].indexOf("(") < 0)
                    break;

                String[] funcSplit = functions[n].substring(0, functions[n].indexOf("(")).split(" ");
                String funcName = "";
                if (funcSplit.length <= 1) {
                    funcName = functions[n].substring(0, functions[n].indexOf("("));
                }
                else {
                    funcName = funcSplit[1];
                }
                if (funcName.startsWith(finalIdentifier)) {
                    inList = true;
                    inListIndex = n;
                    text_identifier.setText(funcName);
                }
            }
            if (inList) {
                text_identifier.setId("blue");
            }
            else {
                text_identifier.setId("non-bold");
            }
            self.guiStatusCurrentFunction.getChildren().add(text_identifier);
        }


        // get current argument
        if (parenRight != -1) {
            String[] arguments = text.substring(parenLeft + 1, parenRight).split(",");
            int s = 0;

            Text text_parenLeft = new Text("(");
            text_parenLeft.setId("non-bold");
            self.guiStatusCurrentFunction.getChildren().add(text_parenLeft);

            if (!inList) {
                for (int i = 0; i < arguments.length; i++) {
                    if (arguments[i].isEmpty())
                        break;

                    Text text_argument = new Text(arguments[i]);
                    if (pos - (parenLeft + 1) >= s && pos - (parenLeft + 1) <= s + arguments[i].length())
                        text_argument.setId("bold");
                    else
                        text_argument.setId("non-bold");
                    self.guiStatusCurrentFunction.getChildren().add(text_argument);

                    if (i < arguments.length - 1) {
                        text_argument = new Text(", ");
                        text_argument.setId("non-bold");
                        self.guiStatusCurrentFunction.getChildren().add(text_argument);
                    }

                    s += arguments[i].length() + ",".length();
                }
            }
            else {
                String[] funcArgs = functions[inListIndex].substring(functions[inListIndex].indexOf("(") + 1, functions[inListIndex].indexOf(")")).split(",");
                for (int i = 0; i < funcArgs.length; i++) {

                    Text text_argument = new Text(funcArgs[i]);
                    if (i < arguments.length) {
                        if (pos - (parenLeft + 1) >= s && pos - (parenLeft + 1) <= s + arguments[i].length())
                            text_argument.setId("bold");
                        else
                            text_argument.setId("non-bold");
                    }
                    else {
                        text_argument.setId("non-bold");
                    }
                    self.guiStatusCurrentFunction.getChildren().add(text_argument);

                    if (i < funcArgs.length - 1) {
                        text_argument = new Text(", ");
                        text_argument.setId("non-bold");
                        self.guiStatusCurrentFunction.getChildren().add(text_argument);
                    }

                    if (i < arguments.length) {
                        s += arguments[i].length() + ",".length();
                    }
                }
            }

            Text text_parenRight = new Text(")");
            text_parenRight.setId("non-bold");
            self.guiStatusCurrentFunction.getChildren().add(text_parenRight);

            //self.guiStatusCurrentFunction.setText(text.substring(parenLeft, parenRight + 1));
        }
    }
    private void onTabOrEnter(boolean enter) {
        if (enter) {
            int newLine = -1;
            int endSpace = -1;
            String text = codeArea.getText();
            for (int i = codeArea.getCaretPosition() - 2; i >= 0; i--) {
                if (text.charAt(i) == '\n') {
                    newLine = i;
                    break;
                }
            }
            if (newLine != -1) {
                for (int i = newLine + 1; i < text.length(); i++) {
                    if (text.charAt(i) != ' ') {
                        endSpace = i;
                        break;
                    }
                }
                codeArea.insertText(codeArea.getCaretPosition(), text.substring(newLine + 1, endSpace));
                //codeArea.moveTo(codeArea.getCaretPosition() + 4);
            }
        }
        else {
            codeArea.replaceText(codeArea.getCaretPosition() - 1, codeArea.getCaretPosition(), "    ");
            //codeArea.moveTo(codeArea.getCaretPosition() + 4);
        }
    }

    public TextEditor() {
        System.setProperty("prism.lcdtext", "false");
        System.setProperty("prism.text", "t2k");

        root = new GridPane();
        ColumnConstraints column = new ColumnConstraints();
        column.setPercentWidth(100);
        root.getColumnConstraints().add(column);

        createCodeArea(root, sampleCode);

        root.setCache(true);
        root.setCacheShape(true);
        root.setCacheHint(CacheHint.SPEED);

        HBox hbStatusBar = new HBox();
        hbStatusBar.setId("ie-status-bar");
        HBox hbStatusBar1 = new HBox();
        HBox hbStatusBar2 = new HBox();
        hbStatusBar.setSpacing(10);
        hbStatusBar.setAlignment(Pos.CENTER_LEFT);
        hbStatusBar.setPadding(new Insets(5, 5, 5, 5));
        root.add(hbStatusBar, 0, 3, 1, 1);
        hbStatusBar1.setAlignment(Pos.CENTER_LEFT);
        hbStatusBar2.setAlignment(Pos.CENTER_RIGHT);
        HBox.setHgrow(hbStatusBar1, Priority.ALWAYS);
        hbStatusBar.getChildren().addAll(hbStatusBar1, hbStatusBar2);

        // Create status bar
        Label guiStatusLabel = new Label("");
        guiStatusCurrentFunction = new TextFlow();
        hbStatusBar1.getChildren().add(guiStatusLabel);
        hbStatusBar2.getChildren().add(guiStatusCurrentFunction);

        GridPane.setHgrow(root, Priority.ALWAYS);
        GridPane.setVgrow(root, Priority.ALWAYS);

        this.setText("Test File");
        this.setContent(root);
        this.setClosable(true);
       // this.setGraphic(new Rectangle(0, 0, 8, 20));
    }

    private static StyleSpans<Collection<String>> computeHighlighting(TextEditor self, StyleClassedTextArea codeArea, String text) {
        int textOffset = 0;
        Matcher matcher = PATTERN.matcher(text.substring(textOffset, textOffset + text.length()));
        int lastKwEnd = 0;
        StyleSpansBuilder<Collection<String>> spansBuilder = new StyleSpansBuilder<>();

        while (matcher.find()) {
            String styleClass =
                matcher.group("KEYWORD") != null ? "keyword" :
                matcher.group("PAREN") != null ? "paren" :
                matcher.group("BRACE") != null ? "brace" :
                matcher.group("BRACKET") != null ? "bracket" :
                matcher.group("SEMICOLON") != null ? "semicolon" :
                matcher.group("CLASSES") != null ? "classes" :
                matcher.group("STRING") != null ? "string" :
                matcher.group("COMMENT") != null ? "comment" :
                matcher.group("METHOD") != null ? "method" :
                matcher.group("CONSTANT") != null ? "constant" :
                        null; /* never happens */ assert styleClass != null;

            spansBuilder.add(Collections.emptyList(), textOffset + matcher.start() - lastKwEnd);
            lastKwEnd = textOffset + matcher.end();

            spansBuilder.add(Collections.singleton(styleClass), matcher.end() - matcher.start());
        }
        spansBuilder.add(Collections.emptyList(), text.length() - lastKwEnd);

        // Checking for function ([A-z0-9]+)(\\s)*\\(([^\\)\\\\]|\\\\.)*\\)
        return spansBuilder.create();
    }
}