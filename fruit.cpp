#include "fruit.h"

Fruit::Fruit(QObject *parent)
    : QObject(parent)
    , generator(QRandomGenerator::system()->generate())
    , pos(0, 0)
{

}

void Fruit::grow()
{
    pos.setX(generator.bounded(grid_width));
    pos.setY(generator.bounded(grid_height));
}

QTextStream& operator<<(QTextStream& out, const Fruit &f)
{
    out << f.pos.x() << " " << f.pos.y() << '\n';
    return out;
}

QTextStream& operator>>(QTextStream& in, Fruit &f)
{
    in >> f.pos.rx() >> f.pos.ry();
    return in;
}
