#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H
#include <mutex>

class AnimationManager {
private:
    static AnimationManager* m_instance;
    static std::mutex m_mutex;

    double m_dt = 0;
    double m_time = 0;
    uint m_frame = 0;
protected:
    AnimationManager() {};
    ~AnimationManager() {};
public:
    AnimationManager(AnimationManager& other) = delete;
    void operator=(const AnimationManager&) = delete;
    static AnimationManager* getInstance();

    void update(double time);
};
#endif