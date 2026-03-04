// Created by egors on 29.11.2025.
//

#ifndef DUMPENGINE_TYPES_H
#define DUMPENGINE_TYPES_H

#include <glm/glm.hpp>
#include <map>

#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct DMaterial;
class DBaseMesh;
// --- Вспомогательные функции ---
inline float DegreesToRadians(float degrees) {
    return degrees * static_cast<float>(M_PI) / 180.0f;
}

inline float RadiansToDegrees(float radians) {
    return radians * 180.0f / static_cast<float>(M_PI);
}



struct RenderCommand {
    const DBaseMesh* mesh;
    const DMaterial* material;
    glm::mat4        modelMatrix;
};


struct DVector {
    float x;
    float y;
    float z;

    DVector() : x(0.0f), y(0.0f), z(0.0f) {
    }

    DVector(float InX, float InY, float InZ) : x(InX), y(InY), z(InZ) {
    }

    // --- Операторы ---
    DVector operator+(const DVector &other) const { return DVector(x + other.x, y + other.y, z + other.z); }
    DVector operator-(const DVector &other) const { return DVector(x - other.x, y - other.y, z - other.z); }
    DVector operator*(float scalar) const { return DVector(x * scalar, y * scalar, z * scalar); }
    DVector operator/(float scalar) const { return DVector(x / scalar, y / scalar, z / scalar); }
    DVector operator-() const { return DVector(-x, -y, -z); }

    DVector operator*(const DVector &other) const { return DVector(x * other.x, y * other.y, z * other.z); }

    DVector &operator+=(const DVector &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    DVector &operator-=(const DVector &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    DVector &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    DVector &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // --- Методы ---
    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSquared() const { return x * x + y * y + z * z; }

    void Normalize() {
        float len = Length();
        if (len > 1e-6f) { *this /= len; }
    }

    DVector Normalized() const {
        DVector result = *this;
        result.Normalize();
        return result;
    }

    float Dot(const DVector &other) const { return x * other.x + y * other.y + z * other.z; }

    DVector Cross(const DVector &other) const {
        return DVector(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    // --- Статические константы ---
    static DVector Zero() { return DVector(0.0f, 0.0f, 0.0f); }
    static DVector One() { return DVector(1.0f, 1.0f, 1.0f); }
    static DVector Up() { return DVector(0.0f, 1.0f, 0.0f); }
    static DVector Down() { return DVector(0.0f, -1.0f, 0.0f); }
    static DVector Right() { return DVector(1.0f, 0.0f, 0.0f); }
    static DVector Left() { return DVector(-1.0f, 0.0f, 0.0f); }
    static DVector Forward() { return DVector(0.0f, 0.0f, -1.0f); }
    static DVector Back() { return DVector(0.0f, 0.0f, 1.0f); }

    glm::vec3 ToGLM() const {
        return glm::vec3(x, y, z);
    }
};

// Внешние операторы и утилиты для DVector
inline DVector operator*(float scalar, const DVector &vector) { return vector * scalar; }

inline std::ostream &operator<<(std::ostream &os, const DVector &v) {
    os << "DVector(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}


// УДАЛЕНО: DRotator. Используем DQuaternion для представления вращений.
// DRotator можно оставить как удобную структуру для ввода в редакторе,
// но все внутренние вычисления должны производиться с кватернионами.
struct DRotator {
    float Pitch;
    float Yaw;
    float Roll;

    DRotator(float p = 0.f, float y = 0.f, float r = 0.f) : Pitch(p), Yaw(y), Roll(r) {
    }
};


// НОВОЕ: DQuaternion - математически корректный способ работать с вращениями
struct DQuaternion {
    float x, y, z, w;

    DQuaternion() : x(0.f), y(0.f), z(0.f), w(1.f) {
    } // Единичный кватернион (нет вращения)
    DQuaternion(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {
    }

    DQuaternion(const glm::quat &q) {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
    }

    // Конструктор из углов Эйлера (DRotator)
    DQuaternion(const DRotator &rot) {
        float radPitch = DegreesToRadians(rot.Pitch * 0.5f);
        float radYaw = DegreesToRadians(rot.Yaw * 0.5f);
        float radRoll = DegreesToRadians(rot.Roll * 0.5f);

        float cP = std::cos(radPitch), sP = std::sin(radPitch);
        float cY = std::cos(radYaw), sY = std::sin(radYaw);
        float cR = std::cos(radRoll), sR = std::sin(radRoll);

        w = cR * cP * cY + sR * sP * sY;
        x = sR * cP * cY - cR * sP * sY;
        y = cR * sP * cY + sR * cP * sY;
        z = cR * cP * sY - sR * sP * cY;
    }

    // Композиция вращений (умножение кватернионов)
    DQuaternion operator*(const DQuaternion &other) const {
        return DQuaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }

    // Вращение вектора
    DVector RotateVector(const DVector &v) const {
        DQuaternion p(v.x, v.y, v.z, 0);
        DQuaternion q_conj(-x, -y, -z, w);
        DQuaternion result = *this * p * q_conj;
        return DVector(result.x, result.y, result.z);
    }

    glm::quat ToGLM() const {
        // Внимание: конструктор glm::quat принимает (w, x, y, z)
        return glm::quat(w, x, y, z);
    }

    // === [НОВОЕ] Получение матрицы вращения 4x4 ===
    glm::mat4 ToMatrix() const {
        return glm::mat4_cast(ToGLM());
    }
};


// --- DTransform ---
struct DTransform {
    DVector Position;
    DQuaternion Rotation; // ИЗМЕНЕНО: Используем кватернион
    DVector Scale;

    DTransform() : Position(DVector::Zero()), Rotation(), Scale(DVector::One()) {
    }

    // ИЗМЕНЕНО: Конструктор теперь может принимать DRotator для удобства
    DTransform(const DVector &pos, const DRotator &rot, const DVector &scl)
        : Position(pos), Rotation(rot), Scale(scl) {
    }

    DTransform(const DVector &pos, const DQuaternion &rot, const DVector &scl)
        : Position(pos), Rotation(rot), Scale(scl) {
    }

    static DTransform Identity() { return DTransform(); }

    // НОВОЕ: Трансформация точки из локального пространства в мировое
    DVector TransformPosition(const DVector &P) const {
        return Rotation.RotateVector(P * Scale) + Position;
    }

    // ИЗМЕНЕНО: Полностью переписанный и корректный оператор умножения
    DTransform operator*(const DTransform &other) const {
        DTransform result;

        // Комбинируем вращение: ParentRotation * ChildRotation
        result.Rotation = this->Rotation * other.Rotation;

        // Комбинируем масштаб: ParentScale * ChildScale
        result.Scale = this->Scale * other.Scale;

        // Комбинируем позицию: ParentPos + ParentRot.Rotate(ParentScale * ChildPos)
        result.Position = this->TransformPosition(other.Position);

        return result;
    }

    glm::mat4 ToMatrix() const {
        // 1. Создаем единичную матрицу
        glm::mat4 model = glm::mat4(1.0f);

        // 2. Translate (Перемещение)
        model = glm::translate(model, Position.ToGLM());

        // 3. Rotate (Вращение)
        // Умножаем на матрицу вращения, полученную из кватерниона
        model = model * Rotation.ToMatrix();

        // 4. Scale (Масштаб)
        model = glm::scale(model, Scale.ToGLM());

        return model;
    }

    // --- Удобные геттеры для векторов направления ---
    DVector GetForwardVector() const { return Rotation.RotateVector(DVector::Forward()); }
    DVector GetRightVector() const { return Rotation.RotateVector(DVector::Right()); }
    DVector GetUpVector() const { return Rotation.RotateVector(DVector::Up()); }
};


#endif //DUMPENGINE_TYPES_H
