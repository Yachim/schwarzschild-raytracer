#ifndef ANIMATION_H
#define ANIMATION_H
enum EaseType {
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT
};

class Animation {
protected:
    EaseType m_easeType;
    double m_startTime;
    double m_duration;
    bool m_repeating = false;
    bool m_isPlaying = false;

public:
    Animation(EaseType easeType, double startTime, double duration);
    Animation(const Animation& animation);
    virtual void animate(double t) = 0; // t is an animation parameter ranging from 0 to 1
    double ease(double t);

    EaseType getEaseType() const;
    void setEaseType(EaseType easeType);

    double getStartTime() const;
    void setStartTime(double startTime);

    double getDuration() const;
    void setDuration(double duration);

    bool getRepeating() const;
    void setRepeating(bool repeating);

    bool isPlaying() const;
    void setPlaying(bool playing);
};
#endif
