#include "snake.h"

#include <Qt>

Snake::Snake(QVector<QPoint>&& body, QObject *parent)
    : QObject(parent)
    , growth(0)
    , body(body)
{

}

void Snake::move(const QPoint &qp, bool food) {
    prev_tail = body.back();

    if (food) growth += 3;

    if (growth > 0) {
        body.append(prev_tail);
        std::move_backward(body.begin(), body.end()-2, body.end()-1);
        --growth;
    } else {
        std::move_backward(body.begin(), body.end()-1, body.end());
    }

    body.front() = qp;
}

void Snake::backtrack()
{
    std::move(body.begin()+1, body.end(), body.begin());
    body.back() = prev_tail;
}

QTextStream& operator<<(QTextStream& out, const Snake& s)
{
    out << s.body.size() << ' ' << s.growth << '\n';
    for (const auto &p : s.body) {
        out << p.x() << ' ' << p.y() << '\n';
    }
    return out;
}

QTextStream& operator>>(QTextStream& in, Snake &s)
{
    s.body.clear();
    int length = 0;
    in >> length >> s.growth;
    for (int i = 0; i < length; ++i) {
        int x, y;
        in >> x >> y;
        s.body.append(QPoint(x, y));
    }

    return in;
}
