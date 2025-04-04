#include <iostream>
#include <chrono>
#include <string>
#include "raylib.h"
#include "Solver.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Simulation");
    // Init
    Solver solver;
    // ThreadPool threadPool(4);
    // ThreadedSolver solver(threadPool);

    solver.SetBoundary({400,400}, 400.00);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (!solver.spawnComplete) {
            solver.InitiateParticles(1000, false);
        }

        solver.Update();
        BeginDrawing();
        DrawFPS(10,10);
        DrawText(("P. Count " + std::to_string(solver.GetParticleCount())).c_str(), 10, 30, 20, LIME);
        ClearBackground(GRAY);
        solver.DrawBoundary();
        solver.DrawParticles();

        // solver.DrawDebugLines();
        EndDrawing();
    }

    // De-Initialization
    // threadPool.~ThreadPool();
    // solver.~ThreadedSolver();
    CloseWindow();
    return 0;
}
