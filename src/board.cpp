#include "board.h"
#include "sorg/sorg.h"
#include "game.h"

#include <iostream>

Board::Board(int width, int height,
             const std::vector<std::vector<int>> &board_state)
    : kWidth(width), kHeight(height), board_state_(board_state)
{
}

void Board::Initialise()
{
    shader_ = sorg::SetupShaderProgram("shaders/board.vs", "shaders/board.fs");
    Refresh();
}

void Board::Reset()
{
    sorg::ResetShaderProgram(shader_);
    ResetBoardState();
}

void Board::Refresh()
{
    RemoveLasersFromBoard();

    auto board_colour  = GetBoardNormalisedColour();
    auto mirror_colour = GetMirrorNormalisedColour();

    // Level parameters.
    const auto &viewport_width  = Game::GetViewportWidth();
    const auto &viewport_height = Game::GetViewportHeight();

    const auto &top_offset    = Game::GetRelativeTopOffset();
    const auto &bottom_offset = Game::GetRelativeBottomOffset();
    const auto &left_offset   = Game::GetRelativeLeftOffset();
    const auto &right_offset  = Game::GetRelativeRightOffset();

    const float level_width =
        (1.0f - left_offset - right_offset) * viewport_width;
    const float level_height =
        (1.0f - top_offset - bottom_offset) * viewport_height;

    // Board frame.
    float board_middle_x = (right_offset - left_offset);
    float board_middle_y = (bottom_offset - top_offset);

    float width_cell_size  = level_width / (kWidth + 2);
    float height_cell_size = level_height / (kHeight + 2);

    cell_size_ = std::min(width_cell_size, height_cell_size);

    horizontal_increment_ = (2.0f * cell_size_) / viewport_width;
    vertical_increment_   = -1 * (2.0f * cell_size_) / viewport_height;

    float board_origin_relative_x =
        board_middle_x - horizontal_increment_ * (kWidth / 2.0f);
    float board_origin_relative_y =
        board_middle_y - vertical_increment_ * (kHeight / 2.0f);

    board_origin_ = {viewport_width / 2.0f * (1 + board_origin_relative_x),
                     viewport_height / 2.0f * (1 - board_origin_relative_y)};
    board_origin_relative_ = {board_origin_relative_x, board_origin_relative_y};

    std::vector<float>    vertices;
    std::vector<unsigned> indices;

    // Board lines.
    for (int i = 0; i < kWidth + 1; ++i)
    {
        vertices.push_back(GetOriginRelativeX() + i * GetHorizontalIncrement());
        vertices.push_back(GetOriginRelativeY());
        vertices.push_back(kBoardDepth);
        vertices.insert(std::end(vertices), std::begin(board_colour),
                        std::end(board_colour));

        vertices.push_back(GetOriginRelativeX() + i * GetHorizontalIncrement());
        vertices.push_back(GetOriginRelativeY() +
                           kHeight * GetVerticalIncrement());
        vertices.push_back(kBoardDepth);
        vertices.insert(std::end(vertices), std::begin(board_colour),
                        std::end(board_colour));

        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
    }

    indices.push_back(0);
    indices.push_back(2 * kWidth);

    indices.push_back(1);
    indices.push_back(2 * kWidth + 1);

    for (int i = 1; i < kHeight; ++i)
    {
        vertices.push_back(GetOriginRelativeX());
        vertices.push_back(GetOriginRelativeY() + i * GetVerticalIncrement());
        vertices.push_back(kBoardDepth);
        vertices.insert(std::end(vertices), std::begin(board_colour),
                        std::end(board_colour));

        vertices.push_back(GetOriginRelativeX() +
                           kWidth * GetHorizontalIncrement());
        vertices.push_back(GetOriginRelativeY() + i * GetVerticalIncrement());
        vertices.push_back(kBoardDepth);
        vertices.insert(std::end(vertices), std::begin(board_colour),
                        std::end(board_colour));

        indices.push_back(2 * (kWidth + i));
        indices.push_back(2 * (kWidth + i) + 1);
    }

    // For interiors, we draw all vertices, since we will utilise them anyway.
    for (int i = 0; i < kHeight + 1; ++i)
    {
        for (int j = 0; j < kWidth + 1; ++j)
        {
            vertices.push_back(GetOriginRelativeX() +
                               j * GetHorizontalIncrement());
            vertices.push_back(GetOriginRelativeY() +
                               i * GetVerticalIncrement());
            vertices.push_back(kInteriorDepth);
            vertices.insert(std::end(vertices), std::begin(mirror_colour),
                            std::end(mirror_colour));
        }
    }

    unsigned board_vertices_count = 2 * (kWidth + kHeight);
    for (int i = 0; i < kWidth; ++i)
    {
        for (int j = 0; j < kHeight; ++j)
        {
            // Mirrors.
            if (GetBoardState(i, j) == 2)
            {
                indices.push_back(board_vertices_count + i + j * (kWidth + 1));
                indices.push_back(board_vertices_count + i +
                                  (j + 1) * (kWidth + 1) + 1);
            }
            else if (GetBoardState(i, j) == 1)
            {
                indices.push_back(board_vertices_count + i + j * (kWidth + 1) +
                                  1);
                indices.push_back(board_vertices_count + i +
                                  (j + 1) * (kWidth + 1));
            }
            // Blockers.
            else if (GetBoardState(i, j) == -1)
            {
                indices.push_back(board_vertices_count + i + j * (kWidth + 1));
                indices.push_back(board_vertices_count + i +
                                  (j + 1) * (kWidth + 1) + 1);
                indices.push_back(board_vertices_count + i + j * (kWidth + 1) +
                                  1);
                indices.push_back(board_vertices_count + i +
                                  (j + 1) * (kWidth + 1));
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
    board_state_[y][x] = value;
}

int Board::GetBoardState(unsigned x, unsigned y) const
{
    return board_state_[y][x];
}

void Board::RemoveLasersFromBoard()
{
    for (auto &line : board_state_)
    {
        for (auto &state : line)
        {
            if (state > 2)
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

    switch (GetBoardState(horizontal_vertex, vertical_vertex))
    {
    // Blockers.
    case -1:
        break;
    // Available spots.
    case 2:
        SetBoardState(horizontal_vertex, vertical_vertex, 1);
        break;
    case 0:
    case 1:
    default:
        SetBoardState(horizontal_vertex, vertical_vertex, 2);
        break;
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

    switch (GetBoardState(horizontal_vertex, vertical_vertex))
    {
    // Blockers.
    case -1:
        break;
    // Available spots.
    default:
        SetBoardState(horizontal_vertex, vertical_vertex, 0);
        break;
    }

    Refresh();
    return true;
}

void Board::DebugPrintState()
{
    for (const auto &line : board_state_)
    {
        for (const auto &state : line)
        {
            std::cout << state;
        }

        std::cout << std::endl;
    }
}

void Board::ResetBoardState()
{
    for (auto &line : board_state_)
    {
        for (auto &cell : line)
        {
            if (cell > 0)
            {
                cell = 0;
            }
        }
    }
}
