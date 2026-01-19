#pragma once

class App
{
public:
	App();
	virtual ~App();

	void UpdateEntityPosition(cpu_entity* entity, float x, float y, float z);

	void UpdateEntityRotation(cpu_entity* entity, float rx, float ry, float rz);

	void UpdateEntityScale(cpu_entity* entity, float scale);

	void SendMessageToServer(std::string message);


	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);

	SOCKET UserSock;
	HANDLE thread1;
	sockaddr_in ServeurAddr;
	//TEMPORARY BUILD
	cpu_particle_emitter* m_pEmitter;

	float camDistance = 8.0f;
	float camHeight = 2.0f;

	POINT lastMousePos;

	std::map<int, cpu_entity*>& GetEntities() { return m_entities; }

private:
	cpu_font m_font;

	std::map<int, cpu_entity*> m_entities;
	inline static App* s_pApp = nullptr;
	cpu_mesh m_meshSphere;
	cpu_entity* m_pBall;
};
