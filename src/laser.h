#ifndef _LASER_H_
#define _LASER_H_

#include "sorg/sorg.h"

enum class Direction
{
    kLeft = 0,
    kRight,
    kTop,
    kBottom
};

class Laser
{
  public:
    Laser() = default;
    Laser(const Direction &location, const int &index,
          const sorg::Colour &colour);

    Direction    GetLocation() const;
    int          GetIndex() const;
    sorg::Colour GetColour() const;

  private:
    const Direction    kLocation = Direction::kLeft;
    const int          kIndex    = 0;
    const sorg::Colour kColour   = {188.0f, 18.0f, 255.0f, 1.0f};
};

#endif
