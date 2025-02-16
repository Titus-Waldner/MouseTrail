// include/particles.h
#pragma once

#include <vector>
#include <windows.h>
#include <chrono>

#define MAX_PARTICLES 5000 

enum class ParticleType {
    HEARTS = 1,
    STARS,
    FIRE,
    SPARKS,
    SMOKE,
	SWORD
};

// Particle struct
struct Particle {
    float x, y;          // Position
    float vx, vy;        // Velocity
    float life;          // Remaining life (seconds)
    float maxLife;       // Maximum life (seconds)
    COLORREF color;      // Color
    float angle;         // Rotation angle (radians)
    float rotationSpeed; // Rotation speed
    float scale;         // Scale
    ParticleType type;   // Which system does this particle belong to?
};

// Globals
extern std::vector<Particle> g_particles;
extern ParticleType g_activeParticleSystem;
extern POINT g_lastMousePos;
extern std::chrono::steady_clock::time_point g_lastFrameTime;

// Particle system functions
void SpawnParticlesOnMouseMove();  // decides which spawn function to call
void SpawnHeartsOnMouseMove();
void SpawnStarsOnMouseMove();
void SpawnFireOnMouseMove();
void SpawnSparksOnMouseMove();
void SpawnSmokeOnMouseMove();
void SpawnSwordOnMouseMove();

void UpdateParticles(float dt);
void DrawParticlesToDIB();

// Let external code select the particle system
void SetActiveParticleSystem(int systemId);
