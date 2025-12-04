#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "board.h"
#include "laser_controller.h"

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
    Board           board_;
    LaserController laser_controller_;
};

#endif
