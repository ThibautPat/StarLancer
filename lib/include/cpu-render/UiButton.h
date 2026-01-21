// UiButton.h
#pragma once
#include "pch.h"

class UiButton : public UiBase
{
public:
    using ButtonCallback = void(*)();

    UiButton();
    ~UiButton();

    void Init();
    void Update(float deltaTime);
    void Draw(cpu_device* pDevice) override;

    void SetTexture(cpu_texture* normalTexture, cpu_texture* hoverTexture = nullptr, cpu_texture* pressedTexture = nullptr);

    void SetOnClickCallback(ButtonCallback callback) { m_onClickCallback = callback; }
    void SetOnHoverCallback(ButtonCallback callback) { m_onHoverCallback = callback; }
    void SetOnPressCallback(ButtonCallback callback) { m_onPressCallback = callback; }
    void SetOnReleaseCallback(ButtonCallback callback) { m_onReleaseCallback = callback; }

    bool isHovered() const { return m_hovered; }
    bool isPressed() const { return m_pressed; }
    bool isDisabled() const { return m_disabled; }
    void setDisabled(bool disabled) { m_disabled = disabled; }

    bool isAnimated() const { return m_animated; }
    void setAnimated(bool animated) { m_animated = animated; }
    void SetupSpriteSheet(int frameWidth, int frameHeight, int frameCount, float frameDuration = 0.1f);

    int getCurrentFrame() const { return m_currentFrame; }
    void setCurrentFrame(int frame) {
        if (frame >= 0 && frame < m_frameCount) {
            m_currentFrame = frame;
            UpdateSpriteRect();
        }
    }
    bool isLooping() const { return m_looping; }
    void setLooping(bool looping) { m_looping = looping; }
    bool isPlaying() const { return m_playing; }
    void Play();
    void Stop();
    void Pause();
    void Resume();

    void SetSize(int width, int height) { m_width = width; m_height = height; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    struct Rect {
        int left, top, right, bottom;
        bool Contains(int px, int py) const {
            return px >= left && px <= right && py >= top && py <= bottom;
        }
    };
    Rect GetBounds() const;

private:
    void UpdateSpriteRect();
    void CheckInteraction();

    cpu_texture* m_normalTexture = nullptr;
    cpu_texture* m_hoverTexture = nullptr;
    cpu_texture* m_pressedTexture = nullptr;
    cpu_texture* m_currentTexture = nullptr;

    ButtonCallback m_onClickCallback = nullptr;
    ButtonCallback m_onHoverCallback = nullptr;
    ButtonCallback m_onPressCallback = nullptr;
    ButtonCallback m_onReleaseCallback = nullptr;

    bool m_hovered = false;
    bool m_pressed = false;
    bool m_disabled = false;

    bool m_animated = false;
    int m_frameCount = 1;
    int m_currentFrame = 0;
    int m_frameWidth = 0;
    int m_frameHeight = 0;
    float m_frameDuration = 0.1f;
    float m_frameTimer = 0.0f;
    bool m_looping = true;
    bool m_playing = false;

        int m_width = 0;
    int m_height = 0;

    struct SrcRect {
        int left, top, width, height;
    };
    SrcRect m_sourceRect = { 0, 0, 0, 0 };
};