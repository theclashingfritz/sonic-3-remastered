#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QPlainTextEdit>
#include <QTextBlockUserData>
#include <QObject>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
class CustomTextBlockUserData;

class CustomTextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    bool enableCheck = false;
    QColor clr_background       = QColor("#282C34");
    QColor clr_function         = QColor("#61AFEF");
    QColor clr_keyword          = QColor("#C479DB");
    QColor clr_special_keyword  = QColor("#56B6C2");
    QColor clr_cursor           = QColor("#61AFEF");
    QColor clr_text             = QColor("#ABB2BF");
    QColor clr_highlight        = QColor(171, 178, 191, 64);
    QColor clr_special_class    = QColor("#E5C07B");
    QColor clr_number           = QColor("#D19A66");
    QColor clr_string           = QColor("#98C379");
    QColor clr_comment          = QColor("#5C6370");

    CustomTextEdit(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void lineNumberAreaMouseEvent(QMouseEvent *e);
    int  lineNumberAreaWidth();
    void updateAllFoldableLines();
    void updateFoldAt(int b);
    void updateAllFolds();
    void ensureCursorVisible();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    QMouseEvent *getNewMouseEvent(QMouseEvent *e);
    bool hiddenLines[10000];
    bool lineFoldable[10000];

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    int lineSpacing = 0;
};

class CustomTextBlockUserData : public QTextBlockUserData {
public:
    int data = 0;
    CustomTextBlockUserData() { }
    CustomTextBlockUserData(int d) { data = d; }
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CustomTextEdit *editor) : QWidget(editor) {
        customTextEdit = editor;
    }

    QSize sizeHint() const override {
        return QSize(customTextEdit->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        customTextEdit->lineNumberAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *e) override {
        customTextEdit->lineNumberAreaMouseEvent(e);
    }
    void mouseMoveEvent(QMouseEvent *e) override {
        customTextEdit->lineNumberAreaMouseEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent *e) override {
        customTextEdit->lineNumberAreaMouseEvent(e);
    }

private:
    CustomTextEdit *customTextEdit;
};

#endif // CUSTOMTEXTEDIT_H
