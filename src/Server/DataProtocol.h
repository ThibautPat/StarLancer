#pragma once
#include <cstdint>

enum class MessageType : uint8_t
{
    MISSING,

    CONNECTION,
    ENTITY,

    UPDATE_POS,
    UPDATE_ROT,
    UPDATE_SCALE,
    UPDATE_FULL,

    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,

    FIRE_BULLET,
    FIRE_MISSILE,

    COUNT,
};

enum class EntityType : uint8_t
{
    SPACESHIP ,
    BULLET ,
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

    float PosX = 0.0f;
    float PosY = 0.0f;
    float PosZ = 0.0f;
};

struct ReturnConnexionMessage
{
    Header head;
    uint32_t ClientID;
};

struct SpawnPlayer
{
    Header head;
    uint32_t IDEntity;
    EntityType entity;

    char pseudo[32];
};

struct SpawnEntity
{
    Header head;
    uint32_t IDEntity;
    EntityType entity;
};

//STRUCT MESSAGE FROM CLIENT ---------------------------

struct AABBUpdateMessage
{
    Header head;
    uint32_t IDEntity;

    float minX = 0;
    float minY = 0;
    float minZ = 0;

    float maxX = 0;
    float maxY = 0;
    float maxZ = 0;
};

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