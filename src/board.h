#ifndef _BOARD_H_
#define _BOARD_H_

#include "sorg/sorg.h"
#include <vector>

class Board
{
  public:
    void Initialise();
    void SetViewportDimensions(float viewport_width, float viewport_height);
    void Refresh();

    std::vector<float> GetBoardNormalisedColour() const;
    std::vector<float> GetMirrorNormalisedColour() const;

    int   GetWidth() const;
    int   GetHeight() const;
    float GetHorizontalIncrement() const;
    float GetVerticalIncrement() const;
    float GetOriginRelativeX() const;
    float GetOriginRelativeY() const;

    void SetBoardState(unsigned x, unsigned y, int value);
    int  GetBoardState(unsigned x, unsigned y) const;
    void RemoveLasersFromBoard();

    bool AddOrChangeMirror(float x, float y);
    bool RemoveMirror(float x, float y);

  private:
    unsigned    shader_ = 0;
    const float kDepth  = -0.5f;

    const int kWidth  = 5;
    const int kHeight = 5;

    const int kHorizontalHalfOffset = 1;
    const int kVerticalHalfOffset   = 2;

    float viewport_width_  = 0.0f;
    float viewport_height_ = 0.0f;

    std::vector<std::vector<int>> board_state_;
    std::pair<float, float>       board_origin_;
    std::pair<float, float>       board_origin_relative_;
    float                         cell_size_            = 0.0f;
    float                         horizontal_increment_ = 0.0f;
    float                         vertical_increment_   = 0.0f;

    sorg::Colour board_colour_  = {50.0f, 50.0f, 50.0f, 255.0f};
    sorg::Colour mirror_colour_ = {255.0f, 255.0f, 255.0f, 255.0f};
};
#endif
