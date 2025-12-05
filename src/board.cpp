#include "board.h"
#include "sorg/sorg.h"

#include <iostream>

void Board::Initialise()
{
    shader_ = sorg::SetupShaderProgram("shaders/board.vs", "shaders/board.fs");
    for (int i = 0; i < kWidth; ++i)
    {
        std::vector<int> row;
        for (int j = 0; j < kHeight; ++j)
        {
            row.push_back(0);
        }

        board_state_.push_back(row);
    }

    Refresh();
}

void Board::SetViewportDimensions(float viewport_width, float viewport_height)
{
    viewport_width_  = viewport_width;
    viewport_height_ = viewport_height;
}

void Board::Refresh()
{
    RemoveLasersFromBoard();

    auto board_colour  = GetBoardNormalisedColour();
    auto mirror_colour = GetMirrorNormalisedColour();

    // Board frame.
    float width_spacing = static_cast<float>(viewport_width_) /
                          (kWidth + 2 * kHorizontalHalfOffset);
    float height_spacing = static_cast<float>(viewport_height_) /
                           (kHeight + 2 * kVerticalHalfOffset);

    cell_size_ = std::min(width_spacing, height_spacing);

    float horizontal_spacing = viewport_width_ / (2 * cell_size_);
    float vertical_spacing   = viewport_height_ / (2 * cell_size_);

    float horizontal_skips = horizontal_spacing - 0.5f * kWidth;
    float vertical_skips   = vertical_spacing - 0.5f * kHeight;

    horizontal_increment_ = 1 / horizontal_spacing;
    vertical_increment_   = 1 / vertical_spacing;

    float initial_vertical_measurement =
        -1 + vertical_skips * vertical_increment_;
    float initial_horizontal_measurement =
        -1 + horizontal_skips * horizontal_increment_;

    board_origin_ = {
        viewport_width_ / 2.0f * (1 + initial_horizontal_measurement),
        viewport_height_ / 2.0f * (1 + initial_vertical_measurement)};

    board_origin_relative_ = {initial_horizontal_measurement,
                              -initial_vertical_measurement};

    std::vector<float> vertices;
    float              vertical_measurement = initial_vertical_measurement;
    for (int i = 0; i < kHeight + 1; ++i)
    {
        float horizontal_measurement = initial_horizontal_measurement;
        for (int j = 0; j < kWidth + 1; ++j)
        {
            vertices.push_back(horizontal_measurement);
            vertices.push_back(-1 * vertical_measurement);
            vertices.push_back(kDepth);
            vertices.insert(std::end(vertices), std::begin(board_colour),
                            std::end(board_colour));
            horizontal_measurement += 1 / horizontal_spacing;
        }

        vertical_measurement += 1 / vertical_spacing;
    }

    std::vector<unsigned> indices;
    for (auto i = 0; i < kHeight + 1; ++i)
    {
        auto start = i * (kWidth + 1);
        indices.push_back(start);
        indices.push_back(start + kWidth);
    }

    for (auto i = 0; i < kWidth + 1; ++i)
    {
        auto start = i;
        indices.push_back(start);
        indices.push_back(start + kHeight * (kWidth + 1));
    }

    // Mirrors.
    unsigned current_index = (kHeight + 1) * (kWidth + 1);
    for (int i = 0; i < kWidth; ++i)
    {
        for (int j = 0; j < kHeight; ++j)
        {
            if (board_state_[i][j] == 1)
            {
                float mirror_top_x =
                    GetOriginRelativeX() + i * GetHorizontalIncrement();
                float mirror_top_y =
                    GetOriginRelativeY() - j * GetVerticalIncrement();
                float mirror_bottom_x =
                    GetOriginRelativeX() + (i + 1) * GetHorizontalIncrement();
                float mirror_bottom_y =
                    GetOriginRelativeY() - (j + 1) * GetVerticalIncrement();
                vertices.push_back(mirror_top_x);
                vertices.push_back(mirror_top_y);
                vertices.push_back(kDepth);
                vertices.insert(std::end(vertices), std::begin(mirror_colour),
                                std::end(mirror_colour));

                vertices.push_back(mirror_bottom_x);
                vertices.push_back(mirror_bottom_y);
                vertices.push_back(kDepth);
                vertices.insert(std::end(vertices), std::begin(mirror_colour),
                                std::end(mirror_colour));

                indices.push_back(current_index++);
                indices.push_back(current_index++);
            }
            else if (board_state_[i][j] == -1)
            {
                float mirror_top_x =
                    GetOriginRelativeX() + (i + 1) * GetHorizontalIncrement();
                float mirror_top_y =
                    GetOriginRelativeY() - j * GetVerticalIncrement();
                float mirror_bottom_x =
                    GetOriginRelativeX() + i * GetHorizontalIncrement();
                float mirror_bottom_y =
                    GetOriginRelativeY() - (j + 1) * GetVerticalIncrement();

                vertices.push_back(mirror_top_x);
                vertices.push_back(mirror_top_y);
                vertices.push_back(kDepth);
                vertices.insert(std::end(vertices), std::begin(mirror_colour),
                                std::end(mirror_colour));

                vertices.push_back(mirror_bottom_x);
                vertices.push_back(mirror_bottom_y);
                vertices.push_back(kDepth);
                vertices.insert(std::end(vertices), std::begin(mirror_colour),
                                std::end(mirror_colour));

                indices.push_back(current_index++);
                indices.push_back(current_index++);
            }
        }
    }

    sorg::SetupLines(shader_, vertices, indices, indices.size());
}

std::vector<float> Board::GetBoardNormalisedColour() const
{
    std::vector<float> colour = {
        board_colour_.r / 255.0f, board_colour_.g / 255.0f,
        board_colour_.b / 255.0f, board_colour_.a / 255.0f};
    return colour;
}

std::vector<float> Board::GetMirrorNormalisedColour() const
{
    std::vector<float> colour = {
        mirror_colour_.r / 255.0f, mirror_colour_.g / 255.0f,
        mirror_colour_.b / 255.0f, mirror_colour_.a / 255.0f};
    return colour;
}

int Board::GetWidth() const
{
    return kWidth;
}

int Board::GetHeight() const
{
    return kHeight;
}

float Board::GetHorizontalIncrement() const
{
    return horizontal_increment_;
}

float Board::GetVerticalIncrement() const
{
    return vertical_increment_;
}

float Board::GetOriginRelativeX() const
{
    return board_origin_relative_.first;
}

float Board::GetOriginRelativeY() const
{
    return board_origin_relative_.second;
}

void Board::SetBoardState(unsigned x, unsigned y, int value)
{
    board_state_[x][y] = value;
}

int Board::GetBoardState(unsigned x, unsigned y) const
{
    return board_state_[x][y];
}

void Board::RemoveLasersFromBoard()
{
    for (auto &line : board_state_)
    {
        for (auto &state : line)
        {
            if (state != -1 && state != 0 && state != 1)
            {
                state = 0;
            }
        }
    }
}

bool Board::AddOrChangeMirror(float x, float y)
{
    if (cell_size_ == 0)
    {
        std::cout << "[L4S3R] Board: OnClicked: cell_size is 0." << std::endl;
        return false;
    }

    if (x < board_origin_.first || y < board_origin_.second ||
        x > board_origin_.first + kWidth * cell_size_ ||
        y > board_origin_.second + kHeight * cell_size_)
    {
        return false;
    }

    unsigned horizontal_vertex = (x - board_origin_.first) / cell_size_;
    unsigned vertical_vertex   = (y - board_origin_.second) / cell_size_;

    switch (board_state_[horizontal_vertex][vertical_vertex])
    {
    case 1:
        board_state_[horizontal_vertex][vertical_vertex] = -1;
        break;

    case 0:
    case -1:
    default:
        board_state_[horizontal_vertex][vertical_vertex] = 1;
        break;

        // std::cout << "[L4S3R] Board: OnClicked: Invalid board state!"
        //           << std::endl;
        // break;
    }

    Refresh();
    return true;
}

bool Board::RemoveMirror(float x, float y)
{
    RemoveLasersFromBoard();

    if (cell_size_ == 0)
    {
        std::cout << "[L4S3R] Board: OnClicked: cell_size is 0." << std::endl;
        return false;
    }

    if (x < board_origin_.first || y < board_origin_.second ||
        x > board_origin_.first + kWidth * cell_size_ ||
        y > board_origin_.second + kHeight * cell_size_)
    {
        return false;
    }

    unsigned horizontal_vertex = (x - board_origin_.first) / cell_size_;
    unsigned vertical_vertex   = (y - board_origin_.second) / cell_size_;

    board_state_[horizontal_vertex][vertical_vertex] = 0;

    Refresh();
    return true;
}
