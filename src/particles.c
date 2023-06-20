#include "./particles.h"
#include "../config.h"

#include <math.h>
#include <SDL2/SDL2_gfxPrimitives.h>

void init_particles(struct particle **particles) {
	*particles = calloc(NUM_PARTICLES, sizeof(struct particle));
}

void spawn_particles(struct particle **particles, struct vector size) {
	for (size_t i = 0; i < NUM_PARTICLES; ++i) {
		struct particle *particle = &(*particles)[i];

		particle->color = i % NUM_COLORS,
		particle->position.x = ((float)rand() / RAND_MAX) * size.x,
		particle->position.y = ((float)rand() / RAND_MAX) * size.y,
		particle->velocity.x = 0,
		particle->velocity.y = 0;
	}
}

float get_magnitude(float x, float y) {
	return sqrt(x * x + y * y);
}

bool in_range_walls(struct vectorf a, struct vectorf b, struct vectorf *diff, float *magnitude, struct vector size) {
	if (in_range(a, b, diff, magnitude)) return true;
	struct vectorf a_;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (i == 0 && j == 0) continue;
			a_.x = i * size.x + a.x, a_.y = j * size.y + a.y;
			if (in_range(a_, b, diff, magnitude)) return true;
		}
	}
	return false;
}

bool in_range(struct vectorf a, struct vectorf b, struct vectorf *diff, float *magnitude) {
	if (a.x < b.x - FORCE_RADIUS) return false;
	if (a.x > b.x + FORCE_RADIUS) return false;
	if (a.y < b.y - FORCE_RADIUS) return false;
	if (a.y > b.y + FORCE_RADIUS) return false;

	struct vectorf d = (struct vectorf) { a.x - b.x, a.y - b.y };
	if (diff) *diff = d;

	float m = get_magnitude(d.x, d.y);
	if (m > FORCE_RADIUS) return false;
	if (magnitude) *magnitude = m;

	return true;
}

float force_beta_inv = 1.0f / FORCE_BETA;
float force_beta_half_inv = 2.0f / (FORCE_RADIUS - FORCE_BETA);
float force_beta_mid = (FORCE_RADIUS - FORCE_BETA) / 2.0f + FORCE_BETA;

float get_force(float distance, float attraction) {
	if (distance < FORCE_BETA)
		return (distance * force_beta_inv - 1) * FORCE_BETA_MULTIPLIER;

	float f = (distance - FORCE_BETA) * force_beta_half_inv;
	if (distance > force_beta_mid) {
		if (f >= 2) return 0.0f;
		f = 2 - f;
	}

	return f * attraction * FORCE_ATTRACTION_MULTIPLIER;
}

float attraction_matrix[NUM_COLORS * NUM_COLORS] = ATTRACTION_MATRIX;

float get_attraction(int a, int b) {
	return attraction_matrix[a * NUM_COLORS + b];
}

void step_particles(struct particle **particles, int frame_duration, struct vector size) {
	struct particle *particle1;
	struct vectorf acceleration;
	float magnitude, attraction, force;

	for (size_t i = 0; i < NUM_PARTICLES; ++i) {
		particle1 = &(*particles)[i];

		for (size_t j = 0; j < NUM_PARTICLES; ++j) {
			if (j == i) continue;

			if (!in_range_walls(particle1->position, (*particles)[j].position, &acceleration, &magnitude, size)) continue;

			if (magnitude == 0.0f) {
				acceleration.x = 1.0f;
				magnitude = 1.0f;
			}

			attraction = get_attraction(particle1->color, (*particles)[j].color);
			force = get_force(magnitude, attraction);

			acceleration.x *= force / magnitude;
			acceleration.y *= force / magnitude;

			particle1->velocity.x -= acceleration.x;
			particle1->velocity.y -= acceleration.y;
		}

		particle1->velocity.x *= FRICTION;
		particle1->velocity.y *= FRICTION;
	}

	for (size_t i = 0; i < NUM_PARTICLES; ++i) {
		particle1 = &(*particles)[i];

		particle1->position.x += particle1->velocity.x * VELOCITY_MULTIPLIER * frame_duration;
		particle1->position.y += particle1->velocity.y * VELOCITY_MULTIPLIER * frame_duration;

		while (particle1->position.x >= size.x) particle1->position.x -= size.x;
		while (particle1->position.y >= size.y) particle1->position.y -= size.y;
		while (particle1->position.x < 0) particle1->position.x += size.x;
		while (particle1->position.y < 0) particle1->position.y += size.y;
	}
}

float color_multiplier = 6.0f / (float)NUM_COLORS;

SDL_Color get_color(int index) {
	float hue = (float)index * color_multiplier;
	int i = (int)hue;
	float inc_value = hue - i;
	float dec_value = 1.0f - inc_value;
	float r = 0, g = 0, b = 0;
	switch (i) {
		case 0:
			r = 1.0f, g = inc_value, b = 0.0f;
			break;
		case 1:
			r = dec_value, g = 1.0f, b = 0.0f;
			break;
		case 2:
			r = 0.0f, g = 1.0f, b = inc_value;
			break;
		case 3:
			r = 0.0f, g = dec_value, b = 1.0f;
			break;
		case 4:
			r = inc_value, g = 0.0f, b = 1.0f;
			break;
		case 5:
			r = 1.0f, g = 0.0f, b = dec_value;
			break;
	}
	return (SDL_Color) { r*255, g*255, b*255, 255 };
}

void draw_particles(struct particle **particles, SDL_Renderer *renderer) {
	for (size_t i = 0; i < NUM_PARTICLES; ++i) {
		struct particle *particle = &(*particles)[i];
		SDL_Color color = get_color(particle->color);
		filledCircleRGBA(renderer, particle->position.x, particle->position.y, CIRCLE_RADIUS, color.r, color.g, color.b, color.a);
	}
}
