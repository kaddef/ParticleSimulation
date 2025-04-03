//
// Created by erkan on 4.03.2025.
//

#include "Particle.h"
#include "raymath.h"

// Constructors
Particle::Particle(const Vector2 position, float radius, Color color, int id) :
    id(id),
    position(position),
    lastPosition(position),
    acceleration({0.0f, 0.0f}),
    radius(radius),
    color(color)
{}

// Setters
void Particle::SetVelocity(const Vector2 velocity, const float deltaTime) {
    this->lastPosition = Vector2Subtract(this->position, Vector2Scale(velocity, deltaTime));
}

void Particle::SetPosition(const Vector2 position) {
    this->position = position;
}

void Particle::SetColor(Color color) {
    this->color = color;
}

// Getter
Vector2 Particle::GetVelocity() const {
    return Vector2Subtract(position, lastPosition);
}

Vector2 Particle::GetPosition() const {
    return position;
}

Vector2 Particle::GetLastPosition() const {
    return lastPosition;
}

Vector2 Particle::GetAcceleration() const {
    return acceleration;
}

float Particle::GetRadius() const {
    return radius;
}

int Particle::GetId() {
    return id;
}

// Update, Render, and others
void Particle::Update(const float deltaTime) {
    Vector2 velocity = Vector2Subtract(position, lastPosition);
    lastPosition = position;
    velocity = Vector2Add(velocity, Vector2Scale(acceleration, deltaTime * deltaTime));
    position = Vector2Add(position, velocity);

    acceleration.x = 0;
    acceleration.y = 0;
}

void Particle::Render() {
    // int id = GetId();
    DrawCircleV(position, radius, color);
    // DrawText(TextFormat("%d", id), position.x - radius / 2, position.y - radius / 2, 10, WHITE);
}

void Particle::Accelerate(const Vector2 acceleration) {
    this->acceleration = Vector2Add(this->acceleration, acceleration);
}

void Particle::AddVelocity(const Vector2 velocity, const float deltaTime) {
    this->lastPosition = Vector2Subtract(this->lastPosition, Vector2Scale(velocity, deltaTime));
}