#pragma once
#include "pch.h"
#include "Menu.h"
#include <fstream>

class MenuManager
{

protected:
	Menu* m_currentMenu = nullptr;
	std::vector<Menu*> menuList;

public:
	void Start();
	void Update(float deltatime);
	Menu* getMenu(std::string name);
	Menu* getCurrentMenu() { return m_currentMenu; }
	void forceActiveMenu(std::string name);
	void forceUnActiveMenu(std::string name);

	void loadAllMenu();
	void switchMenu(std::string name);
	void Draw( );


};

