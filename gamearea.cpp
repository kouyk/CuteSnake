#include "gamearea.h"

#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QtMath>

GameArea::GameArea(QWidget *parent)
    : QWidget(parent)
    , m_state(STOPPED)
    , draw_walls(grid_width * grid_height)
{
    setMinimumSize(420, 420);
    fruit = new Fruit(this);
    walls.reserve(grid_width * grid_height);
    init_game();
}

void GameArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);

    // determine the size of each square
    int w_size = (width() - padding * 2) / grid_width;
    int h_size = (height() - padding * 2) / grid_height;
    int &block_size = (w_size > h_size) ? h_size : w_size;

    auto drawSqr = [&](const QPoint &qp) { p.drawRect(qp.x(), qp.y(), 1, 1); };
    auto drawCir = [&](const QPoint &qp) { p.drawEllipse(qp.x(), qp.y(), 1, 1);};

    p.translate((width() - block_size * grid_width) / 2,
                (height() - block_size * grid_height) / 2);

    // draw boundary
    p.setPen(QPen(Qt::white, 2));
    p.setBrush(Qt::NoBrush);
    p.drawRect(-1, -1, grid_width * block_size + 3, grid_height * block_size + 3);

    p.scale(block_size, block_size);
    p.setPen(QPen(Qt::white, 1 / block_size));

    // draw snake
    p.setBrush(Qt::green);

    std::for_each(snake->getBody().begin(), snake->getBody().end(), drawSqr);

    p.setBrush(Qt::darkGreen);
    drawSqr(snake_head);

    // draw walls
    p.setBrush(Qt::blue);
    if (m_state == RESET) {
        for (int i = 0; i < draw_walls.size(); ++i) {
            if (!draw_walls[i]) continue;

            auto y = i / grid_width;
            auto x = i % grid_width;
            drawSqr(QPoint(x, y));
        }
    } else {
        std::for_each(walls.begin(), walls.end(), drawSqr);
    }

    // draw fruit
    p.setBrush(Qt::red);
    if (m_state != RESET) {
        drawCir(fruit->getPos());
    }
}

void GameArea::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    switch (snake_dir) {
    case LEFT:
        --snake_head.rx();
        break;
    case RIGHT:
        ++snake_head.rx();
        break;
    case UP:
        --snake_head.ry();
        break;
    case DOWN:
        ++snake_head.ry();
        break;
    }

    bool ate_fruit = snake_head == fruit->getPos();

    if (ate_fruit) {
        snake->move(snake_head, true);
        make_fruit();
    } else {
        snake->move(snake_head);
    }
    ++steps;

    if (check_collision()) {
        stop_game();
        --steps;
    }

    auto prev_delay = delay;
    delay = qExp(5-0.002*steps) + minimum_delay;
    if (delay < prev_delay) {
        killTimer(timerId);
        startTimer(delay);
    }

    dir_lock = false;
    emit updateTimer(steps);
    update();
}

void GameArea::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        set_direction(UP);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        set_direction(DOWN);
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        set_direction(LEFT);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        set_direction(RIGHT);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void GameArea::mousePressEvent(QMouseEvent *event)
{
    if (m_state != RESET) return;

    auto p = convertAbsToGrid(event->x(), event->y());
    auto idx = p.y() * grid_width + p.x();

    const auto &s_body = snake->getBody();
    if (std::none_of(s_body.begin(), s_body.end(),
                     [&](const QPoint &qp){ return p == qp; })) {
        select = draw_walls[idx] = !draw_walls[idx];
        update();
    }
}

void GameArea::mouseMoveEvent(QMouseEvent *event)
{
    if (m_state != RESET) return;
    auto p = convertAbsToGrid(event->x(), event->y());
    auto idx = p.y() * grid_width + p.x();

    const auto &s_body = snake->getBody();
    if (std::none_of(s_body.begin(), s_body.end(),
                     [&](const QPoint &qp){ return p == qp; })) {
        if (draw_walls[idx] != select) {
            draw_walls[idx] = select;
            update();
        }
    }
}

void GameArea::init_game()
{
    snake_head.setX(start_x);
    snake_head.setY(start_y);

    delete snake;
    QVector<QPoint> body_start;
    body_start.reserve(grid_width * grid_height);
    body_start.append(snake_head);
    body_start.append(snake_head + QPoint(1, 0));

    snake = new Snake(std::move(body_start), this);
    snake_dir = LEFT;
    walls.clear();
    std::fill_n(draw_walls.begin(), grid_width*grid_height, false);
    delay = qExp(5-0.002*steps) + minimum_delay;
    m_state = RESET;
    steps = 0;
    emit updateTimer(steps);
    repaint();

    emit gameReset();
}

void GameArea::start_game()
{
    if (m_state == RESET) {
        dir_lock = false;
        for (int i = 0; i < draw_walls.size(); ++i) {
            if (!draw_walls[i]) continue;

            auto y = i / grid_width;
            auto x = i % grid_width;
            walls.append(QPoint(x, y));
        }
        make_fruit();
    }
    m_state = PLAYING;
    timerId = (steps > 0) ? startTimer(delay) : -1;
    setFocus();
    update();
    emit gameResumed();
}

void GameArea::stop_game()
{
    killTimer(timerId);
    m_state = STOPPED;
    snake->backtrack();
    snake_head = snake->getBody().front();
    QMessageBox::information(this, tr("Oops"), tr("Game Over!"));
    emit gameEnded();
}

void GameArea::pause_game()
{
    killTimer(timerId);
    m_state = PAUSED;
    emit gamePaused();
}

void GameArea::make_fruit()
{
    const auto &snake_body = snake->getBody();
    QPoint fruit_pos;
    do {
        fruit->grow();
        fruit_pos = fruit->getPos();
    } while (std::any_of(snake_body.begin(),
                         snake_body.end(),
                         [&](const QPoint &qp){ return qp == fruit_pos; })
             || std::any_of(walls.begin(),
                            walls.end(),
                            [&](const QPoint &qp){ return qp == fruit_pos; }));
}

bool GameArea::check_collision() const
{
    const auto &snake_body = snake->getBody();

    return snake_head.x() < 0
            || snake_head.x() >= grid_width
            || snake_head.y() < 0
            || snake_head.y() >= grid_height
            || std::any_of(snake_body.begin()+1,
                           snake_body.end(),
                           [&](const QPoint &qp){ return snake_head == qp; })
            || std::any_of(walls.begin(),
                           walls.end(),
                           [&](const QPoint &qp){ return snake_head == qp; });
}

void GameArea::set_direction(const Direction &dir)
{
    if (dir_lock || m_state != PLAYING) return;

    if (snake_dir != dir && snake_dir != (DOWN - dir)) {
        snake_dir = dir;
        dir_lock = true;
    }

    if (timerId == -1) {
        timerId = startTimer(delay);
    }
}

void GameArea::stateChanged()
{
    switch (m_state) {
    case PLAYING:
        pause_game();
        break;
    case RESET:
    case PAUSED:
        start_game();
        break;
    case STOPPED:
        init_game();
    }
}

QPoint GameArea::convertAbsToGrid(int x, int y)
{
    // determine the size of each square
    int w_size = (width() - padding * 2) / grid_width;
    int h_size = (height() - padding * 2) / grid_height;
    int &block_size = (w_size > h_size) ? h_size : w_size;

    int min_x = (width() - block_size * grid_width) / 2;
    int min_y = (height() - block_size * grid_height) / 2;
    int max_x = min_x + block_size * grid_width;
    int max_y = min_y + block_size * grid_height;

    if (x < min_x || x >= max_x || y < min_y || y >= max_y) {
        return QPoint(start_x, start_y);
    }

    auto grid_x = (x - min_x) / block_size;
    auto grid_y = (y - min_y) / block_size;

    return QPoint(grid_x, grid_y);
}

void GameArea::saveGame()
{
    QFileDialog fileDialog(this, tr("Save Game State"));
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setNameFilter(tr("Game Save (*.gs);;All Files (*)"));
    fileDialog.setDefaultSuffix("gs");
    fileDialog.setDirectory(QDir::homePath());

    QString filename;
    if (fileDialog.exec()) {
        filename = fileDialog.selectedFiles()[0];
    }

    if (filename.isEmpty()) {
        return;
    }
    else {
        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream out(&file);
        out << *snake << snake_dir << ' ' << dir_lock << '\n' << *fruit << '\n';
        out << walls.size() << '\n';
        for (const auto &p : walls) {
            out << p.x() << ' ' << p.y() << '\n';
        }
        out << steps << '\n';
        file.flush();
        file.close();
    }
    m_state = PAUSED;
}

void GameArea::loadGame()
{
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Load Game State"),
                QDir::homePath(),
                tr("Game Save (*.gs);;All Files (*)"));

    if (filename.isEmpty()) {
        return;
    }
    else {
        QFile file(filename);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream in(&file);
        int t_dir, d_lock;
        in >> *snake  >> t_dir >> d_lock >> *fruit;
        snake_dir = static_cast<Direction>(t_dir);
        dir_lock = static_cast<bool>(d_lock);
        snake_head = snake->getBody().front();

        walls.clear();
        int num;
        in >> num;
        for (int i = 0; i < num; ++i) {
            int x, y;
            in >> x >> y;
            walls.append(QPoint(x, y));
        }
        in >> steps;
        emit updateTimer(steps);
        file.close();
    }
    m_state = PAUSED;
    update();
    emit gamePaused();
}

void GameArea::startGame()
{
    if (m_state == RESET) {
        start_game();
    }
}

void GameArea::pauseGame()
{
    if (m_state == PLAYING) {
        pause_game();
    }
}

void GameArea::resumeGame()
{
    if (m_state == PAUSED) {
        start_game();
    }
}

void GameArea::resetGame()
{
    if (m_state == PLAYING) {
        killTimer(timerId);
    }
    m_state = STOPPED;
    init_game();
}
