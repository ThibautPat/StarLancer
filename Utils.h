#pragma once

#include <cmath>
#include <random>
#include <algorithm>

// ============================================================================
// STRUCTURES DE BASE
// ============================================================================

struct XMFLOAT3
{
    float x;
    float y;
    float z;

    XMFLOAT3() : x(0.0f), y(0.0f), z(0.0f) {}
    XMFLOAT3(const XMFLOAT3&) = default;
    XMFLOAT3& operator=(const XMFLOAT3&) = default;
    XMFLOAT3(XMFLOAT3&&) = default;
    XMFLOAT3& operator=(XMFLOAT3&&) = default;

    constexpr XMFLOAT3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {}
    explicit XMFLOAT3(const float* pArray) noexcept : x(pArray[0]), y(pArray[1]), z(pArray[2]) {}

    // Opérateurs mathématiques
    XMFLOAT3 operator+(const XMFLOAT3& other) const {
        return XMFLOAT3(x + other.x, y + other.y, z + other.z);
    }
    
    XMFLOAT3 operator-(const XMFLOAT3& other) const {
        return XMFLOAT3(x - other.x, y - other.y, z - other.z);
    }
    
    XMFLOAT3 operator*(float scalar) const {
        return XMFLOAT3(x * scalar, y * scalar, z * scalar);
    }
    
    XMFLOAT3& operator+=(const XMFLOAT3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    XMFLOAT3& operator-=(const XMFLOAT3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    void Normalize() {
        float len = Length();
        if (len > 0.0001f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
    
    XMFLOAT3 Normalized() const {
        XMFLOAT3 result = *this;
        result.Normalize();
        return result;
    }
    
    float Dot(const XMFLOAT3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    XMFLOAT3 Cross(const XMFLOAT3& other) const {
        return XMFLOAT3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

// ============================================================================
// STRUCTURE OBB (Oriented Bounding Box)
// ============================================================================

struct cpu_obb
{
    XMFLOAT3 center;
    XMFLOAT3 extents;
    XMFLOAT3 axisX;
    XMFLOAT3 axisY;
    XMFLOAT3 axisZ;

    cpu_obb() : center(0, 0, 0), extents(0, 0, 0), 
                axisX(1, 0, 0), axisY(0, 1, 0), axisZ(0, 0, 1) {}
};

// ============================================================================
// STRUCTURE RECTANGLE
// ============================================================================

struct cpu_rectangle
{
    int x, y, width, height;
    
    cpu_rectangle() : x(0), y(0), width(0), height(0) {}
    cpu_rectangle(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h) {}
};

// ============================================================================
// STRUCTURE AABB (Axis-Aligned Bounding Box)
// ============================================================================

struct cpu_aabb
{
    XMFLOAT3 min;
    XMFLOAT3 max;

    cpu_aabb() : min(0, 0, 0), max(0, 0, 0) {}
    cpu_aabb(const XMFLOAT3& _min, const XMFLOAT3& _max) : min(_min), max(_max) {}

    cpu_aabb& operator=(const cpu_obb& obb) {
        // Conversion simple OBB -> AABB
        min = obb.center - obb.extents;
        max = obb.center + obb.extents;
        return *this;
    }

    void Zero() {
        min = XMFLOAT3(0, 0, 0);
        max = XMFLOAT3(0, 0, 0);
    }

    bool Contains(const XMFLOAT3& p) const {
        return (p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y &&
                p.z >= min.z && p.z <= max.z);
    }

    bool ToScreen(cpu_rectangle& out, const float* wvp, float renderWidth, float renderHeight) {
        // Implémentation simplifiée pour la projection
        // Cette fonction nécessiterait une vraie matrice de projection
        return false;
    }
};

// ============================================================================
// STRUCTURE TRANSFORM
// ============================================================================

struct cpu_transform
{
    XMFLOAT3 pos;      // Position
    XMFLOAT3 dir;      // Direction (forward)
    XMFLOAT3 up;       // Up vector
    XMFLOAT3 right;    // Right vector
    
    cpu_transform() {
        Identity();
    }
    
    void Identity() {
        pos = XMFLOAT3(0, 0, 0);
        dir = XMFLOAT3(0, 0, 1);    // Forward
        up = XMFLOAT3(0, 1, 0);     // Up
        right = XMFLOAT3(1, 0, 0);  // Right
    }
    
    void SetYPR(float yaw, float pitch, float roll) {
        // Réplication de la logique cpu_transform::AddYPR
        // Basé sur les rotations par axes comme dans le vrai code
        
        // Commencer avec une base orthonormale
        dir = XMFLOAT3(0.0f, 0.0f, 1.0f);    // Forward
        right = XMFLOAT3(1.0f, 0.0f, 0.0f);  // Right
        up = XMFLOAT3(0.0f, 1.0f, 0.0f);     // Up
        
        // Construire la matrice de rotation en appliquant yaw, pitch, roll dans l'ordre
        // Comme dans AddYPR: Roll autour de dir, Pitch autour de right, Yaw autour de up
        
        // Matrice de rotation Yaw (autour de Y/up)
        float cy = std::cos(yaw);
        float sy = std::sin(yaw);
        float cp = std::cos(pitch);
        float sp = std::sin(pitch);
        float cr = std::cos(roll);
        float sr = std::sin(roll);
        
        // Matrice combinée YPR (Yaw * Pitch * Roll)
        // Dans DirectX: Right = première ligne, Up = deuxième ligne, Dir = troisième ligne
        
        // Calculer la matrice de rotation complète (ordre: Yaw puis Pitch puis Roll)
        // Right vector (première ligne de la matrice)
        right.x = cy * cr + sy * sp * sr;
        right.y = cp * sr;
        right.z = -sy * cr + cy * sp * sr;
        
        // Up vector (deuxième ligne de la matrice)
        up.x = -cy * sr + sy * sp * cr;
        up.y = cp * cr;
        up.z = sy * sr + cy * sp * cr;
        
        // Dir/Forward vector (troisième ligne de la matrice)
        dir.x = sy * cp;
        dir.y = -sp;
        dir.z = cy * cp;
        
        // Normaliser pour éviter les erreurs d'accumulation
        right.Normalize();
        up.Normalize();
        dir.Normalize();
    }
    
    void UpdateWorld() {
        XMFLOAT3 worldUp(0, 1, 0);

        dir.Normalize();

        // CORRECTION ICI ⬇
        right = worldUp.Cross(dir);
        right.Normalize();

        up = dir.Cross(right);
        up.Normalize();
    }
    
    void OrbitAroundAxis(const XMFLOAT3& center, const XMFLOAT3& axis, float radius, float angle) {
        // Rotation autour d'un axe
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        XMFLOAT3 offset = pos - center;
        
        // Rotation simplifiée autour de l'axe Y (comme dans le code original)
        if (std::abs(axis.y - 1.0f) < 0.001f) {
            float newX = offset.x * cosA - offset.z * sinA;
            float newZ = offset.x * sinA + offset.z * cosA;
            pos.x = center.x + newX;
            pos.z = center.z + newZ;
        }
    }
};

// ============================================================================
// NAMESPACE CPU - FONCTIONS UTILITAIRES
// ============================================================================

namespace cpu
{
    // Test de collision AABB vs AABB
    inline bool AabbAabb(const cpu_aabb& a, const cpu_aabb& b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
               (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
               (a.min.z <= b.max.z && a.max.z >= b.min.z);
    }
    
    // Distance entre deux points
    inline float Distance(const XMFLOAT3& a, const XMFLOAT3& b) {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    // Interpolation linéaire
    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    inline XMFLOAT3 Lerp(const XMFLOAT3& a, const XMFLOAT3& b, float t) {
        return XMFLOAT3(
            Lerp(a.x, b.x, t),
            Lerp(a.y, b.y, t),
            Lerp(a.z, b.z, t)
        );
    }
    
    // Clamp
    inline float Clamp(float value, float min, float max) {
        return std::max(min, std::min(max, value));
    }
}

// ============================================================================
// FONCTIONS UTILITAIRES
// ============================================================================

inline int randomBetweenMinus25And25() 
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(-25, 25);
    return dist(gen);
}

inline float randomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

// Compatibilité avec les macros DirectX
#ifndef FXMMATRIX
#define FXMMATRIX const float*
#endif

#ifndef XM_CALLCONV
#define XM_CALLCONV
#endif
