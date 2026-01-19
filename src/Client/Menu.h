#pragma once
#include "pch.h"

class Menu
{
public:
    std::vector<UiBase*> uiList;
    std::string name = "";
    
    bool active = true;
    void Start() {};
    void SetActive(bool _active) { active = _active; };


    void addButton(
        std::string _Id,
        XMFLOAT3 _pos,
        XMFLOAT3 _size,
        std::string _name,
        std::vector<std::string> _ListenerList,
        std::string _hover = "",
        bool _animated = false,
        int _FrameCount = 0,
        int _currentFrame = 0,
        XMFLOAT2 _FrameSize = { 0, 0 },
        bool _looping = false,
        float _frameRate = 0
    );

    void addImage(
        std::string _Id,
        XMFLOAT3 _pos,
        XMFLOAT3 _size,
        std::string _name,
        bool _animated = false,
        int _FrameCount = 0,
        int _currentFrame = 0,
        XMFLOAT2 _FrameSize = { 0, 0 },
        bool _looping = false,
        float _frameRate = 0
    );

    void addText(
        std::string _Id,
        XMFLOAT3 _pos,
        XMFLOAT3 _size,
        std::string _name,
        XMFLOAT3 _Color,
        std::string _Font,
        std::string _text,
        int _FontSize
    );

    UiBase* getElement(std::string _name);

    void Update(float deltaTime);
    void Draw(cpu_device* pDevice);
    void Clear();
};