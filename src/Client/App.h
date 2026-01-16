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

	SOCKET UserSock;
	HANDLE thread1;


private:
	inline static App* s_pApp = nullptr;

	cpu_mesh m_meshSphere;
	cpu_entity* m_pBall;
};
