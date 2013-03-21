#include "converserec.h"

const ConverseRec ConverseRec::BT601(0.114, 0.299);
const ConverseRec ConverseRec::BT709(0.0722, 0.2126);

ConverseRec::ConverseRec(qreal kb, qreal kr) :
    Kb(kb),
    Kr(kr)
{
}
