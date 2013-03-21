#ifndef CONVERSEREC_H
#define CONVERSEREC_H
#include <QtGlobal>

enum UnboxingPolicy
{
    Dub,
    Average
};

struct ConverseRec
{
    static const ConverseRec BT601;
    static const ConverseRec BT709;

    const qreal Kb;
    const qreal Kr;
private:
    ConverseRec(qreal kb, qreal kr);
};

#endif // CONVERSEREC_H
