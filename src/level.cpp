#include "level.h"

#include <iostream>

Level::Level(LevelData data)
    : board_(data.board_width_, data.board_height_, data.board_state_),
      laser_controller_(data.lasers_)
{
}

void Level::Initialise()
{
    board_.Initialise();
    laser_controller_.Initialise(&board_);
}

void Level::Update(float current_time)
{
    laser_controller_.Update(current_time);
}

void Level::Refresh()
{
    board_.Refresh();
    laser_controller_.Refresh();
}

void Level::SetViewportDimensions(float viewport_width, float viewport_height,
                                  bool refresh)
{
    board_.SetViewportDimensions(viewport_width, viewport_height);

    if (refresh)
    {
        Refresh();
    }
}

void Level::OnLeftClicked(float x, float y)
{
    if (finished_)
    {
        return;
    }

    bool board_changed = board_.AddOrChangeMirror(x, y);
    if (board_changed)
    {
        laser_controller_.OnBoardLogicChanged();
        CheckFinishCondition();
    }
}

void Level::OnRightClicked(float x, float y)
{
    if (finished_)
    {
        return;
    }

    bool board_changed = board_.RemoveMirror(x, y);
    if (board_changed)
    {
        laser_controller_.OnBoardLogicChanged();
        CheckFinishCondition();
    }
}

void Level::CheckFinishCondition()
{
    if (laser_controller_.AreAllTargetsHit())
    {
        OnLevelFinished();
    }
}

void Level::OnLevelFinished()
{
    finished_ = true;
    std::cout << "WIN!" << std::endl;
}
