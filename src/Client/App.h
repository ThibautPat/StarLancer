#pragma once 
#include "MenuManager.h" 
#include "Network.h"
#include "EntityBulletClient.h"
#include "EntityClient.h"
#include <queue>
#include "ButtonListenerManager.h"
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

	std::map<uint32_t, EntityClient*> m_entities;


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
	void RenderEntityLabels( cpu_camera* camera, float screenWidth, float screenHeight);

	void OnExit();

	void ClearDeadEntity();

	// --- Entity management ------------------------------------------
	void CreateBullet(uint32_t IdEntity, uint32_t OwnerID);
	void UpdateEntityPosition(cpu_entity* entity, float x, float y, float z);
	void UpdateEntityRotation(cpu_entity* entity, float rx, float ry, float rz);
	void UpdateEntityScale(cpu_entity* entity, float scale);

	std::map<uint32_t, EntityClient*>& GetEntitiesList() { return m_entities; }
	EntityClient* GetEntitie(uint32_t index) 
	{	
		auto it = m_entities.find(index);
		if (it == m_entities.end())
			return nullptr;
		return it->second;
	}

	// --- Rendering --------------------------------------------------
	static void MyPixelShader(cpu_ps_io& io);

	// --- Game state -------------------------------------------------
	enum class State
	{
		PLAY,
		GAME
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
	bool m_LockCursor = true;

	int nextEntityID = 0;

	float coldownNetwork = 0.f;
	float TimerBeforeRetry = 5.f;

	float coldownShoot = 0.3f;
	float TimerShoot = 0.3f;

	XMFLOAT2 CursorDir{ 0,0 };

	cpu_particle_emitter* m_pEmitter = nullptr;
	ClientNetwork* network = nullptr;
	MenuManager* menuManager = nullptr;
	bool connected = false;

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_cs2;
};
