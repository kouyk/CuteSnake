#ifndef GAMEAREA_H
#define GAMEAREA_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QKeyEvent>
#include <QDebug>
#include <QTime>

#include "parameters.h"
#include "snake.h"
#include "fruit.h"

class GameArea : public QWidget
{
    Q_OBJECT

private:
    static const int padding = 10;
    enum State {STOPPED, PLAYING, PAUSED, RESET} m_state;
    bool dir_lock;
    bool select = false;
    Snake *snake = nullptr;
    Direction snake_dir;
    QPoint snake_head;
    Fruit *fruit = nullptr;
    QVector<bool> draw_walls;
    QVector<QPoint> walls;

    int delay = 0;
    int timerId = 0;
    int steps = 0;

    void start_game();
    void stop_game();
    void pause_game();
    void init_game();
    void make_fruit();
    void set_direction(const Direction &dir);
    bool check_collision() const;
    QPoint convertAbsToGrid(int x, int y);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    explicit GameArea(QWidget *parent = nullptr);

public slots:
    void stateChanged();
    void saveGame();
    void loadGame();
    void startGame();
    void pauseGame();
    void resumeGame();
    void resetGame();

signals:
    void gamePaused();
    void gameResumed();
    void gameReset();
    void gameEnded();
    void updateTimer(int steps);
};

#endif // GAMEAREA_H
