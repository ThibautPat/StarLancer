#pragma once
namespace Utils
{
	void ParseurMessage(App* s_pApp, const char* buffer);   
	XMFLOAT3 StringToXMFLOAT3(std::string& msg);
	std::string XMFLOAT3ToString(XMFLOAT3 float3);

}