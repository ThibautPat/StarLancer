#include "pch.h"
#include "ButtonListenerManager.h"
void ButtonListenerManager::Init()
{
	ButtonListenerManager::RegisterButtonListener("onClickPlay", onClickPlay);

}

void ButtonListenerManager::onClickPlay()
{
	App::GetInstance().CurrentState = App::State::GAME;
}
void ButtonListenerManager::onClickExit()
{
	exit(0);
}