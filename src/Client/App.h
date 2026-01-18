#pragma once

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

	//TEMPORARY BUILD
	cpu_entity* SpaceShip;
	cpu_mesh* m_meshShip;
	cpu_texture* m_ShipTexture;
	cpu_particle_emitter* m_pEmitter;

	float camDistance = 8.0f;
	float camHeight = 2.0f;

	POINT lastMousePos;

private:
	inline static App* s_pApp = nullptr;

	cpu_mesh m_meshSphere;
	cpu_entity* m_pBall;
};
