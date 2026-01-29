#pragma once
#include <map>
#include <string>
#include <windows.h>

class ButtonListenerManager
{
public:
    inline static std::map<std::string, void(*)()> s_buttonListenersMap = {};

    // Variables pour l'input
    inline static std::string s_currentInput = "";
    inline static bool s_inputComplete = false;
    inline static bool s_inputActive = false;

    // Tableau pour éviter les répétitions de touches
    inline static bool s_keyWasPressed[256] = { false };

    static void Init();
    static void RegisterButtonListener(const std::string& name, void (*l)())
    {
        s_buttonListenersMap[name] = l;
    }
    static void (*GetButtonListener(const std::string& name))()
    {
        auto it = s_buttonListenersMap.find(name);
        if (it != s_buttonListenersMap.end())
        {
            return it->second;
        }
        return nullptr;
    }
    static void onClickPlay();

    // Démarre la saisie d'IP
    static void EnterIp();

    static void EnterPseudo();

    // À appeler dans ta boucle principale pour capturer les touches
    static void UpdateInput();

    // Fonction pour récupérer l'input actuel
    static std::string GetCurrentInput();

    // Fonction pour savoir si l'input est complet (Enter pressé)
    static bool IsInputComplete();
};