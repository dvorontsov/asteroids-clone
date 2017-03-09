#pragma once
#include "render.h"

#ifndef ASTEROIDS_CLOHE_H
#define ASTEROIDS_CLONE_H

#define ArraySize(a) (sizeof(a)/sizeof(a[0]))

#define FRAMES_PER_SECOND 30

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

struct game_controller
{
	bool forward;
	bool back;
	bool left;
	bool right;
	bool shoot;
	bool pause;
	bool ready;

	bool debug_asteroids;
	bool debug_grid;
};

enum model_type
{
	PROJECTILE,
	SPACESHIP,
	ASTEROID_LARGE,
	ASTEROID_MEDIUM,
	ASTEROID_SMALL,
	MODEL_TYPE_LAST_INDEX
};

struct model_t
{
	int vertCount;
	v2 *vertices;
	v2 *verticesTransformed;
	float rotationDeltaDegrees;
	float rotationDegrees;
};

enum asteroid_gen_region
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	LAST_INDEX
};

struct position_and_direction
{
	v2 posision;
	v2 direction;
};

enum asteroid_state
{
	UNDEFINED,
	HASNT_APPEARED_ON_SCREEN_YET,
	VISIBLE_ON_SCREEN,
	REGENERATE
};

struct game_entity
{
	v2 position;
	v2 direction;
	float velocity;
	model_t *model;
	bool modelMemoryAllocated;
};

struct projectile_t : game_entity
{
	bool active;
	projectile_t *next;
};

struct spaceship : game_entity
{
	float maxVelocity;
	v2 facingDirection;
	v2 projectileWeapon;
	projectile_t *projectiles;
	bool fireCooldown;
	int rateOfFire;				// in frames
	int rateOfFireFrameCount;  
};

struct asteroid : game_entity
{
	asteroid_state state;
};

#define ASTEROIDS_COUNT 20
#define ASSET_COUNT 5

struct game_state
{
	spaceship *player;
	asteroid asteroids[ASTEROIDS_COUNT];
	game_controller controller;
	model_t modelAssets[ASSET_COUNT];
	bool isGameOver;
	bool ready;
	int playerLives = 1;
	bool gameInProgress;
};

// Init
void InitPlayer(game_state *state);
void InitAsteroids(game_state *state, bool allocateMemory);
void InitModels(model_t *models);

// Update
void UpdateGame(game_state * state);
void UpdatePlayer(game_state *state);
void UpdateAsteroids(asteroid *asteroids, int asteroidsCount, model_t *modelAssets);
void UpdateProjectiles(projectile_t **projectiles);
void CheckCollisions(game_state *state);

// Render
void RenderPlayer(const win32_video_buffer *buffer, const game_state *state);
void RenderModel(const win32_video_buffer *buffer, const  model_t *model, v2 position, uint32_t aarrggbb_color);
void RenderModel(const win32_video_buffer *buffer, const v2 *vertices, int vertexCount, v2 position);
void RenderAsteroids(const win32_video_buffer *buffer, const asteroid *asteroids, int asteroidsCount);
void RenderProjectiles(const win32_video_buffer *buffer, const projectile_t *projectiles, float rotationDegrees);

// Cleanup
void CleanUp(game_state *gameState);

#endif