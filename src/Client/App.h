#pragma once 
#include "MenuManager.h" 
#include "Network.h"

class App
{
	// --- UI callbacks -----------------------------------------------
	static void OnPlayButtonClick()
	{
		printf("Play button clicked!\n");
	}

	static void OnPlayButtonHover()
	{
		printf("Hovering play button\n");
	}

	// --- Internal data ----------------------------------------------
	bool m_pause = false;

	cpu_font m_font;
	cpu_mesh m_meshSphere;

	cpu_entity* m_pBall = nullptr;
	std::map<int, cpu_entity*> m_entities;
	std::map<int, char[32]> m_pseudos;

	inline static App* s_pApp = nullptr;

public:
	// --- Lifecycle --------------------------------------------------
	App();
	virtual ~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnRender(int pass);
	void OnExit();

	// --- Entity management ------------------------------------------
	void UpdateEntityPosition(cpu_entity* entity, float x, float y, float z);
	void UpdateEntityRotation(cpu_entity* entity, float rx, float ry, float rz);
	void UpdateEntityScale(cpu_entity* entity, float scale);

	std::map<int, cpu_entity*>& GetEntities() { return m_entities; }

	// --- Rendering --------------------------------------------------
	static void MyPixelShader(cpu_ps_io& io);

	// --- Game state -------------------------------------------------
	enum class State
	{
		PLAY,
		GAME,
		WIN,
		LOOSE,
		PAUSE
	};

	void Pause() { m_pause = true; }
	void Resume() { m_pause = false; }

	State CurrentState = State::PLAY;

	// --- Input -------------------------
	void InputManager();

	// --- Camera -------------------------
	void CameraUpdate();

	// --- Particul -------------------------
	void UpdateParticul();

	// --- Public data  ------------------
	float camDistance = 8.0f;
	float camHeight = 2.0f;

	int nextEntityID = 0;

	cpu_particle_emitter* m_pEmitter = nullptr;
	ClientNetwork* network = nullptr;
	MenuManager* menuManager = nullptr;

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs2;
};
