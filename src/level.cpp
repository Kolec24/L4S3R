#include "level.h"

void Level::Initialise()
{
    board_.Initialise();
    InitialiseLasers();
}

void Level::Refresh()
{
    board_.Refresh();
    RefreshLaserSources();
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

void Level::InitialiseLasers()
{
    AddLaserSource(Direction::kLeft, 1, {255.0f, 0.0f, 0.0f, 1.0f});

    RefreshLaserSources();
}

void Level::AddLaserSource(const Direction &location, const int &index,
                           const sorg::Colour &colour)
{
    lasers_.push_back(Laser(location, index, colour));
}

void Level::RefreshLaserSources()
{
    std::vector<float>    laser_vertices;
    std::vector<unsigned> laser_indices;

    for (int i = 0; i < lasers_.size(); ++i)
    {
        Laser laser = lasers_[i];

        std::vector<std::pair<float, float>> positions2d;
        std::vector<std::pair<float, float>> uv = {
            {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}};

        auto laser_colour = laser.GetColour();

        std::vector<float> colour = {
            laser_colour.r / 255.0f, laser_colour.g / 255.0f,
            laser_colour.b / 255.0f, laser_colour.a / 255.0f};

        float top_left_x = 0.0f;
        float top_left_y = 0.0f;

        switch (laser.GetLocation())
        {
        case Direction::kLeft:
            top_left_x =
                board_.GetOriginRelativeX() - board_.GetHorizontalIncrement();
            top_left_y = board_.GetOriginRelativeY() -
                         laser.GetIndex() * board_.GetVerticalIncrement();
            break;
        case Direction::kRight:
            top_left_x = board_.GetOriginRelativeX() +
                         board_.GetWidth() * board_.GetHorizontalIncrement();
            top_left_y = board_.GetOriginRelativeY() -
                         laser.GetIndex() * board_.GetVerticalIncrement();
            break;
        case Direction::kTop:
            top_left_x = board_.GetOriginRelativeX() +
                         laser.GetIndex() * board_.GetHorizontalIncrement();
            top_left_y =
                board_.GetOriginRelativeY() + board_.GetVerticalIncrement();
            break;
        case Direction::kBottom:
            top_left_x = board_.GetOriginRelativeX() +
                         laser.GetIndex() * board_.GetHorizontalIncrement();
            top_left_y = board_.GetOriginRelativeY() -
                         board_.GetHeight() * board_.GetVerticalIncrement();
            break;
        default:
            break;
        }

        float top_right_x    = top_left_x + board_.GetHorizontalIncrement();
        float bottom_left_x  = top_left_x;
        float bottom_right_x = top_right_x;

        float top_right_y    = top_left_y;
        float bottom_left_y  = top_left_y - board_.GetVerticalIncrement();
        float bottom_right_y = bottom_left_y;

        positions2d.push_back({top_left_x, top_left_y});
        positions2d.push_back({top_right_x, top_right_y});
        positions2d.push_back({bottom_left_x, bottom_left_y});
        positions2d.push_back({bottom_right_x, bottom_right_y});

        for (int j = 0; j < positions2d.size(); ++j)
        {
            // 3d positions, uvs, colours.
            laser_vertices.push_back(positions2d[j].first);
            laser_vertices.push_back(positions2d[j].second);
            laser_vertices.push_back(0.0f);
            laser_vertices.push_back(uv[j].first);
            laser_vertices.push_back(uv[j].second);
            laser_vertices.insert(std::end(laser_vertices), std::begin(colour),
                                  std::end(colour));

            // Indices, typical quad indices.
            laser_indices.push_back(0 + i * positions2d.size());
            laser_indices.push_back(1 + i * positions2d.size());
            laser_indices.push_back(2 + i * positions2d.size());
            laser_indices.push_back(1 + i * positions2d.size());
            laser_indices.push_back(3 + i * positions2d.size());
            laser_indices.push_back(2 + i * positions2d.size());
        }
    }

    sorg::SetupTriangles(laser_vertices, laser_indices, laser_indices.size());
}
