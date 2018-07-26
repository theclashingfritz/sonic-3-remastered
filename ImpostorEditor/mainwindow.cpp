#include <QtWidgets>
#ifdef W_OS_MACX
    #include <QtMac>
#endif

#include "mainwindow.h"
#include <KGL/Widgets/QCodeEditor.hpp>

using namespace kgl;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QFont font;
    font.setFamily("Menlo");
    font.setFixedPitch(true);
    font.setPixelSize(14);
    //font.setPixelSize(22);

    CustomTextEdit *editor = new CustomTextEdit();
    editor->setFont(font);
    //editor->setTabStopDistance(QFontMetricsF(font).width("        "));
    editor->setWordWrapMode(QTextOption::NoWrap);

    //QCodeEditor *editor = new QCodeEditor;

    QFile f("/users/justin/dropbox/sonic3/source/Player.cpp");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&f);
        editor->setPlainText(in.readAll());
        f.close();
    }
    //editor->updateAllFolds();


    #ifdef W_OS_MACX
        QtMac::setBadgeLabelText("▶");
    #endif

    //Highlighter *highlighter =
            new Highlighter(editor->document(), editor);
    //editor->enableCheck = true;

    setCentralWidget(editor);
    setWindowTitle("ImpostorEditor");
    setMinimumSize(800, 550);
}

MainWindow::~MainWindow() {

}
