#ifndef ENTITY_H
#define ENTITY_H

#include "math/Vec3.h"

class Mesh;

class Entity {
    private:
        Mesh* _mesh     = nullptr;
        Vec3  _position;
        Vec3  _rotation;
        Vec3  _scale    = Vec3(1.0f);
    public:
        Entity() = default;
        Entity(Mesh& mesh, const Vec3& position,
               const Vec3& rotation = Vec3(0.0f),
               const Vec3& scale    = Vec3(1.0f));

        Mesh* mesh() const;
        void  setMesh(Mesh& mesh);

        const Vec3& position() const;
        void        setPosition(const Vec3& position);

        const Vec3& rotation() const;
        void        setRotation(const Vec3& rotation);

        const Vec3& scale() const;
        void        setScale(const Vec3& scale);
};

#endif