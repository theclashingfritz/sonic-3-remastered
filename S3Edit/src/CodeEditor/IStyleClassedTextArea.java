package CodeEditor;

import javafx.beans.binding.Bindings;
import javafx.beans.binding.ObjectBinding;
import javafx.scene.Node;
import javafx.scene.control.IndexRange;
import javafx.scene.text.TextFlow;
import org.fxmisc.flowless.Cell;
import org.fxmisc.flowless.VirtualFlow;
import org.fxmisc.richtext.StyleClassedTextArea;
import org.fxmisc.richtext.StyledTextArea;
import org.fxmisc.richtext.model.Paragraph;
import org.fxmisc.richtext.model.StyledSegment;
import org.reactfx.EventStreams;
import org.reactfx.Subscription;
import org.reactfx.value.Val;

import java.util.function.BiConsumer;
import java.util.function.Function;

/**
 * Created by Justin on 12/29/2017.
 */
public class IStyleClassedTextArea<PS, SEG, S> extends StyleClassedTextArea {

    public VirtualFlow virtualFlow;
    private Node caretSelectionBind;

    public IStyleClassedTextArea() {
        this.clear();
        virtualFlow = VirtualFlow.createVertical(
                getParagraphs(),
                /*par -> {
                    Cell<Paragraph<PS, SEG, S>, ParagraphBox<PS, SEG, S>> cell = createCell(
                            par,
                            applyParagraphStyle,
                            nodeFactory);
                    nonEmptyCells.add(cell.getNode());
                    return cell.beforeReset(() -> nonEmptyCells.remove(cell.getNode()))
                            .afterUpdateItem(p -> nonEmptyCells.add(cell.getNode()));
                }*/ null);
        virtualFlow.getCell(0).getNode().setVisible(false);
    }

    private Cell<Paragraph<PS, SEG, S>, ParagraphBox<PS, SEG, S>> createCell(
            Paragraph<PS, SEG, S> paragraph,
            BiConsumer<TextFlow, PS> applyParagraphStyle,
            Function<StyledSegment<SEG, S>, Node> nodeFactory) {

        ParagraphBox<PS, SEG, S> box = new ParagraphBox<>(paragraph, applyParagraphStyle, nodeFactory);

        //box.highlightFillProperty().bind(highlightFill);
        //box.highlightTextFillProperty().bind(highlightTextFill);
        box.wrapTextProperty().bind(wrapTextProperty());
        box.graphicFactoryProperty().bind(paragraphGraphicFactoryProperty());
        box.graphicOffset.bind(virtualFlow.breadthOffsetProperty());

        Val<Boolean> hasCaret = Val.combine(
                box.indexProperty(),
                currentParagraphProperty(),
                (bi, cp) -> bi.intValue() == cp.intValue());

        /*Subscription hasCaretPseudoClass = hasCaret.values().subscribe(value -> box.pseudoClassStateChanged(HAS_CARET, value));
        Subscription firstParPseudoClass = box.indexProperty().values().subscribe(idx -> box.pseudoClassStateChanged(FIRST_PAR, idx == 0));
        Subscription lastParPseudoClass = EventStreams.combine(
                box.indexProperty().values(),
                getParagraphs().sizeProperty().values()
        ).subscribe(in -> in.exec((i, n) -> box.pseudoClassStateChanged(LAST_PAR, i == n-1)));*/

        // caret is visible only in the paragraph with the caret
        Val<Boolean> cellCaretVisible = hasCaret.flatMap(x -> x ? caretSelectionBind.visibleProperty() : Val.constant(false));
        box.caretVisibleProperty().bind(cellCaretVisible);

        // bind cell's caret position to area's caret column,
        // when the cell is the one with the caret
        box.caretPositionProperty().bind(hasCaret.flatMap(has -> has
                ? caretColumnProperty()
                : Val.constant(0)));

        // keep paragraph selection updated
        ObjectBinding<IndexRange> cellSelection = Bindings.createObjectBinding(() -> {
            int idx = box.getIndex();
            return idx != -1
                    ? getParagraphSelection(idx)
                    : StyledTextArea.EMPTY_RANGE;
        }, selectionProperty(), box.indexProperty());
        box.selectionProperty().bind(cellSelection);

        return new Cell<Paragraph<PS, SEG, S>, ParagraphBox<PS, SEG, S>>() {
            @Override
            public ParagraphBox<PS, SEG, S> getNode() {
                return box;
            }

            @Override
            public void updateIndex(int index) {
                box.setIndex(index);
            }

            @Override
            public void dispose() {
                box.highlightFillProperty().unbind();
                box.highlightTextFillProperty().unbind();
                box.wrapTextProperty().unbind();
                box.graphicFactoryProperty().unbind();
                box.graphicOffset.unbind();

                //hasCaretPseudoClass.unsubscribe();
                //firstParPseudoClass.unsubscribe();
                //lastParPseudoClass.unsubscribe();

                box.caretVisibleProperty().unbind();
                box.caretPositionProperty().unbind();

                box.selectionProperty().unbind();
                cellSelection.dispose();
            }
        };
    }
}
