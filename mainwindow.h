#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "gamearea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QMenu *gameMenu;
    QAction *startAct;
    QAction *pauseAct;
    QAction *resumeAct;
    QAction *resetAct;

    QPushButton *openButton;
    QPushButton *saveButton;
    QPushButton *exitButton;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *resumeButton;
    QPushButton *resetButton;

    GameArea *gamearea;

    void createActions();
    void createMenus();
    void createToolbar();
    void createButtons();

private slots:
    void gameHasStarted();
    void gameHasPaused();
    void gameHasReset();
    void gameHasStopped();

signals:
    void toggleState();
};
#endif // MAINWINDOW_H
