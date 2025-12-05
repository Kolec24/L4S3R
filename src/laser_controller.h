#ifndef _LASER_CONTROLLER_H_
#define _LASER_CONTROLLER_H_

#include "sorg/sorg.h"
#include "board.h"

#include <vector>

struct Laser
{
    const unsigned            kId = 0;
    const std::pair<int, int> kSourceCell;
    const std::pair<int, int> kDirection;
    const sorg::Colour        kColour = {0.0f, 0.0f, 0.0f, 1.0f};

    std::vector<float> GetNormalisedColour() const
    {
        std::vector<float> colour = {kColour.r / 255.0f, kColour.g / 255.0f,
                                     kColour.b / 255.0f, kColour.a / 255.0f};

        return colour;
    }
};

class LaserController
{
  public:
    void Initialise(Board *board_ptr);
    void Update(float current_time);
    void Refresh();

    void OnBoardLogicChanged();

  private:
    void AddLaserSource(const unsigned id, const std::pair<int, int> cell,
                        const std::pair<int, int> direction,
                        const sorg::Colour       &colour);
    void RefreshLaserSources();

    void RefreshLasers();

    void GetCellCentre(int x, int y, float &horizontal, float &vertical) const;

  private:
    static constexpr unsigned index_offset        = 100;
    static constexpr bool     infiltrating_lasers = false;

    unsigned laser_source_shader_ = 0;
    unsigned laser_shader_        = 0;

    const float kLaserSourceDepth = 0.0f;
    const float kLaserDepth       = 0.0f;

    std::vector<Laser> lasers_;
    Board             *board_ptr_;
};

#endif
