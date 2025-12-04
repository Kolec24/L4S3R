#include "level.h"

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
    board_.AddOrChangeMirror(x, y);
}

void Level::OnRightClicked(float x, float y)
{
    board_.RemoveMirror(x, y);
}
