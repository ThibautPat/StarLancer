cmake_minimum_required(VERSION 3.10)
project(SpaceGameServer)

# Définir le standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options de compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -pthread")

# Fichiers sources
set(SOURCES
    main.cpp
    Network.cpp
    ServerNetwork.cpp
    EntityServer.cpp
    EntityShipServer.cpp
    EntityBulletServer.cpp
    EntityPlanetServer.cpp
    SpaceShipMove_Calculator.cpp
    pch.cpp
)

# Fichiers headers
set(HEADERS
    main.h
    pch.h
    Utils.h
    DataProtocol.h
    Network.h
    EntityServer.h
    EntityShipServer.h
    EntityBulletServer.h
    EntityPlanetServer.h
    SpaceShipMove_Calculator.h
)

# Créer l'exécutable
add_executable(server ${SOURCES} ${HEADERS})

# Lier les bibliothèques pthread
target_link_libraries(server pthread)

# Message de fin
message(STATUS "Configuration terminée pour SpaceGameServer")
message(STATUS "Utilisation: cmake . && make")
