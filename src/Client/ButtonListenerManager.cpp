#include "pch.h"
#include "ButtonListenerManager.h"

void ButtonListenerManager::Init()
{
    ButtonListenerManager::RegisterButtonListener("onClickPlay", onClickPlay);
    ButtonListenerManager::RegisterButtonListener("EnterIp", EnterIp);
}

void ButtonListenerManager::onClickPlay()
{
    App::GetInstance().CurrentState = App::State::GAME;
}

void ButtonListenerManager::EnterIp()
{
    s_currentInput = "";
    s_inputComplete = false;
    s_inputActive = true;

    // Réinitialiser l'état des touches
    for (int i = 0; i < 256; i++)
    {
        s_keyWasPressed[i] = false;
    }
}

void ButtonListenerManager::UpdateInput()
{
    if (!s_inputActive || s_inputComplete)
        return;

    // Vérifier Enter
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
    {
        if (!s_keyWasPressed[VK_RETURN])
        {
            s_keyWasPressed[VK_RETURN] = true;
            s_inputComplete = true;
            s_inputActive = false;

            // Traiter l'input
            if (s_currentInput == "local")
            {
                App::GetInstance().network->ChoseTarget("127.0.0.1");
            }
            else if (!s_currentInput.empty())
            {
                App::GetInstance().network->ChoseTarget(s_currentInput.c_str());
            }
        }
    }
    else
    {
        s_keyWasPressed[VK_RETURN] = false;
    }

    // Vérifier Backspace
    if (GetAsyncKeyState(VK_BACK) & 0x8000)
    {
        if (!s_keyWasPressed[VK_BACK])
        {
            s_keyWasPressed[VK_BACK] = true;
            if (!s_currentInput.empty())
            {
                s_currentInput.pop_back();
                dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
            }
        }
    }
    else
    {
        s_keyWasPressed[VK_BACK] = false;
    }

    for (int vk = 0x30; vk <= 0x39; vk++) // VK_0 à VK_9
    {
        if (GetAsyncKeyState(vk) & 0x8000)
        {
            if (!s_keyWasPressed[vk])
            {
                s_keyWasPressed[vk] = true;
                s_currentInput += (char)vk;
                dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
            }
        }
        else
        {
            s_keyWasPressed[vk] = false;
        }
    }

    // Vérifier les chiffres du numpad
    for (int vk = VK_NUMPAD0; vk <= VK_NUMPAD9; vk++)
    {
        if (GetAsyncKeyState(vk) & 0x8000)
        {
            if (!s_keyWasPressed[vk])
            {
                s_keyWasPressed[vk] = true;
                char digit = '0' + (vk - VK_NUMPAD0);
                s_currentInput += digit;
                dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
            }
        }
        else
        {
            s_keyWasPressed[vk] = false;
        }
    }

    for (int vk = 0x41; vk <= 0x5A; vk++) // VK_A à VK_Z
    {
        if (GetAsyncKeyState(vk) & 0x8000)
        {
            if (!s_keyWasPressed[vk])
            {
                s_keyWasPressed[vk] = true;
                char c = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (char)vk : (char)(vk + 32);
                s_currentInput += c;
                dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
            }
        }
        else
        {
            s_keyWasPressed[vk] = false;
        }
    }

    if (GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000)
    {
        if (!s_keyWasPressed[VK_OEM_PERIOD])
        {
            s_keyWasPressed[VK_OEM_PERIOD] = true;
            s_currentInput += '.';
            dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
        }
    }
    else
    {
        s_keyWasPressed[VK_OEM_PERIOD] = false;
    }

    if (GetAsyncKeyState(VK_DECIMAL) & 0x8000)
    {
        if (!s_keyWasPressed[VK_DECIMAL])
        {
            s_keyWasPressed[VK_DECIMAL] = true;
            s_currentInput += '.';
            dynamic_cast<UiText*>(App::GetInstance().menuManager->getCurrentMenu()->getElement("text_3"))->SetText(s_currentInput);
        }
    }
    else
    {
        s_keyWasPressed[VK_DECIMAL] = false;
    }
}

std::string ButtonListenerManager::GetCurrentInput()
{
    return s_currentInput;
}

bool ButtonListenerManager::IsInputComplete()
{
    return s_inputComplete;
}