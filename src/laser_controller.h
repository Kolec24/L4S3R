#ifndef _LASER_H_
#define _LASER_H_

#include "sorg/sorg.h"
#include "board.h"

#include <vector>

enum class Direction
{
    kLeft = 0,
    kRight,
    kTop,
    kBottom
};

struct Laser
{
    const Direction    kLocation = Direction::kLeft;
    const int          kIndex    = 0;
    const sorg::Colour kColour   = {188.0f, 18.0f, 255.0f, 1.0f};
};

class LaserController
{
  public:
    void Initialise(Board *pBoard);
    void Update(float current_time);
    void Refresh();

  private:
    void AddLaserSource(const Direction &location, const int &index,
                        const sorg::Colour &colour);
    void RefreshLaserSources();

  private:
    unsigned    laser_source_shader_ = 0;
    const float kSourceDepth         = 0.0f;

    std::vector<Laser> lasers_;
    Board             *pBoard_;
};

#endif
