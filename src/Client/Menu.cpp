#include "pch.h"
#include "Menu.h"
#include "ButtonListenerManager.h"
void Menu::addButton(
    std::string _Id,
    XMFLOAT3 _pos,
    XMFLOAT3 _size,
    std::string _name,
    std::vector<std::string> _ListenerList,
    std::string _hover,
    bool _animated,
    int _FrameCount,
    int _currentFrame,
    XMFLOAT2 _FrameSize,
    bool _looping,
    float _frameRate)
{
    UiButton* button = new UiButton();

    button = cpuEngine.CreateUiElement<UiButton>();
    _name = "../../" + _name;
    cpu_texture* normalTex = new cpu_texture();
    normalTex->Load(_name.c_str());

    cpu_texture* hoverTex = nullptr;
    _hover = "../../" + _hover;

    if (!_hover.empty()) {
        hoverTex = new cpu_texture();
        hoverTex->Load(_hover.c_str());
    }

    button->x = (int)(_pos.x * cpuDevice.GetWidth());
    button->y = (int)(_pos.y * cpuDevice.GetHeight());
    button->z = (int)_pos.z;

    button->SetSize(_size.x, _size.y);

    if (_animated && _FrameCount > 1) {
        button->SetupSpriteSheet(
            (int)_FrameSize.x,
            (int)_FrameSize.y,
            _FrameCount,
            1/_frameRate
        );
        button->setLooping(_looping);
        button->setCurrentFrame(_currentFrame);
    }

    button->SetTexture(normalTex, hoverTex, nullptr);

        button->Init();

    button->anchorX = button->GetWidth() / 2;
    button->anchorY = button->GetHeight() / 2;

    if (_animated && _looping) {
        button->Play();
    }

    button->SetOnClickCallback(ButtonListenerManager::GetButtonListener(_ListenerList[0]));

    button->name = _Id;
    uiList.push_back(button);
}

void Menu::addImage(
    std::string _Id,
    XMFLOAT3 _pos,
    XMFLOAT3 _size,
    std::string _name,
    bool _animated,
    int _FrameCount,
    int _currentFrame,
    XMFLOAT2 _FrameSize,
    bool _looping,
    float _frameRate)
{
    UiImage* image =new UiImage();
    image = cpuEngine.CreateUiElement<UiImage>();
    _name = "../../" + _name;

    cpu_texture* texture = new cpu_texture();
    texture->Load(_name.c_str());
    image->pTexture = texture;

    image->x = (int)(_pos.x * cpuDevice.GetWidth());
    image->y = (int)(_pos.y * cpuDevice.GetHeight());
    image->z = (int)_pos.z;

    image->SetSize((int)_size.x, (int)_size.y);

    if (_animated && _FrameCount > 1) {
        image->SetupSpriteSheet(
            (int)_FrameSize.x,
            (int)_FrameSize.y,
            _FrameCount,
            1/_frameRate
        );
        image->setLooping(_looping);
        image->setCurrentFrame(_currentFrame);
    }

    image->Init();

    image->anchorX = image->width / 2;
    image->anchorY = image->height / 2;

    if (_animated && _looping) {
        image->Play();
    }

    image->name = _Id;
    uiList.push_back(image);
}

void Menu::addText(
    std::string _Id,
    XMFLOAT3 _pos,
    XMFLOAT3 _size,
    std::string _name,
    XMFLOAT3 _Color,
    std::string _Font,
    std::string _text,
    int _FontSize)
{
    UiText* text = new UiText();
    text = cpuEngine.CreateUiElement<UiText>();

    text->m_pFont = new cpu_font();
    text->m_pFont->Create(_FontSize, _Color, _Font.c_str());
    text->SetText(_text);
    text->SetFontSize(_FontSize);

    text->x = (int)(_pos.x * cpuDevice.GetWidth());
    text->y = (int)(_pos.y * cpuDevice.GetHeight());
    text->z = (int)_pos.z;
    text->visible = true;
    text->dead = false;

    text->Init();
    text->SetAlignment(UiText::ALIGN_CENTER);
    text->name = _Id;
    uiList.push_back(text);

}

UiBase* Menu::getElement(std::string _name)
{
    for (UiBase* element : uiList)
    {
        if (element->name == _name)
        {
            return element;
        }
    }
    return nullptr;
}

void Menu::Update(float deltaTime)
{
    if (!active) return;

    for (UiBase* element : uiList)
    {
        if (!element->visible) continue;

        UiButton* button = dynamic_cast<UiButton*>(element);
        if (button) {
            button->Update(deltaTime);
            continue;
        }

        UiImage* image = dynamic_cast<UiImage*>(element);
        if (image) {
            image->Update(deltaTime);
            continue;
        }
    }
}

void Menu::Draw(cpu_device* pDevice)
{
    if (!active) return;

    for (UiBase* element : uiList)
    {
        if (element->visible) 
            element->Draw(pDevice);
    }
}

void Menu::Clear()
{
    for (UiBase* element : uiList)
    {
        cpuEngine.Release(element);
    }
    uiList.clear();
}