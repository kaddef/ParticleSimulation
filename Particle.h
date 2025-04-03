//
// Created by erkan on 4.03.2025.
//

#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib.h"

class Particle {
private:
    // State
    int id{};
    Vector2 position{};
    Vector2 lastPosition{};
    Vector2 acceleration{};
    const float radius = 10.0f;
    Color color{};

public:
    // Constructors
    Particle() = default;
    Particle(Vector2 position, float radius, Color color, int Id);

    // Setters
    void SetVelocity(Vector2 velocity, float deltaTime);
    void SetPosition(Vector2 position);
    void SetColor(Color color);

    // Getter
    Vector2 GetVelocity() const;
    Vector2 GetPosition() const;
    Vector2 GetLastPosition() const;
    Vector2 GetAcceleration() const;
    float GetRadius() const;
    int GetId();

    // Update Render and
    void Update(float deltaTime);
    void Render();
    void Accelerate(Vector2 acceleration);
    void AddVelocity(Vector2 velocity, float deltaTime);

    //Spatial Partitioning
    int gridX, gridY; //SOMEHOW I CANNOT INCLUDE KEY.H HERE IT CREATES AN ERROR
};



#endif //PARTICLE_H
