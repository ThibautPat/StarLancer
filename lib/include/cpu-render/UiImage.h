#pragma once
class UiImage : public UiBase
{
public:
    UiImage();
    ~UiImage();
    void Init();
    void Update(float deltaTime);

    void SetSize(int w, int h) { width = w; height = h; }

    bool isAnimated() const { return m_animated; }
    void setAnimated(bool _animated) { m_animated = _animated; }
    int getFrameCount() const { return m_frameCount; }
    void setFrameCount(int _frameCount) { m_frameCount = _frameCount; }
    int getCurrentFrame() const { return m_currentFrame; }
    void setCurrentFrame(int _currentFrame) {
        if (m_currentFrame != _currentFrame && _currentFrame >= 0 && _currentFrame < m_frameCount) {
            m_currentFrame = _currentFrame;
            UpdateSpriteRect();
        }
    }
    int getFrameWidth() const { return m_frameWidth; }
    void setFrameWidth(int _frameWidth) { m_frameWidth = _frameWidth; }
    int getFrameHeight() const { return m_frameHeight; }
    void setFrameHeight(int _frameHeight) { m_frameHeight = _frameHeight; }
    float getFrameDuration() const { return m_frameDuration; }
    void setFrameDuration(float _duration) { m_frameDuration = _duration; }
    bool isLooping() const { return m_looping; }
    void setLooping(bool _looping) { m_looping = _looping; }
    bool isPlaying() const { return m_playing; }
    void Play() { m_playing = true; m_currentFrame = 0; m_frameTimer = 0.0f; UpdateSpriteRect(); }
    void Stop() { m_playing = false; m_currentFrame = 0; UpdateSpriteRect(); }
    void Pause() { m_playing = false; }
    void Resume() { m_playing = true; }
    void SetupSpriteSheet(int frameWidth, int frameHeight, int frameCount, float frameDuration = 0.1f);
    void UpdateSpriteRect();
    struct Rect {
        int left;
        int top;
        int width;
        int height;
    };
    Rect getSourceRect() const { return m_sourceRect; }
    void Draw(cpu_device* pDevice) override;

    // Taille d'affichage du sprite
    int width = 0;
    int height = 0;

private:
    bool m_animated = false;
    int m_frameCount = 1;
    int m_currentFrame = 0;
    int m_frameWidth = 0;
    int m_frameHeight = 0;
    float m_frameDuration = 0.1f;
    float m_frameTimer = 0.0f;
    bool m_looping = true;
    bool m_playing = false;
    Rect m_sourceRect = { 0, 0, 0, 0 };
};