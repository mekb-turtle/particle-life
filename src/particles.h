#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#ifndef UTIL_DEF
#define UTIL_DEF

#include "../config.h"

struct vector {
	int x, y;
};

struct vectorf {
	float x, y;
};

struct particle {
	int color;
	struct vectorf position;
	struct vectorf velocity;
};

void init_particles(struct particle **particles);
void spawn_particles(struct particle **particles, struct vector size);

float get_magnitude(float x, float y);
bool in_range_walls(struct vectorf a, struct vectorf b, struct vectorf *diff, float *magnitude, struct vector size);
bool in_range(struct vectorf a, struct vectorf b, struct vectorf *diff, float *mag);
float get_force(float distance, float attraction);
float get_attraction(int a, int b);
void step_particles(struct particle **particles, int frame_duration, struct vector size);

SDL_Color get_color(int index);
void draw_particles(struct particle **particles, SDL_Renderer *renderer);

#endif
