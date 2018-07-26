package CodeEditor;

import javafx.beans.InvalidationListener;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.HBox;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Polygon;
import org.reactfx.value.Val;

import java.util.ArrayList;
import java.util.function.IntFunction;

/**
 * Given the line number, return a node (graphic) to display to the left of a line.
 */
public class ArrowFactory implements IntFunction<Node> {
    private final ObservableValue<Integer> shownLine;

    ArrowFactory(ObservableValue<Integer> shownLine) {
        this.shownLine = shownLine;
    }

    @Override
    public Node apply(int lineNumber) {
        return null;
    }
    public Node apply(int lineNumber, ArrayList<Integer> arrayNum, ArrayList<String> array) {
        Circle circle = new Circle(4, 4, 4, Paint.valueOf("#F24141"));
        Circle padding = new Circle(4, 4, 4);
        padding.setFill(Color.TRANSPARENT);

        HBox hbox = new HBox(
                circle,
                padding);
        hbox.setAlignment(Pos.CENTER_LEFT);
        if (arrayNum.indexOf(lineNumber) > -1) {
            Tooltip.install(circle, new Tooltip(array.get(arrayNum.indexOf(lineNumber))));
        }

        ObservableValue<Boolean> visible = Val.map(shownLine, sl -> arrayNum.indexOf(lineNumber) != -1);

        hbox.visibleProperty().bind(
            Val.flatMap(hbox.sceneProperty(), scene -> scene != null ? visible : Val.constant(false)));

        //ObservableValue<Boolean> visible = Val.map(shownLine, );

        //hbox.setVisible(shownLine.indexOf(lineNumber) > -1);

        return hbox;
    }
}