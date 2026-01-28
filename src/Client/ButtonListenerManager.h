#pragma once
#include <map>
#include <string>

class ButtonListenerManager
{
    inline static std::map<std::string, void(*)()> s_buttonListenersMap = {};

public:
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
    static void onClickExit();
};
