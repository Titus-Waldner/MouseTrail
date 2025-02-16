// src/particles.cpp
#include "particles.h"
#include "window.h"   // For g_ScreenWidth, g_ScreenHeight, g_pPixels
#include "utils.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>   // rand()

// Global Variables
std::vector<Particle> g_particles;
ParticleType g_activeParticleSystem = ParticleType::SMOKE;
POINT g_lastMousePos = { -1, -1 };
std::chrono::steady_clock::time_point g_lastFrameTime = std::chrono::steady_clock::now();

//---------------------------------------------------
// SetActiveParticleSystem
//  systemId: 1=Hearts, 2=Stars, 3=Fire, 4=Sparks, 5=Smoke 6=SWORD
//---------------------------------------------------
void SetActiveParticleSystem(int systemId)
{
    switch (systemId) {
        case 1: g_activeParticleSystem = ParticleType::SMOKE; break;
        case 2: g_activeParticleSystem = ParticleType::STARS;  break;
        case 3: g_activeParticleSystem = ParticleType::FIRE;   break;
        case 4: g_activeParticleSystem = ParticleType::SPARKS; break;
        case 5: g_activeParticleSystem = ParticleType::HEARTS;  break;
		case 6: g_activeParticleSystem = ParticleType::SWORD;  break;
        default:
            g_activeParticleSystem = ParticleType::SMOKE;
            break;
    }
}

//---------------------------------------------------
// SpawnParticlesOnMouseMove
//---------------------------------------------------
void SpawnParticlesOnMouseMove()
{
    switch (g_activeParticleSystem) {
        case ParticleType::SMOKE:  SpawnSmokeOnMouseMove(); break;
        case ParticleType::STARS:   SpawnStarsOnMouseMove();  break;
        case ParticleType::FIRE:    SpawnFireOnMouseMove();   break;
        case ParticleType::SPARKS:  SpawnSparksOnMouseMove(); break;
        case ParticleType::HEARTS:   SpawnHeartsOnMouseMove();  break;
		case ParticleType::SWORD:   SpawnSwordOnMouseMove();  break;
		
    }
}

//---------------------------------------------------
// Particle Rendering (Draw Functions)
//---------------------------------------------------
void DrawShape(const Particle& p, const unsigned char* mask, int width, int height);
void DrawStar(const Particle& p);
void DrawFire(const Particle& p);
void DrawSparks(const Particle& p);
void DrawSmoke(const Particle& p);
void DrawSword(const Particle& p);


//---------------------------------------------------
// Common function: calculates # of particles
//  and interpolates spawn positions along the mouse path
//---------------------------------------------------
static void SpawnParticlesCommon(ParticleType type,
                                 float distBetweenParticles,
                                 COLORREF (*colorFunc)(),
                                 float minLife, float maxLife,
                                 float scaleMin, float scaleMax,
                                 bool allowRotation,
                                 bool upwardVelocityBias = true)
{
    POINT pt;
    GetCursorPos(&pt);

    // If first time, just store the last pos
    if (g_lastMousePos.x == -1 && g_lastMousePos.y == -1) {
        g_lastMousePos = pt;
        return;
    }

    float dx = static_cast<float>(pt.x - g_lastMousePos.x);
    float dy = static_cast<float>(pt.y - g_lastMousePos.y);
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > 0.f) {
        int numParticles = std::max(1, static_cast<int>(dist / distBetweenParticles));
        for (int i = 0; i < numParticles; i++) {
            float t = (i + 1) / static_cast<float>(numParticles + 1);

            Particle p = {};
            p.x = g_lastMousePos.x + t * dx;
            p.y = g_lastMousePos.y + t * dy;

            // Random angle & speed
            float angle = ((rand() % 360) / 180.0f) * 3.14159f;
            float speed = 25.f + (rand() % 30); // default: 25..55
            if (!upwardVelocityBias) {
                // Use normal random velocity, no forced negative vy
                p.vx = speed * cosf(angle) * 0.5f;
                p.vy = speed * sinf(angle) * 0.5f;
            } else {
                // Negative vy for upward bias
                p.vx = speed * cosf(angle) * 0.5f;
                p.vy = -fabsf(speed * sinf(angle));
            }

            // Use color function
            if (colorFunc) {
                p.color = colorFunc();
            } else {
                // default to white if no colorFunc is specified
                p.color = 0xFFFFFF;
            }

            // Life
            float lifeRange = maxLife - minLife;
            float chosenLife = minLife + (rand() / (float)RAND_MAX) * lifeRange;
            p.maxLife = chosenLife;
            p.life    = chosenLife;

            // Scale
            float chosenScale = scaleMin + (rand() / (float)RAND_MAX) * (scaleMax - scaleMin);
            p.scale = chosenScale;

            // Rotation
            if (allowRotation) {
                p.angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
                // rotation speed from -3..3
                p.rotationSpeed = ((rand() % 601) - 300) / 100.0f;
            } else {
                p.angle = 0.f;
                p.rotationSpeed = 0.f;
            }

            p.type = type;
            g_particles.push_back(p);
        }
    }
    g_lastMousePos = pt;
}

//---------------------------------------------------
// 1) Hearts
//---------------------------------------------------
void SpawnHeartsOnMouseMove()
{


    auto colorFn = []() -> COLORREF {
        return RandomHeartColor(); 
    };

    POINT pt;
    GetCursorPos(&pt);

    // First time: Just store position, don't spawn yet
    if (g_lastMousePos.x == -1 && g_lastMousePos.y == -1) {
        g_lastMousePos = pt;
        return;
    }

    float dx = static_cast<float>(pt.x - g_lastMousePos.x);
    float dy = static_cast<float>(pt.y - g_lastMousePos.y);
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist > 5.0f) // Ensures movement before spawning
    {
        Particle p = {};
        p.x = pt.x;
        p.y = pt.y;

        // **Make each heart move fast & spread widely**
        float angle = ((rand() % 160) - 80) * (3.14159f / 180.0f); // Wide spread
        float speed = 150.f + (rand() % 50);  // **Much faster**
        p.vx = speed * cosf(angle);
        p.vy = -fabsf(speed * sinf(angle)); // Move upwards

        // **Prevent overlapping**
        p.x += (rand() % 50) - 30;  // Slight horizontal offset
        p.y += (rand() % 50) - 30;  // Slight vertical offset

        p.color = colorFn();
        p.maxLife = 0.9f; // **Longer lifespan**
        p.life = p.maxLife;
        p.scale = 1.0f + (rand() % 100) / 200.0f; // Slight variation in size
        p.angle = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
        p.rotationSpeed = ((rand() % 601) - 300) / 100.0f; 

        p.type = ParticleType::HEARTS;
        g_particles.push_back(p);
    }

    g_lastMousePos = pt;
}




//---------------------------------------------------
// 2) Stars
//---------------------------------------------------
void SpawnStarsOnMouseMove()
{
    // Sparkly star color
    auto colorFn = []() -> COLORREF {
        // White/yellowish
        return RGB(200 + rand() % 56, 200 + rand() % 56, 180 + rand() % 76);
    };

    // More distance => fewer stars
    // life ~0.7..1.0, scale ~1.5..2.5
    SpawnParticlesCommon(
        ParticleType::STARS,
        10.0f,
        colorFn,
        0.3f, 0.5f,
        0.5f, 1.5f,
        true,   // rotation
        true    // upward velocity bias
    );
}

//---------------------------------------------------
// 3) Fire
//---------------------------------------------------
void SpawnFireOnMouseMove()
{
    auto colorFn = []() -> COLORREF {
        int r = 200 + (rand() % 56);  // 200..255
        int g = 50 + (rand() % 80);   // 50..129
        int b = 0;                    // No blue
        return RGB(r, g, b);
    };

	SpawnParticlesCommon(
		ParticleType::FIRE,
		4.0f,  //  Spawn more fire particles per movement (Before: 4.0f)
		colorFn,
		0.3f, 0.5f,   //  Slightly longer lifespan
		1.0f, 1.1f,  //  MASSIVE FLAMES (Before: 5.0f, 9.0f)
		false,
		false
	);

}

//---------------------------------------------------
// 4) Sparks (Chaotic arcs)
//---------------------------------------------------
void SpawnSparksOnMouseMove()
{
    // Electric arcs: bright bluish/purple
    auto colorFn = []() -> COLORREF {
		int r = 0;   
        int g = 100 + (rand() % 56);
        int b = 200 + (rand() % 56);
                         
        return RGB(r, g, b);
    };

    // Sparks: short life, small scale but rotate quickly
    // We'll keep upwardVelocityBias = false for chaotic outward fling
    SpawnParticlesCommon(
        ParticleType::SPARKS,
        2.0f,
        colorFn,
        0.1f, 0.2f,  // short life
        1.0f, 2.0f,  // scale
        true,        // yes rotation
        false        // no upward bias
    );
}

//---------------------------------------------------
// 5) Smoke
//---------------------------------------------------
void SpawnSmokeOnMouseMove()
{
    // Smoke color: grayish
    auto colorFn = []() -> COLORREF {
        int shade = 100 + rand() % 100; // 100..199
        return RGB(shade, shade, shade);
    };

    // Smoke: bigger scale, medium life, no rotation
    // Upward velocity bias so it drifts upward
    SpawnParticlesCommon(
        ParticleType::SMOKE,
        10.0f,
        colorFn,
        0.3f, 0.5f,
        1.0f, 1.5f,   // bigger scale
        false,        // no rotation
        true          // upward velocity
    );
}

//---------------------------------------------------
// 6) Sword
//---------------------------------------------------
void SpawnSwordOnMouseMove()
{
    // Sword color:
	// Smoke color: grayish
    auto colorFn = []() -> COLORREF {
        int shade = 100;
        return RGB(shade, shade, shade);
    };
	 
    // Sword: bigger scale, medium life, no rotation
    // Upward velocity bias so it drifts upward
    SpawnParticlesCommon(
        ParticleType::SWORD,
        10.0f,
        colorFn,
        0.3f, 0.5f,
        0.5f, 0.5f,   // bigger scale
        true,        // no rotation
        true          // upward velocity
    );
}

//---------------------------------------------------
// UpdateParticles
//---------------------------------------------------
void UpdateParticles(float dt)
{
    for (auto &p : g_particles) {
        // Special behavior for hearts
        if (p.type == ParticleType::HEARTS) {
            // Increase speed to spread them out more
            p.vx *= 1.01f;  // Increase horizontal movement
            p.vy *= 1.03f;  // Increase vertical movement

            // Add slight random drift to make them float more naturally
            p.vx += (rand() % 5 - 2) * 0.05f;

            // Reduce gravity effect so they do not fall too fast
            p.vy -= 5.0f * dt;
        }

        // Update position
        p.x += p.vx * dt;
        p.y += p.vy * dt;

        // Apply gravity for other particles
        if (p.type != ParticleType::FIRE && p.type != ParticleType::HEARTS) {
            p.vy += 20.f * dt;
        }

        // Rotate particles unless they are fire or smoke
        if (p.type != ParticleType::FIRE && p.type != ParticleType::SMOKE) {
            p.angle += p.rotationSpeed * dt;
        }

        // Decrease life over time
        p.life -= dt;

        // Smooth fade-out effect by scaling down over time
        float ratio = (p.life > 0.f) ? (p.life / p.maxLife) : 0.f;
        float fadeFactor = 1.0f - powf(1.0f - ratio, 3.0f);  // Smooth fade effect
		p.scale = p.scale * fadeFactor;  // Scale relative to original size


    }

    // Remove expired particles
    g_particles.erase(
        std::remove_if(g_particles.begin(), g_particles.end(),
            [](const Particle &p){ return (p.life <= 0.f); }),
        g_particles.end()
    );
}



//---------------------------------------------------
// DrawParticlesToDIB
//  Currently draws them as single pixels. If you want
//  shape masks (hearts, stars, etc.), see earlier examples
//---------------------------------------------------
// Heart Mask (16x16)
// 10x11 Heart Mask (More Defined and Larger)
static const unsigned char HEART_MASK[10 * 11] = {
    0,0,0,1,1,0,0,0,1,1,0, 
    0,0,1,1,1,1,0,1,1,1,1, 
    0,1,1,1,1,1,1,1,1,1,1, 
    1,1,1,1,1,1,1,1,1,1,1, 
    1,1,1,1,1,1,1,1,1,1,1, 
    0,1,1,1,1,1,1,1,1,1,0, 
    0,0,1,1,1,1,1,1,1,0,0, 
    0,0,0,1,1,1,1,1,0,0,0, 
    0,0,0,0,1,1,1,0,0,0,0, 
    0,0,0,0,0,1,0,0,0,0,0  
};


// 16x16 Star Mask
static const unsigned char STAR_MASK[16 * 16] = {
    // Row  0: 1 at col0, col7, col15
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
    // Row  1: 1 at col1, col7, col14
    0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,
    // Row  2: 1 at col2, col7, col13
    0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,
    // Row  3: 1 at col3, col7, col12
    0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,
    // Row  4: 1 at col4, col7, col11
    0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
    // Row  5: 1 at col5, col7, col10
    0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,
    // Row  6: 1 at col6, col7, col9
    0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,
    // Row  7: (center row) all 1’s for a bold horizontal arm
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    // Row  8: 1 at col7 and col8
    0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,
    // Row  9: 1 at col6, col7, col9
    0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,
    // Row 10: 1 at col5, col7, col10
    0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,
    // Row 11: 1 at col4, col7, col11
    0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,
    // Row 12: 1 at col3, col7, col12
    0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,
    // Row 13: 1 at col2, col7, col13
    0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,
    // Row 14: 1 at col1, col7, col14
    0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,
    // Row 15: 1 at col0, col7, col15
    1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
};



//---------------------------------------------------
// Draw a Masked Shape (For Hearts & Stars)
//---------------------------------------------------
void DrawShape(const Particle& p, const unsigned char* mask, int width, int height)
{
    unsigned int* dst = static_cast<unsigned int*>(g_pPixels);
    const float cx = width / 2.0f;
    const float cy = height / 2.0f;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (mask[j * width + i] == 1) {
                // Scale and rotate
                float localX = (i - cx) * p.scale;
                float localY = (j - cy) * p.scale;
                float cosA = cosf(p.angle);
                float sinA = sinf(p.angle);
                float rotX = localX * cosA - localY * sinA;
                float rotY = localX * sinA + localY * cosA;

                // Final screen coordinates
                int screenX = static_cast<int>(p.x + rotX);
                int screenY = static_cast<int>(p.y + rotY);

                if (screenX < 0 || screenX >= g_ScreenWidth ||
                    screenY < 0 || screenY >= g_ScreenHeight) continue;

                // Write pixel with full alpha
                dst[screenY * g_ScreenWidth + screenX] = (0xFF << 24) | (p.color & 0xFFFFFF);

                // **Fix holes: Fill adjacent pixels**
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int nx = screenX + dx;
                        int ny = screenY + dy;

                        if (nx >= 0 && nx < g_ScreenWidth && ny >= 0 && ny < g_ScreenHeight) {
                            dst[ny * g_ScreenWidth + nx] = (0xFF << 24) | (p.color & 0xFFFFFF);
                        }
                    }
                }
            }
        }
    }
}





//---------------------------------------------------
// Draw Fire (Fast-Fading Triangle Flames)
//---------------------------------------------------
void DrawFire(const Particle& p)
{
    if (!g_pPixels) return;

    unsigned int* dst = static_cast<unsigned int*>(g_pPixels);
    
    // 🔥 Reduce the number of flame triangles to prevent excessive writes
    int numTriangles = 3 + (rand() % 3); // 3-5 small flames per particle

    for (int i = 0; i < numTriangles; i++) {
        float angle = ((rand() % 30) - 15) * (3.14159f / 180.0f);
        float scale = p.scale * (0.5f + (rand() % 50) / 100.0f);  // 50%-100% scale flickering
        int baseWidth = static_cast<int>(p.scale * 10);  // 🔥 Now directly using p.scale
		int height = static_cast<int>(p.scale * 15);     // 🔥 Scaling fire flames properly


        int tx = static_cast<int>(p.x) + (rand() % 4 - 2);
        int ty = static_cast<int>(p.y) - (rand() % 6);  // Moves upwards slightly

        // 🔥 Fire Color Gradient (Flame animation)
        int phase = rand() % 3;
        int r = 255;
        int g = (phase == 0) ? (80 + rand() % 100) : (150 + rand() % 50);  // More orange-y tones
        int b = 0;  // No blue

        // Windows expects BGR, swap manually
        unsigned int color = (0xFF << 24) | (r << 16) | (g << 8) | b;

        // Triangle Shape
        int p1x = tx;
        int p1y = ty - height;
        int p2x = tx - baseWidth / 2;
        int p2y = ty;
        int p3x = tx + baseWidth / 2;
        int p3y = ty;

        // Draw Triangle
		
		int dy = p2y - p1y;
		if (dy == 0) return;

        for (int y = p1y; y <= p2y; y++) {
            float progress = (y - p1y) / (float)(p2y - p1y);
            int leftX = p1x + static_cast<int>((p2x - p1x) * progress);
            int rightX = p1x + static_cast<int>((p3x - p1x) * progress);

            for (int x = leftX; x <= rightX; x++) {
                if (x < 0 || x >= g_ScreenWidth || y < 0 || y >= g_ScreenHeight) continue;

                // 🔥 Faster flickering brightness effect
                float alpha = 0.5f + 0.5f * sinf(p.life * 15.0f); // Adjusted flicker rate
                unsigned int finalColor = (static_cast<int>(alpha * 255) << 24) | (color & 0xFFFFFF);

                dst[y * g_ScreenWidth + x] = finalColor;
            }
        }
    }
}




// Helper function to draw a line between two points using Bresenham's algorithm.
void DrawLine(int x0, int y0, int x1, int y1, unsigned int color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x0 >= 0 && x0 < g_ScreenWidth && y0 >= 0 && y0 < g_ScreenHeight) {
            ((unsigned int*)g_pPixels)[y0 * g_ScreenWidth + x0] = color;
        }
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

//---------------------------------------------------
// Draw Sparks (Chaotic Electric Arcs)
//---------------------------------------------------
void DrawSparks(const Particle& p)
{
    // Decide how many arms (arcs) to draw for this spark.
    // For example, choose between 2 and 4 arms.
    int numArms = 2 + (rand() % 3);  // 2, 3, or 4 arms

    // Use the spark's color (with full alpha) for all arms.
    unsigned int color = (0xFF << 24) | (p.color & 0xFFFFFF);

    for (int arm = 0; arm < numArms; arm++) {
        // For each arm, choose a random number of control points (segments).
        int numPoints = 3 + (rand() % 4);  // 3 to 6 control points

        // Choose a random arc length for this arm.
        // For example, arc lengths will range between 10 and 40 pixels.
        int arcLength = 10 + (rand() % 31);

        // Pick a random overall angle (in radians) for the arm.
        float angle = ((rand() % 360) * 3.14159f) / 180.0f;

        // Starting point is the spark particle's position.
        int startX = static_cast<int>(p.x);
        int startY = static_cast<int>(p.y);

        // Compute the end point based on the arc length and angle.
        int endX = static_cast<int>(p.x + arcLength * cosf(angle));
        int endY = static_cast<int>(p.y + arcLength * sinf(angle));

        // Create an array to hold the control points.
        // (You can also use a vector if preferred.)
        POINT points[10];  // Ensure enough room (we use at most 6 points here)
        points[0].x = startX;
        points[0].y = startY;
        points[numPoints - 1].x = endX;
        points[numPoints - 1].y = endY;

        // Generate intermediate control points.
        for (int i = 1; i < numPoints - 1; i++) {
            // t goes from 0 to 1 along the line between start and end.
            float t = i / static_cast<float>(numPoints - 1);
            // Base point by linear interpolation.
            int baseX = static_cast<int>(p.x + t * (endX - p.x));
            int baseY = static_cast<int>(p.y + t * (endY - p.y));

            // Determine a perpendicular vector to the line from start to end.
            float dx = endX - p.x;
            float dy = endY - p.y;
            float len = sqrtf(dx * dx + dy * dy);
            float perpX = 0, perpY = 0;
            if (len != 0) {
                perpX = -dy / len;
                perpY = dx / len;
            }
            // Random offset magnitude:
            int offsetMagnitude = (rand() % (arcLength / 2 + 1)) - (arcLength / 4);
            int offsetX = static_cast<int>(perpX * offsetMagnitude);
            int offsetY = static_cast<int>(perpY * offsetMagnitude);

            points[i].x = baseX + offsetX;
            points[i].y = baseY + offsetY;
        }

        // Draw the arc by connecting successive control points.
        for (int i = 0; i < numPoints - 1; i++) {
            DrawLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, color);
        }
    }
}

//---------------------------------------------------
// Draw Smoke (Soft Puffs)
//---------------------------------------------------
//---------------------------------------------------
// Draw Smoke (Improved Soft Puffs)
//---------------------------------------------------
void DrawSmoke(const Particle& p)
{
    unsigned int* dst = static_cast<unsigned int*>(g_pPixels);
    
    // Determine the "radius" of the smoke puff based on its scale.
    int radius = static_cast<int>(p.scale * 8);
    
    // Loop over a square region that fully contains the smoke circle.
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int screenX = static_cast<int>(p.x) + dx;
            int screenY = static_cast<int>(p.y) + dy;
            
            // Skip drawing if we're outside the screen.
            if (screenX < 0 || screenX >= g_ScreenWidth ||
                screenY < 0 || screenY >= g_ScreenHeight)
                continue;
            
            // Compute the distance from the center of the puff.
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > radius)
                continue;
            
            // Compute alpha with a squared falloff for a smoother edge.
            float alpha = 1.0f - (dist / (float)radius);
            alpha = alpha * alpha;  // Squared falloff
            
            // Optionally, add a small amount of randomness to the alpha
            // to simulate the turbulent, wispy nature of smoke.
            float noise = (rand() % 20) / 100.0f - 0.1f;  // Random value in [-0.1, 0.1]
            alpha = std::min(1.0f, std::max(0.0f, alpha + noise));
            
            // You might want to lower the overall opacity for smoke;
            // here we multiply by 150 (out of 255) to limit the maximum alpha.
            unsigned int finalAlpha = static_cast<int>(alpha * 150);
            
            // Construct the final color.
            // p.color is assumed to be a grayish tone.
            unsigned int color = (finalAlpha << 24) | (p.color & 0xFFFFFF);
            
            dst[screenY * g_ScreenWidth + screenX] = color;
        }
    }
}


//---------------------------------------------------
// DrawParticlesToDIB (Now draws full shapes, not single pixels)
//---------------------------------------------------
void DrawParticlesToDIB()
{
    if (!g_pPixels) return;

    // Clear the screen buffer.
    unsigned int* dst = static_cast<unsigned int*>(g_pPixels);
    memset(dst, 0, g_ScreenWidth * g_ScreenHeight * sizeof(unsigned int));

    // For each particle, convert its global coordinates into the overlay's
    // coordinate space by subtracting the virtual offsets.
    for (const auto &p : g_particles)
    {
        // Compute adjusted coordinates.
        int adjustedX = static_cast<int>(p.x) - g_VirtualOffsetX;
        int adjustedY = static_cast<int>(p.y) - g_VirtualOffsetY;

        // Only draw if the particle lies within the DIB.
        if (adjustedX < 0 || adjustedX >= g_ScreenWidth ||
            adjustedY < 0 || adjustedY >= g_ScreenHeight)
            continue;

        // Create a local copy of the particle with adjusted coordinates.
        Particle pAdjusted = p;
        pAdjusted.x = adjustedX;
        pAdjusted.y = adjustedY;

        // Now use the adjusted particle for drawing.
        switch (p.type) {
            case ParticleType::HEARTS:
                DrawShape(pAdjusted, HEART_MASK, 11, 10); // Use new mask dimensions.
                break;
            case ParticleType::STARS:
                DrawShape(pAdjusted, STAR_MASK, 16, 16);
                break;
            case ParticleType::FIRE:
                DrawFire(pAdjusted);
                break;
            case ParticleType::SPARKS:
                DrawSparks(pAdjusted);
                break;
            case ParticleType::SMOKE:
                DrawSmoke(pAdjusted);
                break;
            case ParticleType::SWORD:
                DrawSword(pAdjusted);
                break;
        }
    }
}


//---------------------------------------------------
// Draw Sword (Composite Particle)
// This function draws a sword composed of a blade,
// a cross-guard, a hilt, and a pommel.
//---------------------------------------------------
void DrawSword(const Particle& p)
{
    // We'll assume that p.x and p.y are the center position
    // of the sword particle. The sword is defined in local
    // coordinates (in pixels) and then transformed by the
    // particle's scale and rotation (p.angle).

    // Pre-calculate rotation values.
    float cosA = cosf(p.angle);
    float sinA = sinf(p.angle);

    // Define a bounding box in local coordinates that encloses the whole sword.
    // (These values are chosen based on our sword design below.)
    int localMinX = -20;
    int localMaxX = 20;
    int localMinY = -60;
    int localMaxY = 20;

    // Loop over the bounding box.
    for (int ly = localMinY; ly <= localMaxY; ly++) {
        for (int lx = localMinX; lx <= localMaxX; lx++) {

            // Transform local coordinates (lx, ly) to screen space.
            float localX = lx * p.scale;
            float localY = ly * p.scale;
            float rotX = localX * cosA - localY * sinA;
            float rotY = localX * sinA + localY * cosA;
            int screenX = static_cast<int>(p.x + rotX);
            int screenY = static_cast<int>(p.y + rotY);

            // Skip if outside the screen.
            if (screenX < 0 || screenX >= g_ScreenWidth ||
                screenY < 0 || screenY >= g_ScreenHeight)
                continue;

            // Determine which part of the sword the point falls in.
            // --- Sword Design in Local Coordinates ---
            // Blade: A narrow vertical rectangle.
            //     x in [-3, 3], y in [-50, 0]
            bool inBlade = (lx >= -3 && lx <= 3 && ly >= -50 && ly <= 0);

            // Cross-guard: A wide, thin horizontal rectangle.
            //     x in [-10, 10], y in [0, 4]
            bool inGuard = (lx >= -10 && lx <= 10 && ly >= 0 && ly <= 4);

            // Hilt: A narrow vertical rectangle.
            //     x in [-2, 2], y in [4, 14]
            bool inHilt = (lx >= -2 && lx <= 2 && ly >= 4 && ly <= 14);

            // Pommel: A circle at the end of the hilt.
            //     Centered at (0, 14) with radius 3.
            float dx = lx - 0;
            float dy = ly - 14;
            bool inPommel = (dx * dx + dy * dy <= 9);

            // Choose a color based on which part we are drawing.
            // Colors are defined as 0xAARRGGBB.
            unsigned int color = 0;
            if (inBlade) {
                // Blade: A shining silver.
                color = (0xFF << 24) | (0xC0C0C0 & 0xFFFFFF);
            } else if (inGuard) {
                // Guard: A rich golden color.
                color = (0xFF << 24) | (0xFFD700 & 0xFFFFFF);
            } else if (inHilt) {
                // Hilt: A deep brown.
                color = (0xFF << 24) | (0x8B4513 & 0xFFFFFF);
            } else if (inPommel) {
                // Pommel: A dark grey.
                color = (0xFF << 24) | (0x696969 & 0xFFFFFF);
            } else {
                // Not part of the sword.
                continue;
            }

            // Draw the pixel.
            ((unsigned int*)g_pPixels)[screenY * g_ScreenWidth + screenX] = color;
        }
    }
}
