#pragma once
#include <cstdint>

enum class MessageType : uint8_t
{
    MISSING,

    CONNECTION,
    ENTITY,
    RESPAWN,
    DATA,

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

    HIT,

    MOUSE,

    COUNT,
};

enum class EntityType : uint8_t
{
    SPACESHIP,
    BULLET,
    PLANET,
};

enum class PlanetType : uint8_t
{
    SOLEIL,
    TERRE,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    LUNE,
};


#pragma pack(push,1)  //Sert a éviter le padding binaire
struct Header 
{
    MessageType type;
};

//STRUCT MESSAGE FROM SERVER ---------------------------

struct UpdateRot
{
    Header head;
    uint32_t entityID;

    float Pitch = 0.0f;
    float Yaw = 0.0f;
    float Roll = 0.0f;
};

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
    uint32_t IDUser;
};
struct SpawnPlanet
{
    Header head;
    uint32_t IDEntity;
    EntityType entity;
    uint32_t IDEntityTarget;
    PlanetType planetType;

    float PosX = 0.0f;
    float PosY = 0.0f;
    float PosZ = 0.0f;


    float radius;
    float angle;

};

struct BulletHitMessage
{
    Header head;
    uint32_t bulletID;
    uint32_t targetID;
    int targetLife;
};

struct RespawnEntity
{
    Header head;
    uint32_t targetID;
    int targetLife;
};

struct MessageScore
{
    Header head;
    uint32_t targetID;

    uint32_t Kill;
    uint32_t Death;
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

struct MouseMessage
{
    Header head;
    uint32_t ClientID;
    float X = 0;
    float Y = 0;
};

struct ConnexionMessage
{
    Header head;
    uint32_t magicnumber;
    char pseudo[32];

};

#pragma pack(pop) //Sert a éviter le padding binaire