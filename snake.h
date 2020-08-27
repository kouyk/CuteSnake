#ifndef SNAKE_H
#define SNAKE_H

#include <QObject>
#include <QVector>
#include <QPoint>
#include <QTextStream>

enum Direction {UP, LEFT, RIGHT, DOWN};

class Snake : public QObject
{
    Q_OBJECT
public:
    Snake(QVector<QPoint>&& body, QObject *parent = nullptr);
    const QVector<QPoint>& getBody() const { return body; };
    void move(const QPoint &qp, bool food = false);
    void backtrack(); // undo last move
    friend QTextStream& operator<<(QTextStream& out, const Snake &s);
    friend QTextStream& operator>>(QTextStream& in, Snake &s);

private:
    int growth;
    QVector<QPoint> body;
    QPoint prev_tail;
signals:

};

#endif // SNAKE_H
