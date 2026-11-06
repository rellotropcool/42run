#include "game/actors/Entity.h"

#include "gfx/Mesh.h"

Entity::Entity(Mesh& mesh, const Vec3& position,
               const Vec3& rotation, const Vec3& scale)
    : _mesh(&mesh), _position(position), _rotation(rotation), _scale(scale) {}

Mesh* Entity::mesh() const { return _mesh; }
void  Entity::setMesh(Mesh& mesh) { _mesh = &mesh; }

const Vec3& Entity::position() const           { return _position; }
void        Entity::setPosition(const Vec3& p) { _position = p; }

const Vec3& Entity::rotation() const           { return _rotation; }
void        Entity::setRotation(const Vec3& r) { _rotation = r; }

const Vec3& Entity::scale() const              { return _scale; }
void        Entity::setScale(const Vec3& s)    { _scale = s; }