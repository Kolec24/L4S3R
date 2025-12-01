#include "laser.h"

Laser::Laser(const Direction &location, const int &index,
             const sorg::Colour &colour)
    : kLocation(location), kIndex(index), kColour(colour)
{
}

Direction Laser::GetLocation() const
{
    return kLocation;
}

int Laser::GetIndex() const
{
    return kIndex;
}

sorg::Colour Laser::GetColour() const
{
    return kColour;
}
