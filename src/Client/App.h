#pragma once

class App
{
public:
	App();
	virtual ~App();

	void UpdateEntityPosition(cpu_entity* entity, float x, float y, float z);

	void UpdateEntityRotation(cpu_entity* entity, float rx, float ry, float rz);

	void UpdateEntityScale(cpu_entity* entity, float scale);


	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);

	SOCKET ClientSock;
	HANDLE thread1;
	std::map<int, cpu_entity*>& GetEntities() { return m_entities; }

private:
	cpu_font m_font;

	inline static App* s_pApp = nullptr;
	std::map<int, cpu_entity*> m_entities;
	cpu_mesh m_meshSphere;
	cpu_entity* m_pBall;
};
