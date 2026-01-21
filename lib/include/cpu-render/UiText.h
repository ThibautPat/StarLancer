#pragma once
#include "pch.h"

class UiText : public UiBase
{
public:
    UiText();
    ~UiText();

    void Init();
    void Update(float deltaTime);
    void Draw(cpu_device* pDevice) override;

    // Gestion du texte
    void SetText(const std::string& text) { m_text = text; }
    std::string GetText() const { return m_text; }

    // Gestion de la police
    void SetFontSize(int fontSize);
    int GetFontSize() const { return m_fontSize; }

    // Gestion de la couleur
    void SetColor(unsigned int color) { m_color = color; }
    unsigned int GetColor() const { return m_color; }

    // Alignement
    enum TextAlign {
        ALIGN_LEFT = 0,
        ALIGN_CENTER = 1,
        ALIGN_RIGHT = 2
    };

    void SetAlignment(TextAlign align) { m_alignment = align; }
    TextAlign GetAlignment() const { return m_alignment; }

    cpu_font* m_pFont = nullptr;
private:

    std::string m_text;
    int m_fontSize = 12;
    unsigned int m_color = 0xFFFFFFFF; // Blanc par défaut
    TextAlign m_alignment = ALIGN_LEFT;
};