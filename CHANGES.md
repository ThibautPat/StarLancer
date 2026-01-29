# Modifications Windows → Ubuntu

## 📋 Résumé des changements

Ce document détaille toutes les modifications effectuées pour porter le serveur de jeu spatial de Windows vers Ubuntu/Linux.

## 🔧 Modifications principales

### 1. Système de Threading

#### Avant (Windows):
```cpp
CRITICAL_SECTION csNewUser;
CRITICAL_SECTION csMovedUsers;

InitializeCriticalSection(&csMovedUsers);
InitializeCriticalSection(&csNewUser);

EnterCriticalSection(&network->csNewUser);
LeaveCriticalSection(&network->csNewUser);

DWORD WINAPI ThreadFonction(LPVOID lpParam);
HANDLE thread1 = CreateThread(NULL, 0, ServerNetwork::ThreadFonction, (LPVOID)this, 0, NULL);
CloseHandle(thread1);
```

#### Après (Ubuntu):
```cpp
std::mutex mtxNewUser;
std::mutex mtxMovedUsers;

// Pas besoin d'initialisation explicite

{
    std::lock_guard<std::mutex> lock(network->mtxNewUser);
    // Code critique
}

void ThreadFonction(ServerNetwork* network);  // Signature simplifiée
std::thread listenThread(ThreadFonction, this);
listenThread.detach();
```

### 2. Sleep/Timing

#### Avant (Windows):
```cpp
Sleep(static_cast<DWORD>(milliseconds));
```

#### Après (Ubuntu):
```cpp
#ifdef _WIN32
    #define SLEEP_MS(x) Sleep(x)
#else
    #define SLEEP_MS(x) usleep((x) * 1000)
#endif

SLEEP_MS(milliseconds);
```

### 3. Sockets réseau

#### Avant (Windows):
```cpp
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;
WSADATA wsa;
WSAStartup(MAKEWORD(2, 2), &wsa);
closesocket(sock);
WSACleanup();
int err = WSAGetLastError();
```

#### Après (Ubuntu):
```cpp
#ifdef _WIN32
    #include <winsock2.h>
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR   (-1)
#endif

// Pas de WSAStartup sous Linux
close(sock);  // au lieu de closesocket
int err = errno;  // au lieu de WSAGetLastError()
```

### 4. Manipulation de chaînes

#### Avant (Windows):
```cpp
strncpy_s(user->Pseudo, 32, message.pseudo, _TRUNCATE);
```

#### Après (Ubuntu):
```cpp
size_t len = strnlen(message.pseudo, sizeof(message.pseudo));
memcpy(user->Pseudo, message.pseudo, std::min(len, sizeof(user->Pseudo) - 1));
user->Pseudo[std::min(len, sizeof(user->Pseudo) - 1)] = '\0';
```

### 5. Taille des structures socket

#### Avant (Windows):
```cpp
int sizeAddr = sizeof(addr);
```

#### Après (Ubuntu):
```cpp
socklen_t sizeAddr = sizeof(addr);  // Type spécifique Linux
```

## 📦 Nouvelles structures autonomes (Utils.h)

### Structure XMFLOAT3
Remplace DirectXMath::XMFLOAT3 avec toutes les opérations mathématiques:
- Constructeurs
- Opérateurs (+, -, *, +=, -=)
- Méthodes: Length(), Normalize(), Normalized(), Dot(), Cross()

### Structure cpu_transform
Gère les transformations 3D complètes:
- **pos**: Position dans l'espace
- **dir**: Direction (forward vector)
- **up**: Vecteur "haut"
- **right**: Vecteur "droite"
- **Identity()**: Réinitialise à l'identité
- **SetYPR()**: Définit la rotation avec Yaw/Pitch/Roll
- **UpdateWorld()**: Recalcule les vecteurs de base
- **OrbitAroundAxis()**: Fait orbiter autour d'un point

### Structure cpu_aabb
Bounding box alignée sur les axes:
- **min/max**: Coins de la boîte
- **Contains()**: Test de contenance d'un point
- **Zero()**: Réinitialise la boîte

### Fonctions utilitaires (namespace cpu)
- **AabbAabb()**: Test de collision entre deux AABB
- **Distance()**: Distance entre deux points
- **Lerp()**: Interpolation linéaire
- **Clamp()**: Limite une valeur entre min et max

## 🔄 Changements de comportement

### 1. Gestion d'erreurs réseau
- Windows: `WSAGetLastError()` avec codes d'erreur Windows
- Linux: `errno` avec codes d'erreur POSIX
- Ajout de `#ifdef` pour gérer les deux

### 2. Gestion mémoire
- Pas de changement majeur
- Les `delete` et `new` fonctionnent identiquement
- Utilisation de smart pointers recommandée (non implémentée)

### 3. Sécurité des pointeurs
- Ajout de vérifications `nullptr` dans plusieurs endroits
- Exemple: `if (ship->LastKiller)` avant d'accéder aux membres

## 📊 Comparaison des performances

### Avantages Ubuntu:
- ✅ Pas d'overhead de WSA
- ✅ Meilleure gestion native des sockets
- ✅ Threading plus efficace avec `std::thread`
- ✅ Moins de latence système

### Compromis:
- ⚠️ Pas d'API DirectX (remplacé par structures custom)
- ⚠️ Nécessite recompilation (pas de binaires portables)

## 🛠️ Outils de développement

### Windows:
- Visual Studio
- MSVC compiler
- Windows SDK

### Ubuntu:
- GCC/G++
- Make ou CMake
- GDB pour le débogage
- Valgrind pour la détection de fuites mémoire

## 📝 Checklist de portage

- [x] Remplacer CRITICAL_SECTION par std::mutex
- [x] Remplacer CreateThread par std::thread
- [x] Remplacer WSA par sockets POSIX
- [x] Remplacer Sleep par usleep
- [x] Créer structures XMFLOAT3 custom
- [x] Créer cpu_transform custom
- [x] Adapter les fonctions de chaînes
- [x] Adapter les types de données réseau
- [x] Créer Makefile
- [x] Créer CMakeLists.txt
- [x] Tester la compilation
- [x] Documenter les changements

## 🚀 Pour aller plus loin

### Améliorations possibles:
1. **Utiliser des smart pointers** (`std::unique_ptr`, `std::shared_ptr`)
2. **Ajouter des logs structurés** (spdlog, etc.)
3. **Implémenter un pool de threads** pour meilleures performances
4. **Ajouter des tests unitaires** (Google Test)
5. **Implémenter une vraie physique 3D** (Bullet Physics)
6. **Ajouter un système de configuration** (JSON, YAML)
7. **Implémenter la reconnexion automatique**
8. **Ajouter des statistiques en temps réel**

### Compatibilité multi-plateforme:
Le code utilise maintenant des `#ifdef _WIN32` et peut être compilé sur:
- Ubuntu/Debian
- Fedora/RedHat
- Arch Linux
- macOS (avec modifications mineures)
- Windows (avec les mêmes sources)

## 📞 Support

Pour toute question sur le portage:
1. Consultez le README.md
2. Vérifiez les warnings de compilation
3. Utilisez `make clean && make` pour une recompilation complète
4. Testez avec `./server` dans un terminal

## 🎓 Ressources

- [POSIX Sockets](https://man7.org/linux/man-pages/man7/socket.7.html)
- [C++17 Threading](https://en.cppreference.com/w/cpp/thread)
- [UDP Programming](https://beej.us/guide/bgnet/)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)
