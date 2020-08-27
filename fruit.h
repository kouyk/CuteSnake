#ifndef FRUIT_H
#define FRUIT_H

#include <QObject>
#include <QPoint>
#include <QRandomGenerator>
#include <QTextStream>

#include "parameters.h"

class Fruit : public QObject
{
    Q_OBJECT
public:
    explicit Fruit(QObject *parent = nullptr);
    void grow();
    QPoint getPos() const { return pos; };
    friend QTextStream& operator<<(QTextStream& out, const Fruit &f);
    friend QTextStream& operator>>(QTextStream& in, Fruit &f);

private:
    QRandomGenerator generator;
    QPoint pos;

signals:

};

#endif // FRUIT_H
