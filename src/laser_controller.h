#ifndef _LASER_CONTROLLER_H_
#define _LASER_CONTROLLER_H_

#include "sorg/sorg.h"
#include "board.h"

#include <vector>

struct LaserData
{
    std::pair<int, int> source_cell_;
    std::pair<int, int> target_cell_;
    std::pair<int, int> direction_;
    sorg::Colour        colour_;
};

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

struct Target
{
    const unsigned            kId = 0;
    const std::pair<int, int> kCell;
    const sorg::Colour        kColour = {0.0f, 0.0f, 0.0f, 1.0f};

    bool hit_ = false;

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
    LaserController() = default;
    LaserController(const std::vector<LaserData> lasers);

    void Initialise(Board *board_ptr);
    void Update(float current_time);
    void Reset();
    void Refresh();

    void OnBoardLogicChanged();
    bool AreAllTargetsHit() const;

  private:
    void AddLaserWithTarget(const unsigned            id,
                            const std::pair<int, int> source_cell,
                            const std::pair<int, int> target_cell,
                            const std::pair<int, int> direction,
                            const sorg::Colour       &colour);
    void RefreshLaserSources();
    void RefreshTargets();
    void RefreshLasers();
    void ResetTargets();

    void OnTargetHit(Target &target, bool hit);

    void GetCellCentre(int x, int y, float &horizontal, float &vertical) const;

  private:
    static constexpr unsigned index_offset        = 100;
    static constexpr bool     infiltrating_lasers = false;

    unsigned laser_source_shader_ = 0;
    unsigned laser_shader_        = 0;
    unsigned target_hit_shader_   = 0;
    unsigned target_nohit_shader_ = 0;

    const float kLaserSourceDepth = 0.0f;
    const float kTargetDepth      = 0.0f;
    const float kLaserDepth       = -0.1f;

    std::vector<Laser>  lasers_;
    std::vector<Target> targets_;
    Board              *board_ptr_;
};

#endif
