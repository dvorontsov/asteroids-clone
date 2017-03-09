#include "asteroids-clone.h"
#include <string>

v2 GetInitPlayerDirection()
{
	return v2(0, 1);
}

asteroid_gen_region GetRandomAsteroidGenerationRegion()
{
	int region = rand() % asteroid_gen_region::LAST_INDEX;
	return (asteroid_gen_region)region;
}

position_and_direction GenerateAsteroidInitialPositionAndMovementDirection()
{
	asteroid_gen_region region = GetRandomAsteroidGenerationRegion();
	int randomX = rand() % (SCREEN_WIDTH);
	int randomY = rand() % (SCREEN_HEIGHT);
	v2 direction = v2(1, 0);  // init value

	int directionDegreesDelta = rand() % 46;
	int directionRotation = rand() % 2;  // binary choice
	if (!directionRotation)
	{
		directionDegreesDelta = -1 * directionDegreesDelta;
	}

	position_and_direction result;
	switch (region)
	{
	case NORTH:
		result.posision = v2(randomX, randomY + SCREEN_HEIGHT);
		Rotate(v2(0, 0), &direction, 270.0 + directionDegreesDelta);
		result.direction = direction;
		break;
	case EAST:
		result.posision = v2(randomX + SCREEN_WIDTH, randomY);
		Rotate(v2(0, 0), &direction, 180.0 + directionDegreesDelta);
		result.direction = direction;
		break;
	case SOUTH:
		result.posision = v2(randomX, 0 - randomY);
		Rotate(v2(0, 0), &direction, 90.0 + directionDegreesDelta);
		result.direction = direction;
		break;
	case WEST:
		result.posision = v2(0 - randomX, randomY);
		Rotate(v2(0, 0), &direction, 0.0 + directionDegreesDelta);
		result.direction = direction;
		break;
	default:
		result.posision = v2(0, 0);  // should never happen, indication of error
		result.direction = direction;
		break;
	}

	return result;
}

float GenerateAsteroidInitialVelocity()
{
	int whole = (rand() % 5) + 1;
	int fraction = rand() % 100;
	float result = whole + (fraction / 100);
	return result * 1.5;
}

void InitAsteroidState(asteroid *asteroid)
{
	if (asteroid->model == nullptr)
	{
		asteroid->state = asteroid_state::UNDEFINED;
		return;
	}

	v2 *verticies = asteroid->model->vertices;
	int numOfVerts = asteroid->model->vertCount;
	bool allVertsAreOffScreen = true;

	for (int j = 0; j < numOfVerts; j++)
	{
		if (WithinScreenBoundaries(asteroid->model->vertices[j].x + asteroid->position.x,
			asteroid->model->vertices[j].y + asteroid->position.y, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			allVertsAreOffScreen = false;
		}
	}

	if (allVertsAreOffScreen) 
	{
		if (asteroid->state == asteroid_state::VISIBLE_ON_SCREEN) 
		{
			asteroid->state = REGENERATE;
			return;
		}
		asteroid->state = asteroid_state::HASNT_APPEARED_ON_SCREEN_YET;
	}
	else
	{
		asteroid->state = asteroid_state::VISIBLE_ON_SCREEN;
	}	
}

int GernerateAsteroidRotationDeltaDegrees()
{
	int direction = rand() % 2;
	int magnitude = rand() % 10;

	int result;
	if (direction)
	{
		result = magnitude;
	}
	else
	{
		result = -1 * magnitude;
	}

	return result;
}

void GenerateAsteroid(asteroid *asteroid, model_t *models, bool initMemory)
{

	int asteroidRand = rand() % 3;
	model_type asteroidModel;
	switch (asteroidRand)
	{
	case 0:
		asteroidModel = model_type::ASTEROID_SMALL;
		break;
	case 1:
		asteroidModel = model_type::ASTEROID_MEDIUM;
		break;
	case 2:
		asteroidModel = model_type::ASTEROID_LARGE;
		break;

	default:
		asteroidModel = model_type::ASTEROID_MEDIUM;
		break;
	}

	size_t vertMemSize = sizeof(v2) * models[asteroidModel].vertCount;

	if (initMemory)
	{
		asteroid->model = (model_t *)malloc(sizeof(model_t));
		asteroid->model->verticesTransformed = (v2 *)malloc(vertMemSize);
	}

	asteroid->model->vertices = models[asteroidModel].vertices;
	asteroid->model->vertCount = models[asteroidModel].vertCount;
	memcpy(asteroid->model->verticesTransformed, asteroid->model->vertices, vertMemSize);
	asteroid->model->rotationDeltaDegrees = GernerateAsteroidRotationDeltaDegrees();
	asteroid->model->rotationDegrees = 0;

	position_and_direction posAndDir = GenerateAsteroidInitialPositionAndMovementDirection();
	asteroid->position = posAndDir.posision;
	asteroid->direction = posAndDir.direction;
	asteroid->velocity = GenerateAsteroidInitialVelocity();
	asteroid->state = asteroid_state::UNDEFINED;
	
	InitAsteroidState(asteroid);
}

void InitAsteroids(game_state *state, bool allocateMemory)
{
	for(int i = 0; i < ASTEROIDS_COUNT; i++)
	{
		GenerateAsteroid(&state->asteroids[i], state->modelAssets, allocateMemory);
	}
}

bool isAtLeastOneVerticeOnScreen(v2 *vertices, int vertCount, v2 position)
{
	for (int i = 0; i < vertCount; i++)
	{
		if (WithinScreenBoundaries(vertices[i].x + position.x,
			vertices[i].y + position.y, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			return true;
		}
	}
	return false;
}

bool isAllVerticesOffScreen(v2 *vertices, int vertCount, v2 position)
{
	for (int i = 0; i < vertCount; i++)
	{
		if (WithinScreenBoundaries(vertices[i].x + position.x,
			vertices[i].y + position.y, SCREEN_WIDTH, SCREEN_HEIGHT))
		{
			return false;
		}
	}
	return true;
}

void UpdateAsteroidState(asteroid *asteroid, int asteroidsCount, model_t *modelAssets)
{
	if (asteroid->state == asteroid_state::REGENERATE)
	{
		GenerateAsteroid(asteroid, modelAssets, false);
	}
	else if (asteroid->state == asteroid_state::HASNT_APPEARED_ON_SCREEN_YET)
	{
		// if position crossed either horizontal or vertical border, but
		// the asteriod is still in "HASNT_APPEARED_ON_SCREEN_YET" stage,
		// it means it is never going on screen because of starting_postion 
		// and direction_vector
		int error = 3;		
		if ((asteroid->position.x < (0 + error)) && (asteroid->position.x >(0 - error)) ||
			(asteroid->position.y < (0 + error)) && (asteroid->position.y >(0 - error)))
		{
			asteroid->state = asteroid_state::REGENERATE;
		}

		if (isAtLeastOneVerticeOnScreen(asteroid->model->vertices, asteroid->model->vertCount, 
			asteroid->position))
		{
			asteroid->state = asteroid_state::VISIBLE_ON_SCREEN;
		}
	}
	else if (asteroid->state == asteroid_state::VISIBLE_ON_SCREEN)
	{
		// check if it went offscreen and "off the game"
		v2 *verticies = asteroid->model->vertices;
		int numOfVerts = asteroid->model->vertCount;
		if (isAllVerticesOffScreen(asteroid->model->vertices, asteroid->model->vertCount, 
			asteroid->position)) {
			asteroid->state = REGENERATE;
		}
	}
}

void UpdateGame(game_state *state)
{
	if (state->isGameOver && state->controller.ready)
	{
		state->isGameOver = false;
		state->controller.ready = false;
		state->playerLives += 1;
		InitPlayer(state);
		InitAsteroids(state, false);
	}

	bool isPaused = state->controller.pause;

	if (state->playerLives)
	{
		if (!isPaused  && !state->isGameOver)
		{
			CheckCollisions(state);
			UpdatePlayer(state);
			UpdateAsteroids(state->asteroids, ArraySize(state->asteroids),
				state->modelAssets);
		}
	}
	else
	{
		// Game Over
		state->isGameOver = 1;
		state->ready = 0;
	}
}

void UpdateAsteroids(asteroid *asteroids, int asteroidsCount, model_t *modelAssets)
{
	for (int i = 0; i < asteroidsCount; i++)
	{
		UpdateAsteroidState(&asteroids[i], asteroidsCount, modelAssets);

		if (asteroids[i].state != asteroid_state::UNDEFINED)
		{
			// update position
			v2 dPos = asteroids[i].direction * asteroids[i].velocity;
			asteroids[i].position = asteroids[i].position + dPos;
			float rotation = asteroids[i].model->rotationDegrees + asteroids[i].model->rotationDeltaDegrees;
			if (rotation > 360.0)
			{
				rotation = rotation - 360;
			}			
			asteroids[i].model->rotationDegrees = rotation;
		}		
	}
}

void RenderAsteroids(const win32_video_buffer *buffer, const asteroid *asteroids, int asteroidsCount)
{
	for (int i = 0; i < asteroidsCount; i++)
	{
		if (asteroids[i].state == VISIBLE_ON_SCREEN)
		{
			RenderModel(buffer, asteroids[i].model, asteroids[i].position, AARRGGBB_Color(0, 255, 255, 255));
		}		
	}
}

void InitModels(model_t *models)
{
	// Spaceship
	models[model_type::SPACESHIP].vertCount = 4;
	v2 *spaceship = new v2[4]{ v2(0, 35), v2(15, -15), v2(0, 0), v2(-15, -15) };
	models[model_type::SPACESHIP].vertices = spaceship;

	// Projectile
	models[model_type::PROJECTILE].vertCount = 4;
	v2 *projectileModel = new v2[4]{ v2(0, 0), v2(0, 5), v2(1, 5), v2(1, 0) };
	models[model_type::PROJECTILE].vertices = projectileModel;

	// Asteroid Small
	models[model_type::ASTEROID_SMALL].vertCount = 6;
	//v2 *asteroidSmallModel = new v2[4]{ v2(0, 15), v2(15, 0), v2(0, -15), v2(-15, 0) };
	v2 *asteroidSmallModel = new v2[6]{ v2(0, 15), v2(12, 10), v2(15, 0), v2(5, -15), v2(-15, -10), v2(-15, 5) };
	models[model_type::ASTEROID_SMALL].vertices = asteroidSmallModel;

	// Asteroid Medium
	models[model_type::ASTEROID_MEDIUM].vertCount = 7;
	//v2 *asteroidMediumModel = new v2[4]{ v2(0, 25), v2(25, 0), v2(0, -25), v2(-25, 0) };
	v2 *asteroidMediumModel = new v2[7]{ v2(0, 25), v2(-15, 15), v2(-20, -10), v2(-10, -25), v2(15, -20), v2(25, 0), v2 (15, 20) };
	models[model_type::ASTEROID_MEDIUM].vertices = asteroidMediumModel;

	// Asteroid Large
	models[model_type::ASTEROID_LARGE].vertCount = 7;
	//v2 *asteroidLargeModel = new v2[4]{ v2(0, 35), v2(35, 0), v2(0, -35), v2(-35, 0) };
	v2 *asteroidLargeModel = new v2[7]{ v2(0, 35), v2(-25, 30), v2(-35, 0), v2(-25, -25),  v2(10, -35), v2(30, -20), v2(30, 20) };
	models[model_type::ASTEROID_LARGE].vertices = asteroidLargeModel;
}


void AddProjectile(projectile_t **head, projectile_t *projectile)
{
	if (projectile == nullptr || projectile == nullptr)
	{
		return;
	}

	projectile->next = nullptr;
	if (*head == nullptr)
	{
		// add head
		*head = projectile;			
	}
	else
	{
		projectile_t *current = *head;
		while (current->next != nullptr)
		{
			current = current->next;
		}
		current->next = projectile;
	}	
}


void InitPlayer(game_state *state)
{
	if (!state->player)
	{
		state->player = (spaceship *)malloc(sizeof(spaceship));
		state->player->modelMemoryAllocated = false;
	}
	state->player->maxVelocity = 1.0;
	state->player->velocity = 0;
	state->player->facingDirection = GetInitPlayerDirection();
	state->player->direction = GetInitPlayerDirection();
	state->player->position = v2(500, 200);

	state->player->projectileWeapon = v2(0, 35);
	state->player->rateOfFire = 10;
	state->player->rateOfFireFrameCount = 0;
	state->player->fireCooldown = false;

	state->player->projectiles = nullptr;

	size_t vertMemSize = sizeof(v2) * state->modelAssets[model_type::SPACESHIP].vertCount;
	if (!state->player->modelMemoryAllocated)
	{
		state->player->model = (model_t *)malloc(sizeof(model_t));
		state->player->modelMemoryAllocated = true;
	}

	state->player->model->verticesTransformed = (v2 *)malloc(vertMemSize);
	state->player->model->vertices = state->modelAssets[model_type::SPACESHIP].vertices;
	state->player->model->vertCount = state->modelAssets[model_type::SPACESHIP].vertCount;
	memcpy(state->player->model->verticesTransformed, state->player->model->vertices, vertMemSize);
	state->player->model->rotationDegrees = 0;
	state->player->model->rotationDeltaDegrees = 0;
}


bool isBetween(float value, float bound1, float bound2)
{
	if (bound1 < bound2)
	{
		if (value > bound1 && value < bound2)
		{
			return true;
		}
	}
	else
	{
		if (value > bound2 && value < bound1)
		{
			return true;
		}
	}
	return false;
}

/*
 * For each line segment of model 1, check if intersects with any line segments of model 2.
 * Returns true is intersection found.  False, otherwise.
 *
 */
bool isCollisionDetected(v2 *model1, int vertCount1, v2 position1, v2 *model2, int vertCount2, v2 position2)
{
	for (int i = 0; i < vertCount1; i++)
	{
		float x1 = model1[i].x + position1.x;
		float y1 = model1[i].y + position1.y;
		float x1_next;
		float y1_next;

		if (i == vertCount1 - 1)
		{
			x1_next = model1[0].x + position1.x;
			y1_next = model1[0].y + position1.y;
		}
		else
		{
			x1_next = model2[i + 1].x + position1.x;
			y1_next = model2[i + 1].y + position1.y;
		}

		float m1 = (y1_next - y1) / (x1_next - x1);
		float b1 = y1 - m1 * x1;

		for (int j = 0; j < vertCount2; j++)
		{
			float x2 = model2[j].x + position2.x;
			float y2 = model2[j].y + position2.y;
			float x2_next;
			float y2_next;

			if (j == vertCount2 - 1)
			{
				x2_next = model2[0].x + position2.x;
				y2_next = model2[0].y + position2.y;
			}
			else
			{
				x2_next = model2[j + 1].x + position2.x;
				y2_next = model2[j + 1].y + position2.y;
			}

			float m2 = (y2_next - y2) / (x2_next - x2);
			float b2 = y2 - m2 * x2;

			float x_intersection = (b2 - b1) / (m1 - m2);
			float y_intersection = m1 * x_intersection + b1;

			if (isBetween(x_intersection, x1, x1_next) && isBetween(x_intersection, x2, x2_next) &&
				isBetween(y_intersection, y1, y1_next) && isBetween(y_intersection, y2, y2_next))
			{
				return true;				
			}
		}
	}

	return false;
}


void UpdateProjectiles(projectile_t **projectiles)
{
	projectile_t *current = *projectiles;
	projectile_t *previous = nullptr;

	if (current != nullptr)
	{
		while (current != nullptr)
		{
			if (WithinScreenBoundaries(current->position.x, current->position.y, 
				SCREEN_WIDTH, SCREEN_HEIGHT) && current->active)
			{
				// update position
				current->position = current->position +
					current->direction * current->velocity;

				previous = current;
				current = current->next;
			}
			else
			{
				// remove from list
				projectile_t *temp = current;
				current = current->next;
				if (previous == nullptr) {
					// update head
					*projectiles = current;
				}
				else
				{
					// if not removing head
					previous->next = current;
				}

 				free(temp->model->verticesTransformed);
				free(temp->model);
				free(temp);
			}					
		}
	}
}

void CheckCollisions(game_state *state)
{
	asteroid *asteroids = state->asteroids;
	spaceship *player = state->player;

	int asteroidsArraySize = ArraySize(state->asteroids);
	for (int i = 0; i < asteroidsArraySize; i++)
	{
		asteroid *currAsteroid = &asteroids[i];
		if (currAsteroid != nullptr && currAsteroid->state != asteroid_state::UNDEFINED)
		{
			// player vs asteroid
			bool playerAsteriodCollision =
				isCollisionDetected(player->model->vertices, player->model->vertCount, player->position,
					currAsteroid->model->vertices, currAsteroid->model->vertCount, currAsteroid->position);
			if (playerAsteriodCollision)
			{
				state->playerLives -= 1;
				currAsteroid->state = asteroid_state::REGENERATE;
			}

			// projectile vs asteroid
			projectile_t *currProjectile = state->player->projectiles;
			while (currProjectile != nullptr)
			{
				bool projectileAsteriodCollision =
					isCollisionDetected(currProjectile->model->vertices, currProjectile->model->vertCount, currProjectile->position,
						currAsteroid->model->vertices, currAsteroid->model->vertCount, currAsteroid->position);

				if (projectileAsteriodCollision)
				{
					currAsteroid->state = asteroid_state::REGENERATE;
					currProjectile->active = false;
				}

				currProjectile = currProjectile->next;
			}
		}
	}
}


void UpdatePlayer(game_state *state)
{
	spaceship *player = state->player;
	game_controller *controller = &state->controller;
	model_t *playerModel = player->model;

	UpdateProjectiles(&player->projectiles);

	if (controller->right)
	{
		playerModel->rotationDeltaDegrees -= (7 * 2);
		playerModel->rotationDegrees = std::fmod(playerModel->rotationDeltaDegrees, 360.0);
	}

	if (controller->left)
	{
		playerModel->rotationDeltaDegrees += (7 * 2);
		playerModel->rotationDegrees = std::fmod(playerModel->rotationDeltaDegrees, 360.0);
	}

	Rotate(v2(0, 0), &player->facingDirection, playerModel->rotationDegrees);
	Rotate(v2(0, 0), &player->projectileWeapon, playerModel->rotationDegrees);

	if (player->fireCooldown)
	{
		if (player->rateOfFireFrameCount >= player->rateOfFire)
		{
			player->rateOfFireFrameCount = 0;
			player->fireCooldown = false;
		}
		else
		{
			player->rateOfFireFrameCount++;
		}
	}

	if (controller->shoot) {
		if (!player->fireCooldown)
		{
			projectile_t *torpedo = (projectile_t *)malloc(sizeof(projectile_t));
			torpedo->next = nullptr;
			torpedo->position = player->position + player->projectileWeapon;
			torpedo->direction = player->facingDirection;
			torpedo->velocity = 25.0;
			torpedo->active = true;

			// Init projectile model
			torpedo->model = (model_t *)malloc(sizeof(model_t));
			torpedo->model->vertices = state->modelAssets[model_type::PROJECTILE].vertices;
			torpedo->model->vertCount = state->modelAssets[model_type::PROJECTILE].vertCount;
			int vertexArraySize = sizeof(v2) * torpedo->model->vertCount;
			torpedo->model->verticesTransformed = (v2 *)malloc(vertexArraySize);
			memcpy(torpedo->model->verticesTransformed, torpedo->model->vertices, vertexArraySize);

			for (int i = 0; i < torpedo->model->vertCount; i++)
			{
				Rotate(v2(0, 0), &torpedo->model->verticesTransformed[i], player->model->rotationDegrees);
			}

			AddProjectile(&player->projectiles, torpedo);
			player->fireCooldown = true;
		}		
	}

	if (controller->forward)
	{
		player->direction = player->facingDirection;
	}

	float t = (float)1 / 30 * 1000.0; // seconds
	float a = 0.0f; // acceleration

	if (controller->forward) {
		a = 0.005f / 2.0f;
	}

	if (controller->back) {
		a = -0.0005f;
	}

	// drag
	a += -0.0025 * player->velocity;

	player->velocity = player->velocity + a * t;
	if (player->velocity < 0)
	{
		player->velocity = 0;
	}
	if (player->velocity > player->maxVelocity)
	{
		player->velocity = player->maxVelocity;
	}

	v2 direction = player->direction;
	if (controller->forward)
	{
		direction = player->facingDirection;
	}
	
	v2 dPosition = direction * (player->velocity * t + (1 / 2) * a * t * t);
	player->position = dPosition + player->position;
	
	// Reset Values
	player->facingDirection = GetInitPlayerDirection();
	player->projectileWeapon = v2(0, 35);
}

void CleanUp(game_state *gameState)
{
	// Asteroids
	asteroid *asteroids = gameState->asteroids;
	for (int i = 0; i < ArraySize(asteroids); i++)
	{
		free(asteroids[i].model->verticesTransformed);
		free(asteroids[i].model);		
	}

	// Assets
	for (int i = 0; i < model_type::MODEL_TYPE_LAST_INDEX; i++)
	{
		delete gameState->modelAssets[i].vertices;
	}

	// Player
	free(gameState->player->model->verticesTransformed);
	free(gameState->player->model);
	free(gameState->player);
}

void RenderModel(const win32_video_buffer *buffer, v2 *vertices, int vertexCount, v2 position)
{
	if (vertices == nullptr || buffer == nullptr)
	{
		return;
	}

	for (int i = 0; i < vertexCount - 1; i++)
	{
		DrawLine(buffer, vertices[i] + position,
			vertices[i + 1] + position, AARRGGBB_Color(0, 255, 0, 0));
	}
	DrawLine(buffer, vertices[vertexCount - 1] + position, vertices[0] + position, AARRGGBB_Color(0, 255, 0, 0));
}

void RenderModel(const win32_video_buffer *buffer, const  model_t *model, v2 position, uint32_t aarrggbb_color)
{
	if (model == nullptr || buffer == nullptr)
	{
		return;
	}

	memcpy(model->verticesTransformed, model->vertices, sizeof(v2) * model->vertCount);

	if (model->rotationDegrees)
	{
		for (int i = 0; i < model->vertCount; i++)
		{
			Rotate(v2(0, 0), &model->verticesTransformed[i], model->rotationDegrees);
		}
	}

	for (int i = 0; i < model->vertCount - 1; i++)
	{
		DrawLine(buffer, model->verticesTransformed[i] + position,
			model->verticesTransformed[i + 1] + position, aarrggbb_color);
	}
	DrawLine(buffer, model->verticesTransformed[model->vertCount - 1] + position,
		model->verticesTransformed[0] + position, aarrggbb_color);
}

void RenderProjectiles(const win32_video_buffer *buffer, const projectile_t *projectiles, float rotationDegrees)
{
	const projectile_t *current = projectiles;
	if (projectiles != nullptr)
	{
		while (current != nullptr)
		{
			RenderModel(buffer, current->model->verticesTransformed, 
				current->model->vertCount, current->position);

			current = current->next;
		}
	}
}

void RenderPlayer(const win32_video_buffer *buffer, const game_state *state)
{
	model_t *playerModel = state->player->model;

	RenderProjectiles(buffer, state->player->projectiles, playerModel->rotationDeltaDegrees);

	if (playerModel)
	{
		RenderModel(buffer, playerModel, state->player->position, AARRGGBB_Color(0, 255, 255, 255));
	}
}