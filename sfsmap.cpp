#include "sfsmap.h"

SfsMap::SfsMap()
{
    layers = new QVector<SfsLayer*>;
    bbox = new BoundaryBox();
}
