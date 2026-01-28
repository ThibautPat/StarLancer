#pragma once
class UiBase : public cpu_sprite
{
public:

    std::string name = "";
    virtual ~UiBase() {}
    virtual void Draw(cpu_device* pDevice) {
        if (!dead && visible && pTexture) {
            pDevice->DrawSprite(this);
        }
    }
};