#include "./particles.h"
#include <time.h>

struct particle *particles;

int main() {
	struct vector size = { 640, 480 };

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	SDL_Window *window = SDL_CreateWindow("Particle Life",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			size.x, size.y, SDL_WINDOW_SHOWN);
	if (!window) return 1;

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_DestroyWindow(window);
		return 1;
	}
	
	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(renderer, &renderer_info);

	init_particles(&particles);
	if (!particles) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		return 1;
	}

	srand(time(NULL));

	spawn_particles(&particles, size);

	SDL_Event event;
	int quit = 0, time = 0, frame_duration = 0;

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						if (event.window.data1 != size.x || event.window.data2 != size.y) {
							size.x = event.window.data1;
							size.y = event.window.data2;
							spawn_particles(&particles, size);
						}
					}
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		int new_time = SDL_GetTicks();
		frame_duration = new_time - time;
		time = new_time;
		step_particles(&particles, frame_duration, size);

		draw_particles(&particles, renderer);

		SDL_RenderPresent(renderer);
	}

	free(particles);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
