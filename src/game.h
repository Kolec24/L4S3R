#ifndef _GAME_H_
#define _GAME_H_

#include <string>
#include <vector>

#include "level.h"

class Game
{
  public:
    Game() = default;

    static unsigned GetViewportWidth();
    static unsigned GetViewportHeight();

    static float GetCurrentTime();

    static float GetRelativeTopOffset();
    static float GetRelativeBottomOffset();
    static float GetRelativeLeftOffset();
    static float GetRelativeRightOffset();

    void Initialise(int viewport_width, int viewport_height,
                    int start_level_id);
    void Update(float current_time);
    void SetGameViewport(int viewport_width, int viewport_height, bool refresh);

    void OnLeftMouseButtonClicked(float x, float y);
    void OnRightMouseButtonClicked(float x, float y);

    // Debug.
    void DebugLoadLevel(int level_id);

  private:
    void LoadLevelsFromFiles();
    bool ParseLevelData(const char *path, LevelData &level_data);
    void ParseLaserData(const std::vector<std::string> &laser_lines,
                        LaserData &laser_data, const char *path);

    bool LoadNextLevel();
    bool LoadLevel(int level_id);

    void StartTransitionToNextLevel();
    void StartTransitionToLevel(int level_id);
    void UpdateTransition();
    void FinishTransition();

    void SetupOverlayShaders();

  private:
    static float current_time_;

    static constexpr std::string kLevelsRoot     = "levels/";
    static constexpr std::string kLevelExtension = ".txt";

    static constexpr int kMaxNumberOfLevels  = 10;
    static constexpr int kNumberOfLaserLines = 4;

    static constexpr float kRelativeTopOffset    = 0.2f;
    static constexpr float kRelativeBottomOffset = 0.1f;
    static constexpr float kRelativeLeftOffset   = 0.0f;
    static constexpr float kRelativeRightOffset  = 0.0f;

    static unsigned viewport_width_;
    static unsigned viewport_height_;

    unsigned top_shader_              = 0;
    unsigned bottom_shader_           = 0;
    unsigned level_transition_shader_ = 0;

    // Levels.
    std::vector<Level> levels_;
    int                current_level_id_          = -1;
    int                level_id_to_transition_to_ = -1;

    // Transition.
    const float kTransitionHalfTime = 1.0f;
    const float kTransitionDelay    = 1.0f;
    const bool  kUseTransitions     = true;

    float transition_start_time_  = 0.0f;
    bool  transition_in_progress_ = false;
};
#endif
