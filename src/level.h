#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "board.h"
#include "laser.h"

#include <vector>

class Level
{
  public:
    void Initialise();
    void Update(float current_time);
    void Refresh();
    void SetViewportDimensions(float viewport_width, float vieport_height,
                               bool refresh);

    void OnLeftClicked(float x, float y);
    void OnRightClicked(float x, float y);

  private:
    void InitialiseLasers();
    void AddLaserSource(const Direction &location, const int &index,
                        const sorg::Colour &colour);
    void RefreshLaserSources();

  private:
    Board              board_;
    std::vector<Laser> lasers_;

    unsigned laser_source_shader_ = 0;
};

#endif
