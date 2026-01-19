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





        std::stringstream ss(msg);
        char separator;


        if (ss.fail())
            return;

        //s_pApp->m_pBall->transform.SetPosition(x, y, z);
        std::cout << "NEW LOCATION";
    }

}