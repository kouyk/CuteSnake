#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QLCDNumber>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("My Snake Game"));
    gamearea = new GameArea(this);

    createActions();
    createMenus();
    createToolbar();
    createButtons();

    QLCDNumber *clock = new QLCDNumber(this);
    clock->setMaximumHeight(80);
    clock->setSegmentStyle(QLCDNumber::Filled);

    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setLayout(new QVBoxLayout);

    buttonWidget->layout()->addWidget(openButton);
    buttonWidget->layout()->addWidget(saveButton);
    buttonWidget->layout()->addWidget(startButton);
    buttonWidget->layout()->addWidget(pauseButton);
    buttonWidget->layout()->addWidget(resumeButton);
    buttonWidget->layout()->addWidget(resetButton);
    buttonWidget->layout()->addWidget(exitButton);

    QWidget *c_widget = new QWidget(this);
    setCentralWidget(c_widget);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setAlignment(Qt::AlignTop);
    c_widget->setLayout(gridLayout);

    gridLayout->addWidget(gamearea, 0, 0, -1, 6);
    gridLayout->setRowStretch(0, 1);

    gridLayout->addWidget(clock, 0, 7);
    gridLayout->addWidget(buttonWidget, 1, 7);

    connect(this, SIGNAL(toggleState()), gamearea, SLOT(stateChanged()));
    connect(gamearea, SIGNAL(gameResumed()), this, SLOT(gameHasStarted()));
    connect(gamearea, SIGNAL(gamePaused()), this, SLOT(gameHasPaused()));
    connect(gamearea, SIGNAL(gameReset()), this, SLOT(gameHasReset()));
    connect(gamearea, SIGNAL(gameEnded()), this, SLOT(gameHasStopped()));
    connect(gamearea, SIGNAL(updateTimer(int)), clock, SLOT(display(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        emit toggleState();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Load..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a saved game"));
    connect(openAct, SIGNAL(triggered()), gamearea, SLOT(loadGame()));

    saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the current game state to disk"));
    saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), gamearea, SLOT(saveGame()));

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    startAct = new QAction(tr("Start"), this);
    startAct->setStatusTip(tr("Begin the game"));
    connect(startAct, SIGNAL(triggered()), gamearea, SLOT(startGame()));

    pauseAct = new QAction(tr("Pause"), this);
    pauseAct->setStatusTip(tr("Pause the game"));
    pauseAct->setEnabled(false);
    connect(pauseAct, SIGNAL(triggered()), gamearea, SLOT(pauseGame()));

    resumeAct = new QAction(tr("Resume"), this);
    resumeAct->setStatusTip(tr("Continue the game"));
    resumeAct->setEnabled(false);
    connect(resumeAct, SIGNAL(triggered()), gamearea, SLOT(resumeGame()));

    resetAct = new QAction(tr("&Reset"), this);
    resetAct->setStatusTip(tr("Reset the game and restore to default state"));
    resetAct->setEnabled(false);
    connect(resetAct, SIGNAL(triggered()), gamearea, SLOT(resetGame()));
}

void MainWindow::createMenus()
{
    fileMenu = ui->menubar->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    gameMenu = ui->menubar->addMenu(tr("Game"));
    gameMenu->addAction(startAct);
    gameMenu->addAction(pauseAct);
    gameMenu->addAction(resumeAct);
    gameMenu->addAction(resetAct);
}

void MainWindow::createToolbar()
{
    ui->toolBar->addAction(openAct);
    ui->toolBar->addAction(saveAct);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(startAct);
    ui->toolBar->addAction(pauseAct);
    ui->toolBar->addAction(resumeAct);
    ui->toolBar->addAction(resetAct);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(exitAct);
}

void MainWindow::createButtons()
{
    QSizePolicy policy(QSizePolicy::Minimum,
                       QSizePolicy::MinimumExpanding);

    openButton = new QPushButton(tr("&Load..."));
    openButton->setSizePolicy(policy);
    connect(openButton, SIGNAL(pressed()), gamearea, SLOT(loadGame()));

    saveButton = new QPushButton(tr("&Save..."));
    saveButton->setSizePolicy(policy);
    connect(saveButton, SIGNAL(pressed()), gamearea, SLOT(saveGame()));
    saveButton->setEnabled(false);

    exitButton = new QPushButton(tr("&Exit"));
    exitButton->setSizePolicy(policy);
    connect(exitButton, &QPushButton::pressed, this, &QWidget::close);

    startButton = new QPushButton(tr("Start"));
    startButton->setSizePolicy(policy);
    connect(startButton, SIGNAL(pressed()), gamearea, SLOT(startGame()));
    startButton->setEnabled(true);

    pauseButton = new QPushButton(tr("Pause"));
    pauseButton->setSizePolicy(policy);
    connect(pauseButton, SIGNAL(pressed()), gamearea, SLOT(pauseGame()));
    pauseButton->setEnabled(false);

    resumeButton = new QPushButton(tr("Resume"));
    resumeButton->setSizePolicy(policy);
    connect(resumeButton, SIGNAL(pressed()), gamearea, SLOT(resumeGame()));
    resumeButton->setEnabled(false);

    resetButton = new QPushButton(tr("&Reset"));
    resetButton->setSizePolicy(policy);
    connect(resetButton, SIGNAL(pressed()), gamearea, SLOT(resetGame()));
    resetButton->setEnabled(false);
}

void MainWindow::gameHasReset()
{
    openAct->setEnabled(true);
    saveAct->setEnabled(false);
    startAct->setEnabled(true);
    pauseAct->setEnabled(false);
    resumeAct->setEnabled(false);
    resetAct->setEnabled(false);

    openButton->setEnabled(true);
    saveButton->setEnabled(false);
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resumeButton->setEnabled(false);
    resetButton->setEnabled(false);
}

void MainWindow::gameHasStarted()
{
    openAct->setEnabled(false);
    saveAct->setEnabled(false);
    startAct->setEnabled(false);
    pauseAct->setEnabled(true);
    resumeAct->setEnabled(false);
    resetAct->setEnabled(false);

    openButton->setEnabled(false);
    saveButton->setEnabled(false);
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    resumeButton->setEnabled(false);
    resetButton->setEnabled(false);
}

void MainWindow::gameHasStopped()
{
    openAct->setEnabled(false);
    saveAct->setEnabled(false);
    startAct->setEnabled(false);
    pauseAct->setEnabled(false);
    resumeAct->setEnabled(false);
    resetAct->setEnabled(true);

    openButton->setEnabled(false);
    saveButton->setEnabled(false);
    startButton->setEnabled(false);
    pauseButton->setEnabled(false);
    resumeButton->setEnabled(false);
    resetButton->setEnabled(true);
}

void MainWindow::gameHasPaused()
{
    openAct->setEnabled(false);
    saveAct->setEnabled(true);
    startAct->setEnabled(false);
    pauseAct->setEnabled(false);
    resumeAct->setEnabled(true);
    resetAct->setEnabled(true);

    openButton->setEnabled(false);
    saveButton->setEnabled(true);
    startButton->setEnabled(false);
    pauseButton->setEnabled(false);
    resumeButton->setEnabled(true);
    resetButton->setEnabled(true);
}
