#ifndef _LEVEL_H_
#define _LEVEL_H_

#include "board.h"
#include "laser_controller.h"

struct LevelData
{
    int board_width_  = 0;
    int board_height_ = 0;

    std::vector<std::vector<int>> board_state_;
    std::vector<LaserData>        lasers_;
};

class Level
{
  public:
    Level() = default;
    Level(LevelData data);

    void Initialise();
    void Update(float current_time);
    void Reset();
    void Refresh();

    bool IsFinished() const;

    void OnLeftClicked(float x, float y);
    void OnRightClicked(float x, float y);

  private:
    void CheckFinishCondition();
    void OnLevelFinished();

  private:
    bool            finished_ = false;
    Board           board_;
    LaserController laser_controller_;
};

#endif
