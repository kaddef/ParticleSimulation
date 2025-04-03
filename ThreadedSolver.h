//
// Created by erkan on 1.04.2025.
//

#ifndef THREADEDSOLVER_H
#define THREADEDSOLVER_H
#include <string>
#include <unordered_map>
#include <vector>

#include "Boundary.h"
#include "Particle.h"
#include "Key.h"
#include "ThreadPool.h"

// Struct for Boundary
// struct Boundary {
//     Vector2 position;
//     float radius;
// };

class ThreadedSolver {
private:
    std::vector<Particle> particles;
    ThreadPool& threadPool;
    std::mutex gridAccesMutex;
    Vector2 gravity = {0.0f, 1000.0f};
    float dt = 1.0f / 60.0f;
    int substeps = 6;
    const float influenceRadius = 100.0f;
    const float mouseStrength = 2000.0f;
    Boundary boundary;

    void ApplyGravity();
    void UpdateObjects(float dt);
    //
    void ParticleBoundaryCheck(int id);
    void ThreadedBoundaryCheck();
    //
    void ParticleUpdate(int id, float dt);
    void ThreadedUpdate(float dt);
    void SolveCollision();
public:
    ThreadedSolver(ThreadPool& threadPool);
    ~ThreadedSolver();
    Particle& AddParticle(Vector2 position, float radius, Color color);
    void Update();
    void DrawParticles(); // Move Drawing from here
    void DrawBoundary();
    int GetParticleCount() const;
    void SetBoundary(Vector2 position, float radius);
    void SetObjectVelocity(Particle& object, Vector2 vel);
    void SetGravity(Vector2 gravity);
    void MousePull(Vector2 position);
    void MousePush(Vector2 position);
    void InitiateParticles(int maxCount, bool isDrawingImage);

    //Functions and variables about drawing image
    double spawnClock = 0;
    double spawnAngle = 0;
    double spawnDelay = 0.05f;
    bool colorsLoaded = false;
    bool spawnComplete = false;
    int colorQueueIndex = 0;
    std::vector<Color> colorQueue;
    std::string pCoordFilePath = "../output.txt";
    std::string colorFilePath = "../colors.txt";
    void GetCurrentState();
    void LoadColorsFromFile();

    //Spatial Partitioning
    int cellSize = 20;
    std::unordered_map<Key, std::vector<int>, KeyHasher> grid;
    Color colors [16] = {RED, GRAY, LIME, BLUE, PURPLE, BEIGE, BROWN, MAGENTA, YELLOW, DARKGREEN, DARKBLUE, PINK, RED, GRAY, LIME, PURPLE};
    Key GetKey(float x, float y);
    Key CreateKey(int gridX, int gridY);
    void AddParticleToGrid(Particle* p);
    void UpdateParticleInGrid(Particle *p, Vector2 newPos);
    void DrawDebugLines();
    void CollideCells(Key key1, Key key2);
    void SpatialCollision();
    std::vector<Key> GetNeighboringCells(Key& key);
};



#endif //THREADEDSOLVER_H
