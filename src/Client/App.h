#pragma once
#include "MenuManager.h"

class App
{
public:
	App();
	virtual ~App();

	static void MoveBall(const char* buffer);

	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);

	SOCKET ClientSock;
	HANDLE thread1;

	MenuManager* menuManager;
	void Pause() { m_pause = true; }
	void Resume() { m_pause = false; }

	enum States
	{
		PLAY,
		GAME,
		WIN,
		LOOSE,
		PAUSE,

	};
	States CurrentStates = States::PLAY;
private:
	inline static App* s_pApp = nullptr;
	bool m_pause = false;


	static void OnPlayButtonClick()
	{
		// Code quand le bouton est cliqué
		printf("Play button clicked!\n");
	}

	static void OnPlayButtonHover()
	{
		printf("Hovering play button\n");
	}

	cpu_mesh m_meshSphere;
	cpu_entity* m_pBall;
};
