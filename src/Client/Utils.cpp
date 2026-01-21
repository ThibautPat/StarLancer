#include "pch.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
namespace Utils
{
    XMFLOAT3 StringToXMFLOAT3(std::string& msg)
    {
        XMFLOAT3 float3;
        int pos = msg.find(';');
        float3.x = std::stof(msg.substr(0, pos));
        msg = msg.substr(pos + 1);
        pos = msg.find(';');
        float3.y = std::stof(msg.substr(0, pos));
        msg = msg.substr(pos + 1);
        pos = msg.find(';');
        float3.z = std::stof(msg.substr(0, pos));
        msg = msg.substr(pos + 1);
		return float3;
    }

    std::string XMFLOAT3ToString(XMFLOAT3 float3)
    {

		return std::to_string(float3.x) + ";" + std::to_string(float3.y) + ";" + std::to_string(float3.z) + ";";

    }

    void ParseurMessage(App* s_pApp, const char* buffer)
    {

        std::string msg(buffer);

        if (msg.front() == '{' && msg.back() == '}')
        {
            msg = msg.substr(1, msg.size() - 2);
        }
        else {
            return;
        }
        size_t pos = msg.find(';');
        std::string resultat;
        if (pos != std::string::npos) {
            resultat = msg.substr(0, pos);
            msg = msg.substr(pos + 1);  

        }
        if (resultat == "UPDATE")
        {
            size_t pos = msg.find(';');
            int ID = std::stoi(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            cpu_entity* entity = s_pApp->GetEntities()[ID];

            XMFLOAT3 position = StringToXMFLOAT3(msg);
            s_pApp->UpdateEntityPosition(entity, position.x, position.y, position.z);

            XMFLOAT3 Rotation = StringToXMFLOAT3(msg);
            s_pApp->UpdateEntityRotation(entity, Rotation.x, Rotation.y, Rotation.z);

            float Scale;
            pos = msg.find(';');
            Scale = std::stof(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            s_pApp->UpdateEntityScale(entity, Scale);
        }
        else if (resultat == "UPDATE_ROT")
        {
            size_t pos = msg.find(';');
            int ID = std::stoi(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            cpu_entity* entity = s_pApp->GetEntities()[ID];

            XMFLOAT3 Rotation = StringToXMFLOAT3(msg);
            s_pApp->UpdateEntityRotation(entity, Rotation.x, Rotation.y, Rotation.z);
        }
        else if (resultat == "UPDATE_POS")
        {
            size_t pos = msg.find(';');
            int ID = std::stoi(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            cpu_entity* entity = s_pApp->GetEntities()[ID];

            XMFLOAT3 position= StringToXMFLOAT3(msg);
            s_pApp->UpdateEntityPosition(entity, position.x, position.y, position.z);
        }
        else if (resultat == "UPDATE_SCALE")
        {
            size_t pos = msg.find(';');
            int ID = std::stoi(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            cpu_entity* entity = s_pApp->GetEntities()[ID];

            float Scale;
            pos = msg.find(';');
            Scale = std::stof(msg.substr(0, pos));
            msg = msg.substr(pos + 1);
            s_pApp->UpdateEntityScale(entity, Scale);
        }
        else if (resultat == "ENTITY")
        {
            size_t pos = msg.find(';');
          
            if (msg == "SPACESHIP")
            {
                EnterCriticalSection(&s_pApp->m_cs2);
                cpu_entity* SpaceShip = cpuEngine.CreateEntity();

                cpu_mesh* m_meshShip = new cpu_mesh();

               // m_meshShip->CreateCube();
                m_meshShip->LoadOBJ("../../res/3D_model/SpaceShip.obj", { 1,1,1 }, false);
                m_meshShip->FlipWinding();
                m_meshShip->Optimize();
                SpaceShip->pMesh = m_meshShip;

               
                int i = s_pApp->GetEntities().size();
                s_pApp->GetEntities()[i] = SpaceShip;
                LeaveCriticalSection(&s_pApp->m_cs2);

            }
        }

        std::stringstream ss(msg);
        char separator;

        if (ss.fail())
            return;

        std::cout << "NEW LOCATION";
    }
}