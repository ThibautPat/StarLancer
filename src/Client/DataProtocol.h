#pragma once
#include <cstdint>

enum class MessageType : uint8_t 
{
    CONNEXION = 0,
    UPDATE_POS = 1,
    UPDATE_ROT = 2,
    UPDATE_SCALE = 3,
    UPDATE_FULL = 4,
    ENTITY = 5,
    FORWARD = 6,
    BACKWARD = 7,
    LEFT = 8,
    RIGHT = 9
};

enum class EntityType : uint8_t
{
    SPACESHIP = 0,
};

#pragma pack(push,1)  //Sert a éviter le padding binaire
struct Header 
{
    MessageType type;
};

//STRUCT MESSAGE FROM SERVER ---------------------------
struct UpdatePos
{
    Header head;
    uint32_t entityID;

    float PosX = 0;
    float PosY = 0;
    float PosZ = 0;
};

struct ReturnConnexionMessage
{
    Header head;
    uint32_t ClientID;
};

struct SpawnEntity
{
    Header head;
    EntityType entity;
};

//STRUCT MESSAGE FROM CLIENT ---------------------------

struct InputMessage
{
    Header head;
    uint32_t ClientID;
};

struct ConnexionMessage
{
    Header head;
    uint32_t magicnumber;
};

#pragma pack(pop) //Sert a éviter le padding binaire