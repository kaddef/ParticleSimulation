//
// Created by erkan on 4.03.2025.
//

#include "Solver.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <ostream>

#include "raymath.h"

Particle &Solver::AddParticle(Vector2 position, float radius, Color color) {
    return this->particles.emplace_back(position, radius, color, static_cast<int>(particles.size()));
}

void Solver::InitiateParticles(int maxCount, bool isDrawingImage) {
    if (spawnComplete) return;

    if (isDrawingImage && !colorsLoaded) {
        LoadColorsFromFile();
    }

    if (GetParticleCount() < maxCount && spawnClock > spawnDelay) {
        if (isDrawingImage) {
            // const float radius = 2.0f + static_cast<float>(rand() % 4);
            const float radius = 10.0f;
            Particle& particle = AddParticle({450,150}, radius, colorQueue[colorQueueIndex]);
            AddParticleToGrid(&particle);
            spawnClock = 0;

            const float angle = PI * 0.5f + PI / 2 * sin(spawnAngle);
            spawnAngle += 0.1f;
            SetObjectVelocity(particle, Vector2Scale({cos(angle), sin(angle)}, 800));
            colorQueueIndex += 1;
        } else {
            // const float radius = 2.0f + static_cast<float>(rand() % 4);
            const float radius = 10.0f;
            Particle& particle = AddParticle({450,150}, radius, RED);
            AddParticleToGrid(&particle);
            spawnClock = 0;

            const float angle = PI * 0.5f + PI / 2 * sin(spawnAngle);
            spawnAngle += 0.1f;
            SetObjectVelocity(particle, Vector2Scale({cos(angle), sin(angle)}, 800));
        }
    }

    spawnClock += GetFrameTime();

    if (GetParticleCount() >= maxCount /* && GetFrameTime() < 0.01667*/) {
        spawnComplete = true;
        spawnClock, spawnAngle, spawnDelay = 0;
        if (isDrawingImage) {
            colorQueue.clear();
        }
    }
}

void Solver::ApplyGravity() {
    for (Particle &particle : this->particles) {
        particle.Accelerate(gravity);
    }
}

void Solver::UpdateObjects(float dt) {
    for (Particle &particle : this->particles) {
        particle.Update(dt);
        UpdateParticleInGrid(&particle, particle.GetPosition());
    }
}

void Solver::Update() {
    float substepDt = dt/substeps;
    for (int i = 0; i < substeps; i++) {
        if (IsKeyDown(KEY_SPACE)) GetCurrentState();
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) SetGravity({0.0f,-1000.0f});
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) SetGravity({0.0f,1000.0f});;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) SetGravity({-1000.0f,0.0f});
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) SetGravity({1000.0f,0.0f});
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) MousePull(GetMousePosition());
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) MousePush(GetMousePosition());
        this->ApplyGravity();
        this->BoundaryCheck();
        // this->SolveCollision();
        this->SpatialCollision();
        this->UpdateObjects(substepDt);
    }

}

void Solver::BoundaryCheck() {
    for (Particle &particle : particles) {
        float dampening = 0.99f;
        float distance = Vector2Distance(particle.GetPosition(), boundary.position);

        if (distance + particle.GetRadius() > boundary.radius) {
            float overflow = distance + particle.GetRadius() - boundary.radius;

            Vector2 direction = Vector2Normalize(
                Vector2Subtract(boundary.position, particle.GetPosition())
            );

            Vector2 newPosition = Vector2Add(particle.GetPosition(), Vector2Scale(direction, overflow));
            Vector2 newVelocity = Vector2Reflect(particle.GetVelocity(), direction);
            newVelocity = Vector2Scale(newVelocity, dampening);
            particle.SetPosition(newPosition);
            particle.SetVelocity(newVelocity, 1.0f);
        }
    }
}

void Solver::SolveCollision() {
    const unsigned int size = particles.size();

    for (int i = 0; i < size; i++) {
        Particle& particle1 = particles[i];
        for (int j = i+1; j < size; j++) {
            Particle& particle2 = particles[j];
            const Vector2 collisionAxis = Vector2Subtract(particle1.GetPosition(), particle2.GetPosition());
            const float distance = Vector2Distance(particle1.GetPosition(), particle2.GetPosition());
            const float centerToCenter = particle1.GetRadius() + particle2.GetRadius();
            if (distance < centerToCenter) {
                const float overlap = std::abs(centerToCenter - distance); // Just in case abs
                const Vector2 normal = Vector2Normalize(collisionAxis);
                const Vector2 particle1Pullback = Vector2Scale(normal, overlap*(particle1.GetRadius()/(particle1.GetRadius() + particle2.GetRadius())));
                const Vector2 particle2Pullback = Vector2Scale(normal, -overlap*(particle1.GetRadius()/(particle1.GetRadius() + particle2.GetRadius())));

                particle1.SetPosition(Vector2Add(particle1.GetPosition(),particle1Pullback));
                particle2.SetPosition(Vector2Add(particle2.GetPosition(),particle2Pullback));
            }
        }
    }
}


void Solver::DrawParticles() {
    for (Particle &particle : this->particles) {
        particle.Render();
    }
}

void Solver::DrawBoundary() {
    DrawCircleV(boundary.position, boundary.radius, BLACK);
}

void Solver::SetBoundary(const Vector2 position, const float radius) {
    this->boundary.position = position;
    this->boundary.radius = radius;
}

void Solver::SetObjectVelocity(Particle& object, Vector2 vel) {
    object.SetVelocity(vel, dt / substeps);
}

int Solver::GetParticleCount() const {
    return static_cast<int>(this->particles.size());
}

void Solver::MousePull(Vector2 position) {
    for (Particle &particle : particles) {
        Vector2 toMouse = Vector2Subtract(position, particle.GetPosition());
        float distance = Vector2Length(toMouse);

        if (distance < influenceRadius) {
            float strength = mouseStrength; // (1.0f + distance * 0.1f);
            Vector2 direction = Vector2Normalize(toMouse);  // Direction TO mouse
            Vector2 force = Vector2Scale(direction, strength);
            particle.Accelerate(force);
        }
    }
}

void Solver::MousePush(Vector2 position) {
    for (Particle &particle : particles) {
        Vector2 fromMouse = Vector2Subtract(particle.GetPosition(), position);
        float distance = Vector2Length(fromMouse);

        if (distance < influenceRadius) {
            float strength = mouseStrength; // (1.0f + distance * 0.1f);
            Vector2 direction = Vector2Normalize(fromMouse);  // Direction AWAY from mouse
            Vector2 force = Vector2Scale(direction, strength);

            particle.Accelerate(force);
        }
    }
}

void Solver::SetGravity(Vector2 gravity) {
    this->gravity = gravity;
}

void Solver::GetCurrentState() {
    std::ofstream fout("../output.txt");
    if (!fout.is_open()) {
        std::cout << "Error opening output.txt for writing!" << std::endl;
        return;
    }

    for (const Particle &particle : particles) {
        fout << particle.GetPosition().x << " " << particle.GetPosition().y << std::endl;
    }
}

void Solver::LoadColorsFromFile() {
    std::ifstream RgbColorFile(colorFilePath);
    if (!RgbColorFile) {
        std::cout << "Error opening file: " << colorFilePath << std::endl;
        exit(1);
    }

    std::string line;
    int r, g, b;

    while (getline(RgbColorFile, line)) {
        std::stringstream ss(line);
        if (!(ss >> r >> g >> b)) {
            std::cerr << "Invalid color format in line: " << line << std::endl;
            continue;
        }
        colorQueue.emplace_back(r,g,b,255);
    }

    colorsLoaded = true;
}

//Spatial Partitioning
Key Solver::GetKey(float x, float y) {
    return Key(
        static_cast<int>(x / cellSize),
        static_cast<int>(y / cellSize)
    );
}

Key Solver::CreateKey(int gridX, int gridY) {
    return Key(gridX, gridY);
}

void Solver::AddParticleToGrid(Particle *p) {
    Vector2 pos = p->GetPosition();
    Key key = GetKey(pos.x, pos.y);
    grid[key].push_back(p->GetId());

    p->gridX = key.x;
    p->gridY = key.y;
}

void Solver::UpdateParticleInGrid(Particle *p, Vector2 newPos) {
    Key currCell = CreateKey(p->gridX,p->gridY);
    Key newCell = GetKey(newPos.x, newPos.y);

    if (currCell != newCell) {
        auto& oldCellParticles = grid[currCell];
        std::erase(oldCellParticles, p->GetId());

        grid[newCell].push_back(p->GetId());
        // int index = (newCell.y) * GetScreenWidth()/cellSize + (newCell.x);
        // p->SetColor(colors[index%(sizeof(colors)/sizeof(colors[0]))]);
        p->gridX = newCell.x;
        p->gridY = newCell.y;
    }
}

void Solver::DrawDebugLines() {
    for (int x = 0; x < GetScreenWidth(); x+=cellSize) {
        for (int y = 0; y < GetScreenHeight(); y+=cellSize) {
            DrawLineV({0.0f,(float)y}, {(float)GetScreenWidth(), (float)y}, PURPLE);

            int index = (y / cellSize) * GetScreenWidth()/cellSize + (x / cellSize);
            // Draw the cell with its persistent color
            DrawRectangle(x ,y , 15, 15, colors[index%(sizeof(colors)/sizeof(colors[0]))]);
            DrawText(TextFormat("%d",index), x,y,12,WHITE);
        }
        DrawLineV({(float)x,0.0f}, {(float)x, (float)GetScreenHeight()}, PURPLE);
    }
}

void Solver::SpatialCollision() {
    int cellCountInRow = GetScreenWidth() / cellSize;
    int cellCountInColumn = GetScreenHeight() / cellSize;
    for (int x = 0; x < cellCountInRow; x++) {
        for (int y = 0; y < cellCountInColumn; y++) {
            Key key(x,y);
            auto it = grid.find(key);
            if (it == grid.end() || it->second.empty()) continue;

            std::vector<Key> neighborCells = GetNeighboringCells(key);

            for (Key &cellKey : neighborCells) {
                CollideCells(key, cellKey);
            }
        }
    }
}


void Solver::CollideCells(Key key1, Key key2) {
    for (int& id1: grid[key1]) {
        Particle& particle1 = particles[id1];
        for (int& id2: grid[key2]) {
            if (id1 == id2) continue;
            Particle& particle2 = particles[id2];

            // This is copy from brute force collider fom above maybe put it in seperate func
            const Vector2 collisionAxis = Vector2Subtract(particle1.GetPosition(), particle2.GetPosition());
            const float distance = Vector2Distance(particle1.GetPosition(), particle2.GetPosition());
            const float centerToCenter = particle1.GetRadius() + particle2.GetRadius();
            if (distance < centerToCenter) {
                const float overlap = std::abs(centerToCenter - distance); // Just in case abs
                const Vector2 normal = Vector2Normalize(collisionAxis);
                const Vector2 particle1Pullback = Vector2Scale(normal, overlap*(particle1.GetRadius()/(particle1.GetRadius() + particle2.GetRadius())));
                const Vector2 particle2Pullback = Vector2Scale(normal, -overlap*(particle1.GetRadius()/(particle1.GetRadius() + particle2.GetRadius())));

                particle1.SetPosition(Vector2Add(particle1.GetPosition(),particle1Pullback));
                particle2.SetPosition(Vector2Add(particle2.GetPosition(),particle2Pullback));
            }
        }
    }
}

std::vector<Key> Solver::GetNeighboringCells(Key& key) {
    int cellCountInRow = GetScreenWidth() / cellSize;
    std::vector<Key> neighborCells;
    neighborCells.reserve(9);
    int dx[] = {1, 1, 0, 0, -1};
    int dy[] = {0, 1, 0, 1, 1};

    for (int i = 0; i < 5; i++) {
        int nx = key.x + dx[i], ny = key.y + dy[i];
        if (nx < 0 || ny < 0 || nx >= cellCountInRow || ny >= cellCountInRow) continue;
        neighborCells.emplace_back(nx, ny);
    }

    return neighborCells;
}