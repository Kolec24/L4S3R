#ifndef _BOARD_H_
#define _BOARD_H_

#include "sorg/sorg.h"
#include <vector>

class Board
{
  public:
    Board() = default;
    Board(int width, int height,
          const std::vector<std::vector<int>> &board_state);

    void Initialise();
    void Reset();
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

    void DebugPrintState();

  private:
    void ResetBoardState();

  private:
    unsigned shader_ = 0;

    const int kWidth  = 0;
    const int kHeight = 0;

    const int kHorizontalHalfOffset = 1;
    const int kVerticalHalfOffset   = 2;

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
