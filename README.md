# Serveur de Jeu Spatial - Version Ubuntu

Ce projet est un serveur de jeu spatial multijoueur porté depuis Windows vers Ubuntu/Linux.

## 📋 Prérequis

- Ubuntu 20.04 ou supérieur (ou toute distribution Linux récente)
- GCC/G++ avec support C++17
- CMake 3.10+ (optionnel, pour compilation avec CMake)
- Make

### Installation des dépendances

```bash
sudo apt update
sudo apt install build-essential g++ make cmake
```

## 🔧 Compilation

### Méthode 1: Avec Make (recommandé)

```bash
# Compilation
make

# Exécution
./server
```

### Méthode 2: Avec CMake

```bash
# Configuration
cmake .

# Compilation
make

# Exécution
./server
```

### Méthode 3: Compilation manuelle

```bash
g++ -std=c++17 -O2 -pthread \
    main.cpp \
    Network.cpp \
    ServerNetwork.cpp \
    EntityServer.cpp \
    EntityShipServer.cpp \
    EntityBulletServer.cpp \
    EntityPlanetServer.cpp \
    SpaceShipMove_Calculator.cpp \
    pch.cpp \
    -o server
    
./server
```

## 🎮 Fonctionnalités

Le serveur gère:
- ✅ Connexions multijoueurs UDP
- ✅ Physique 3D des vaisseaux spatiaux
- ✅ Système de tir et de projectiles
- ✅ Détection de collision AABB
- ✅ Système de score (kills/deaths)
- ✅ Respawn automatique
- ✅ Planètes avec orbites
- ✅ Synchronisation réseau

## 🔌 Configuration réseau

- **Port par défaut**: 1888 (UDP)
- **Protocole**: UDP avec numéro magique 8542
- **FPS cible**: 60 FPS

Pour modifier le port, éditez `Network.cpp` ligne 60:
```cpp
if (!BindSocketToPort(m_NetworkSocket, 1888))  // Changer 1888
```

## 📁 Structure du projet

```
.
├── main.cpp                          # Point d'entrée du serveur
├── main.h
├── pch.h / pch.cpp                   # Precompiled header
├── Utils.h                           # Structures mathématiques (XMFLOAT3, cpu_transform, etc.)
├── DataProtocol.h                    # Protocole réseau (messages)
├── Network.h / Network.cpp           # Couche réseau de base
├── ServerNetwork.cpp                 # Logique serveur
├── EntityServer.h / .cpp             # Classe de base des entités
├── EntityShipServer.h / .cpp         # Vaisseaux spatiaux
├── EntityBulletServer.h / .cpp       # Projectiles
├── EntityPlanetServer.h / .cpp       # Planètes
├── SpaceShipMove_Calculator.h / .cpp # Déplacement des vaisseaux
├── Makefile                          # Compilation Make
└── CMakeLists.txt                    # Compilation CMake
```

## 🔄 Changements par rapport à Windows

### Remplacements effectués:

1. **CRITICAL_SECTION** → `std::mutex` avec `std::lock_guard`
2. **CreateThread** → `std::thread`
3. **Sleep(ms)** → `usleep(ms * 1000)` sous Linux
4. **WSA (Windows Sockets)** → sockets POSIX
5. **strncpy_s** → `memcpy` avec vérification manuelle
6. **DWORD WINAPI** → `void` avec threads C++11
7. **DirectXMath (XMFLOAT3, etc.)** → Structures custom dans `Utils.h`

### Structures mathématiques autonomes:

Le fichier `Utils.h` contient maintenant:
- `XMFLOAT3` - Vecteur 3D avec opérateurs
- `cpu_transform` - Transform 3D (position, rotation, direction)
- `cpu_aabb` - Bounding box alignée sur les axes
- `cpu_obb` - Bounding box orientée
- Fonctions mathématiques (normalize, dot, cross, etc.)

## 🧹 Nettoyage

```bash
# Avec Make
make clean

# Avec CMake
make clean
rm CMakeCache.txt
rm -rf CMakeFiles/

# Manuel
rm -f *.o server
```

## 🐛 Débogage

Pour compiler en mode debug avec symboles:

```bash
g++ -std=c++17 -g -pthread -DDEBUG \
    main.cpp Network.cpp ServerNetwork.cpp \
    EntityServer.cpp EntityShipServer.cpp \
    EntityBulletServer.cpp EntityPlanetServer.cpp \
    SpaceShipMove_Calculator.cpp pch.cpp \
    -o server_debug
    
# Utiliser avec GDB
gdb ./server_debug
```

## 📊 Performances

- Le serveur tourne à 60 FPS constant
- Gestion efficace des collisions avec broad phase
- Pas de fuites mémoire (smart pointers pour entités critiques)
- Thread séparé pour la réception réseau

## ⚠️ Notes importantes

1. Le serveur utilise UDP, donc pas de garantie de livraison des paquets
2. Les collisions bullet-bullet sont désactivées par design
3. Le respawn se fait après 5 secondes de mort
4. Les entités mortes (sauf vaisseaux) sont supprimées automatiquement

## 📝 Licence

Ce projet est un portage d'un serveur Windows existant vers Linux.
