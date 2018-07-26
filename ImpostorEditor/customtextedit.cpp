#include <QtWidgets>
#include <QtDebug>
#include <QStack>

#include "customtextedit.h"

CustomTextEdit::CustomTextEdit(QWidget *) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    QPalette p = palette();
    p.setColor(QPalette::Base, clr_background);
    p.setColor(QPalette::Text, clr_text);
    p.setColor(QPalette::Highlight, clr_highlight);
    setPalette(p);

    setContentsMargins(0, 0, 0, 0);
    setCursorWidth(0);
    setCenterOnScroll(false);
    qApp->setCursorFlashTime(0);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCenterOnScroll(false);

    document()->setDocumentMargin(0);
    document()->setDefaultStyleSheet("margin: 40px;");
    //layout()->setSpacing(10);
    setStyleSheet("p { margin: 40px; }");
    //document()->setDefaultStyleSheet("* { line-height: 40px; }");

    for (int i = 0; i < 10000; i++) {
        hiddenLines[i] = false;
        lineFoldable[i] = false;
    }
}
int  CustomTextEdit::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 20 + fontMetrics().width("9999");// * digits;
    return space;
}
void CustomTextEdit::updateLineNumberAreaWidth(int b) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void CustomTextEdit::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
void CustomTextEdit::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CustomTextEdit::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
void CustomTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), clr_background);

    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

    QTextBlock block = document()->begin();
    block = firstVisibleBlock();
    while (!block.isVisible()) {
        block = block.next();
    }

    int blockNumber = block.blockNumber();
    double top = blockBoundingGeometry(block).translated(contentOffset()).top();
    double bottom = top + blockBoundingRect(block).height() + lineSpacing;
    int b_hei = blockBoundingRect(block).height();

    QFont smallFont(font());
    //smallFont.setPointSizeF(14);
    smallFont.setPixelSize(14);
    painter.setFont(smallFont);
    painter.setPen(clr_text);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            if (block.contains(textCursor().position()))
                painter.fillRect(QRectF(0, top, lineNumberArea->width() + 4, b_hei), QColor(clr_highlight.red(), clr_highlight.green(), clr_highlight.blue(), 16));

            int blockData = 0;
            if ((CustomTextBlockUserData *)block.userData() != NULL)
                blockData = ((CustomTextBlockUserData *)block.userData())->data;


            if (blockData == 1)
                painter.setPen(clr_number);
            painter.drawText(0, top, lineNumberArea->width() - 24, b_hei, Qt::AlignRight | Qt::AlignVCenter, number);

            if (lineFoldable[blockNumber]) {
                smallFont.setBold(true);
                painter.setFont(font());
                painter.setPen(clr_comment);

                painter.save();
                    painter.translate(lineNumberArea->width() - 20 / 2, top - 1.375 + b_hei / 2);

                    // block.userState()
                    if (hiddenLines[blockNumber] || blockData == 1)
                        painter.rotate(90);

                    painter.drawText(QRectF(-10, -b_hei / 2, 20, b_hei), Qt::AlignCenter, "›");
                painter.restore();

                //painter.drawText(QRectF(0, top - 1.375, lineNumberArea->width() - 4, b_hei), Qt::AlignRight | Qt::AlignVCenter, "›");

                //painter.drawText(QRectF(lineNumberArea->width() - 20, top - 1.375, 20, b_hei), Qt::AlignCenter, "›");

                smallFont.setBold(false);
                painter.setFont(smallFont);
                painter.setPen(clr_text);
            }
        }
        if (block.isVisible()) {
            top = bottom;
            bottom = top + b_hei;
        }
        block = block.next();
        ++blockNumber;
    }
}
void CustomTextEdit::lineNumberAreaMouseEvent(QMouseEvent *e) {
    if (e->type() == QMouseEvent::MouseButtonPress) {
        QTextBlock block = firstVisibleBlock();

        int blockNumber = block.blockNumber();
        int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
        int bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;

        QFont smallFont(font());
        smallFont.setPointSizeF(14);

        while (block.isValid() && top <= lineNumberArea->visibleRegion().boundingRect().bottom()) {
            if (block.isVisible() && bottom >= lineNumberArea->visibleRegion().boundingRect().top()) {
                if (QRectF(0, top, lineNumberArea->width() + 4, blockBoundingRect(block).height()).contains(e->pos()) && lineFoldable[blockNumber]) {
                    //hiddenLines[blockNumber] = !hiddenLines[blockNumber];

                    int blockData = 0;
                    if ((CustomTextBlockUserData *)block.userData() != NULL)
                        blockData = ((CustomTextBlockUserData *)block.userData())->data;

                    block.setUserData(new CustomTextBlockUserData(1 - blockData));
                    //updateAllFolds();
                    updateFoldAt(blockNumber);
                    break;
                }
                top = bottom;
                bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
            }

            block = block.next();
            ++blockNumber;
        }
    }
}

void CustomTextEdit::updateAllFoldableLines() {
    QTextBlock block = document()->begin();

    int  blockID = block.blockNumber();
    int  lastLen = 1000000;
    bool lastEmpty = false;

    while (block.isValid()) {
        QString txt = block.text();
        int initialBrack = 0;
        bool empty = false;
        for (int i = 0; i < txt.length(); i++) {
            if (txt.at(i) == " ") {
                initialBrack++;
            }
            else {
                break;
            }
        }
        if (txt.length() <= initialBrack) {
            empty = true;
        }
        lineFoldable[blockID] = false;
        if (initialBrack > lastLen && lastLen != -1 && !lastEmpty) {
            lineFoldable[blockID - 1] = true;
        }
        lastLen = initialBrack;
        lastEmpty = empty;

        block = block.next();
        blockID++;
    }
}

void CustomTextEdit::updateFoldAt(int b) {
    QTextBlock block = document()->begin();
    for (int i = 0; i < b; i++) {
        block = block.next();
    }
    int startPos = block.position();
    int endLen = 0;
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;

    QStack<int> hiddenAboveID;
    QStack<int> hiddenAboveLen;
    int    lastLen = 1000000;
    bool   lastEmpty = false;
    int         hiddenAboveLenFirst = 0;

    while (block.isValid()) {
        endLen += block.length();
        QString txt = block.text();
        int initialBrack = 0;
        bool empty = false;
        for (int i = 0; i < txt.length(); i++) {
            if (txt.at(i) == " ") {
                initialBrack++;
            }
            else {
                break;
            }
        }
        if (txt.length() <= initialBrack) {
            empty = true;
        }
        if (blockNumber == b) {
            hiddenAboveLenFirst = initialBrack;
        }
        if (initialBrack >= 0) {
            int blockData = 0;
            if ((CustomTextBlockUserData *)block.userData() != NULL)
                blockData = ((CustomTextBlockUserData *)block.userData())->data;

            if (hiddenLines[blockNumber] || blockData == 1) {
                hiddenAboveID.push(blockNumber);
                hiddenAboveLen.push(initialBrack);
            }

            bool isParen = false;
            if (block.text().mid(initialBrack) == "}") {
                isParen = true;
            }

            if (initialBrack == hiddenAboveLenFirst * 0 && blockNumber > b && !empty && !(hiddenLines[blockNumber] || blockData == 1) && isParen) {
                if (hiddenAboveID.size() > 0) {
                    block.setVisible(false);
                }
                else {
                    block.setVisible(true);
                    top = bottom;
                    bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                }
                break;
            }

            if (!hiddenAboveID.isEmpty()) {
                if (hiddenAboveID.top() != blockNumber) {
                    bool killer = false;
                    if (initialBrack <= hiddenAboveLen.top() && txt.length() != initialBrack) {
                        hiddenAboveID.pop();
                        hiddenAboveLen.pop();

                        killer = true;
                    }

                    if (isParen) {
                        if (hiddenAboveID.size() > 0) {
                            block.setVisible(false);
                        }
                        else if (killer) {
                            block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                    else {
                        if (hiddenAboveID.size() > 0) {
                            if (initialBrack > hiddenAboveLen.top() || empty)
                                block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                }
                else {
                    if (hiddenAboveID.size() > 1) {
                        hiddenAboveID.pop();
                        hiddenAboveLen.pop();
                    }

                    if (hiddenAboveID.size() > 0) {
                        if (initialBrack > hiddenAboveLen.top() || empty) {
                            block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                    else {
                        block.setVisible(true);
                        top = bottom;
                        bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                    }

                }
            }
            else {
                block.setVisible(true);
                top = bottom;
                bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
            }
            lineFoldable[blockNumber] = false;
            if (initialBrack > lastLen && lastLen != -1 && !lastEmpty) {
                lineFoldable[blockNumber - 1] = true;
            }
        }
        lastLen = initialBrack;
        lastEmpty = empty;
        block = block.next();
        blockNumber++;
    }

    document()->markContentsDirty(startPos, endLen);
}
void CustomTextEdit::updateAllFolds() {
    QTextBlock block = document()->begin();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;

    QStack<int> hiddenAboveID;
    QStack<int> hiddenAboveLen;
    int    lastLen = 1000000;
    bool   lastEmpty = false;

    int b = 0;
    while (block.isValid()) {
        QString txt = block.text();
        int initialBrack = 0;
        bool empty = false;
        for (int i = 0; i < txt.length(); i++) {
            if (txt.at(i) == " ") {
                initialBrack++;
            }
            else {
                break;
            }
        }
        if (txt.length() <= initialBrack) {
            empty = true;
        }
        if (initialBrack >= 0) {
            int blockData = 0;
            if ((CustomTextBlockUserData *)block.userData() != NULL)
                blockData = ((CustomTextBlockUserData *)block.userData())->data;

            if (hiddenLines[blockNumber] || blockData == 1) {
                hiddenAboveID.push(blockNumber);
                hiddenAboveLen.push(initialBrack);
            }

            bool isParen = false;
            if (block.text().mid(initialBrack) == "}") {
                isParen = true;
            }

            if (initialBrack == 0 && blockNumber > b && !empty && !(hiddenLines[blockNumber] && blockData == 1) && isParen) {
                if (hiddenAboveID.size() > 0) {
                    block.setVisible(false);
                }
                else {
                    block.setVisible(true);
                    top = bottom;
                    bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                }
                b = blockNumber;
            }

            if (!hiddenAboveID.isEmpty()) {
                if (hiddenAboveID.top() != blockNumber) {
                    bool killer = false;
                    if (initialBrack <= hiddenAboveLen.top() && txt.length() != initialBrack) {
                        hiddenAboveID.pop();
                        hiddenAboveLen.pop();

                        killer = true;
                    }

                    if (isParen) {
                        if (hiddenAboveID.size() > 0) {
                            block.setVisible(false);
                        }
                        else if (killer) {
                            block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                    else {
                        if (hiddenAboveID.size() > 0) {
                            if (initialBrack > hiddenAboveLen.top() || empty)
                                block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                }
                else {
                    if (hiddenAboveID.size() > 1) {
                        hiddenAboveID.pop();
                        hiddenAboveLen.pop();
                    }

                    if (hiddenAboveID.size() > 0) {
                        if (initialBrack > hiddenAboveLen.top() || empty) {
                            block.setVisible(false);
                        }
                        else {
                            block.setVisible(true);
                            top = bottom;
                            bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                        }
                    }
                    else {
                        block.setVisible(true);
                        top = bottom;
                        bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
                    }

                }
            }
            else {
                block.setVisible(true);
                top = bottom;
                bottom = top + (int)blockBoundingRect(block).height() + lineSpacing;
            }
        }
        lineFoldable[blockNumber] = false;
        if (initialBrack > lastLen && lastLen != -1 && !lastEmpty) {
            lineFoldable[blockNumber - 1] = true;
        }
        lastLen = initialBrack;
        lastEmpty = empty;
        block = block.next();
        blockNumber++;

    }
    qDebug() << "hiddenAboveID " << hiddenAboveID;
    qDebug() << "hiddenAboveLen" << hiddenAboveLen;
    document()->markContentsDirty(0, document()->toPlainText().length());
}
QMouseEvent *CustomTextEdit::getNewMouseEvent(QMouseEvent *e) {
    QFont pHolder(font());
    pHolder.setPixelSize(14);
    //painter.setFont(pHolder);

    QPoint newPoint = QPoint(e->x() * QFontMetricsF(font()).width(" ") / QFontMetricsF(pHolder).width(" "), e->y());
    return new QMouseEvent(e->type(), newPoint, mapToGlobal(newPoint), e->button(), e->buttons(), e->modifiers());
}
void CustomTextEdit::mousePressEvent(QMouseEvent *e) {
    e = getNewMouseEvent(e);

    QTextBlock block = document()->begin();
    block = firstVisibleBlock();
    while (!block.isVisible()) {
        block = block.next();
    }

    int blockNumber = block.blockNumber();
    double top = blockBoundingGeometry(block).translated(contentOffset()).top();
    double bottom = top + blockBoundingRect(block).height() + lineSpacing;
    int b_hei = blockBoundingRect(block).height();

    double initialX = viewport()->rect().x();
    double w = QFontMetricsF(fontMetrics()).width("0");

    while (block.isValid() && top <= viewport()->rect().bottom()) {
        if (block.isVisible() && bottom >= viewport()->rect().top()) {
            if (e->y() >= top && e->y() < bottom) {
                qDebug() << "Line" << blockNumber + 1 << "Mouse X" << e->x() << "Character Size" << w << "Select" << qRound(e->x() / w);

                QTextCursor cur = textCursor();
                cur.setPosition(block.position() + qMin(block.length() - 1, qRound(e->x() / w)));
                setTextCursor(cur);

                //viewport()->setGeometry(initialX, viewport()->rect().y(), viewport()->rect().width(), viewport()->rect().height());
            }
        }
        if (block.isVisible()) {
            top = bottom;
            bottom = top + b_hei;
        }
        block = block.next();
        blockNumber++;
    }
    //QPlainTextEdit::mousePressEvent(e);
}
void CustomTextEdit::mouseMoveEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseMoveEvent(getNewMouseEvent(e));
}
void CustomTextEdit::mouseReleaseEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseReleaseEvent(getNewMouseEvent(e));
}
void CustomTextEdit::mouseDoubleClickEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseDoubleClickEvent(getNewMouseEvent(e));
}
void CustomTextEdit::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Backtab) {
        QString doc = document()->toPlainText();
        QTextCursor cur = textCursor();
        int pos = cur.position();
        int beginOfLine = 0;
        for (int i = pos - 1; i >= 0; i--) {
            if (doc.at(i) == '\n') {
                beginOfLine = i + 1;
                break;
            }
        }
        if (doc.mid(beginOfLine, 4) == "    ") {
            doc.remove(beginOfLine, 4);
        }
        document()->setPlainText(doc);
        cur.setPosition(qMax(pos - 4, beginOfLine));
        setTextCursor(cur);
    }
    else if (e->key() == Qt::Key_Tab) {
        QString doc = document()->toPlainText();
        QTextCursor cur = textCursor();
        int pos = cur.position();
        int beginOfLine = 0;
        for (int i = pos - 1; i >= 0; i--) {
            if (doc.at(i) == '\n') {
                beginOfLine = i + 1;
                break;
            }
        }
        doc.insert(pos, QString("    ").left(4 - (pos - beginOfLine) % 4));
        document()->setPlainText(doc);
        if (e->modifiers() == Qt::NoModifier)
            cur.setPosition(pos + 4 - (pos - beginOfLine) % 4);
        else
            cur.setPosition(pos);
        setTextCursor(cur);
    }
    else if (e->key() == Qt::Key_BracketLeft && e->modifiers() == Qt::ControlModifier) {
        qDebug() << e->modifiers() << (e->modifiers() == Qt::ControlModifier);

        updateAllFoldableLines();
        QTextBlock block = document()->begin();
        for (int i = 0; i < document()->blockCount(); i++) {
            if (lineFoldable[i]) {
                block.setUserData(new CustomTextBlockUserData(1));
            }
            block = block.next();
        }
        updateAllFolds();
    }
    else if (e->key() == Qt::Key_BracketRight && e->modifiers() == Qt::ControlModifier) {
        qDebug() << e->modifiers() << (e->modifiers() == Qt::ControlModifier);

        updateAllFoldableLines();
        QTextBlock block = document()->begin();
        for (int i = 0; i < document()->blockCount(); i++) {
            if (lineFoldable[i]) {
                block.setUserData(new CustomTextBlockUserData(0));
            }
            block = block.next();
        }
        updateAllFolds();
    }
    else {
        QPlainTextEdit::keyPressEvent(e);
    }
}

void CustomTextEdit::ensureCursorVisible() {
    qDebug() << "sup";
}

/*static void fillBackground(QPainter *p, const QRectF &r, QBrush brush, const QRectF &gradientRect = QRectF())
{
    QRectF rect(r.topLeft(), QSizeF(200, 200));

    //rect.moveBottom(200);

    p->save();
    if (brush.style() >= Qt::LinearGradientPattern && brush.style() <= Qt::ConicalGradientPattern) {
        if (!gradientRect.isNull()) {
            QTransform m = QTransform::fromTranslate(gradientRect.left(), gradientRect.top());
            m.scale(gradientRect.width(), gradientRect.height());
            brush.setTransform(m);
            const_cast<QGradient *>(brush.gradient())->setCoordinateMode(QGradient::LogicalMode);
        }
    } else {
        p->setBrushOrigin(rect.topLeft());
    }
    p->fillRect(rect, brush);
    p->restore();
}*/

void CustomTextEdit::paintEvent(QPaintEvent *e) {
    QTextCursor cursor(document());
    QTextBlock blocque = document()->firstBlock();
    QTextBlockFormat blocqueFormat = blocque.blockFormat();
    blocqueFormat.setLineHeight(200, QTextBlockFormat::SingleHeight);
    blocqueFormat.setProperty(QTextBlockFormat::LineHeight, 200);
    cursor.setBlockFormat(blocqueFormat);


    QTextCursor c(document());

    QPainter painter(viewport());
    Q_ASSERT(qobject_cast<QPlainTextDocumentLayout*>(document()->documentLayout()));

    QPointF offset(contentOffset());

    QRect er = e->rect();
    QRect viewportRect = viewport()->rect();

    bool editable = !isReadOnly();

    QTextBlock block = firstVisibleBlock();
    while (!block.isVisible()) {
        block = block.next();
    }

    //qDebug() << "blockNumber" << block.blockNumber() << "block.isVisible()" << block.isVisible();

    qreal maximumWidth = document()->documentLayout()->documentSize().width();

    painter.setBrushOrigin(offset);

    er.setLeft(0);
    er.setRight(viewportRect.right());
    painter.setClipRect(er);

    offset.setX(qFloor(offset.x()));

    QAbstractTextDocumentLayout::PaintContext context = getPaintContext();

    c.movePosition(QTextCursor::Start);
    QTextBlockFormat f = c.blockFormat();
    f.setLineHeight(100, QTextBlockFormat::LineDistanceHeight);

    double r_height = blockBoundingRect(block).translated(offset).height();

    QFont pHolder(font());
    //pHolder.setPixelSize(14);
    //painter.setFont(pHolder);
    QFontMetricsF qfmf(pHolder);

    int  blockID = block.blockNumber();
    int  lastLen = 1000000;
    bool lastEmpty = false;

    while (block.isValid()) {
        QRectF r = blockBoundingRect(block).translated(offset);
        QTextLayout *layout = block.layout();

        QString txt = block.text();
        int initialBrack = 0;
        bool empty = false;
        for (int i = 0; i < txt.length(); i++) {
            if (txt.at(i) == " ") {
                initialBrack++;
            }
            else {
                break;
            }
        }
        if (txt.length() <= initialBrack) {
            empty = true;
        }
        lineFoldable[blockID] = false;
        if (initialBrack > lastLen && lastLen != -1 && !lastEmpty) {
            lineFoldable[blockID - 1] = true;
        }
        lastLen = initialBrack;
        lastEmpty = empty;

        if (!block.isVisible()) {
            //offset.ry() += r.height() + lineSpacing;
            block = block.next();
            blockID++;
            continue;
        }
        if (r.bottom() >= er.top() && r.top() <= er.bottom() && false) {
            QTextBlockFormat blockFormat = block.blockFormat();
            QBrush bg = blockFormat.background();
            if (bg != Qt::NoBrush) {
                QRectF contentsRect = r;
                contentsRect.setWidth(qMax(r.width(), maximumWidth));
                //fillBackground(&painter, contentsRect, bg);
            }

            QVector<QTextLayout::FormatRange> selections;
            int blpos = block.position();
            int bllen = block.length();
            for (int i = 0; i < context.selections.size(); i++) {
                const QAbstractTextDocumentLayout::Selection &range = context.selections.at(i);
                const int selStart = range.cursor.selectionStart() - blpos;
                const int selEnd = range.cursor.selectionEnd() - blpos;

                if (selStart < bllen && selEnd > 0 && selEnd > selStart) {
                    int o_start = selStart;
                    //int o_length = selEnd - selStart;

                    QString cleanText = toPlainText();
                    QFontMetricsF qfmf(pHolder);
                    int n_start = qMax(o_start, 0);
                    int n_len = qMin(bllen - n_start, range.cursor.selectionEnd() - (blpos + n_start));
                    double xx = qfmf.width(cleanText.mid(blpos, n_start)/*.replace("\t", "    ")*/);
                    double ww = qfmf.width(cleanText.mid(blpos + n_start, n_len)/*.replace("\t", "    ")*/);

                    painter.fillRect(QRectF(xx + offset.x(), offset.y(), ww, r_height + lineSpacing), QColor(clr_highlight.red(), clr_highlight.green(), clr_highlight.blue(), 96));
                }
                else if (!range.cursor.hasSelection() && range.format.hasProperty(QTextFormat::FullWidthSelection) && block.contains(range.cursor.position())) {
                    QFontMetricsF qfmf(pHolder);
                    painter.fillRect(QRectF(offset.x(), offset.y(), maximumWidth, r_height + lineSpacing), QColor(clr_highlight.red(), clr_highlight.green(), clr_highlight.blue(), 16));
                }
            }

            bool drawCursor = ((editable || (textInteractionFlags() & Qt::TextSelectableByKeyboard))
                && context.cursorPosition >= blpos
                && context.cursorPosition < blpos + bllen);



            offset.ry() += lineSpacing / 2;



            for (int j = 0; j < layout->formats().length(); j++) {
                //QTextLayout::FormatRange form = layout->formats().at(j);
                //painter.setPen(form.format.foreground().color());
                //painter.drawText(QRectF(offset.x() + qfmf.width("0") * form.start, offset.y(), qfmf.width("0") * form.length, r_height), Qt::AlignLeft | Qt::AlignVCenter, block.text().mid(form.start, form.length));
            }

            //layout->draw(&painter, offset, selections, er);

            offset.ry() -= lineSpacing / 2;

            if (drawCursor) {
                int cpos = context.cursorPosition;
                if (cpos < -1)
                    cpos = layout->preeditAreaPosition() - (cpos + 2);
                else
                    cpos -= blpos;

                QString cleanText = toPlainText();

                //offset.ry() += lineSpacing / 2;
                //layout->drawCursor(&painter, offset, cpos, 2);
                painter.fillRect(QRectF(QFontMetricsF(pHolder).width(cleanText.mid(context.cursorPosition - cpos, cpos)/*.replace("\t", "    ")*/) + offset.rx(), offset.ry(), 2, r_height), clr_cursor);
                //offset.ry() -= lineSpacing / 2;
            }
        }

        offset.ry() += r_height + lineSpacing;
        if (offset.y() > viewportRect.height())
            break;
        block = block.next();
        blockID++;
    }

    if (backgroundVisible() && !block.isValid() && offset.y() <= er.bottom()
        && (centerOnScroll() || verticalScrollBar()->maximum() == verticalScrollBar()->minimum())) {
        //painter.fillRect(QRect(QPoint((int)er.left(), (int)offset.y()), er.bottomRight()), palette().background());
    }
}

