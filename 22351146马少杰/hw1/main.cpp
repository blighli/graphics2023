#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>

SDL_Window *window = nullptr;
SDL_GLContext context;

float triangleX = 0.0f;
float triangleY = 0.0f;
float moveSpeed = 0.1f;

void render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(triangleX, triangleY, 0.0f);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    glPopMatrix();

    SDL_GL_SwapWindow(window);
}

void handleInput(SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        SDL_Quit();
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                triangleY += moveSpeed;
                break;
            case SDLK_DOWN:
                triangleY -= moveSpeed;
                break;
            case SDLK_LEFT:
                triangleX -= moveSpeed;
                break;
            case SDLK_RIGHT:
                triangleX += moveSpeed;
                break;
            default:
                break;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("22351146",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    context = SDL_GL_CreateContext(window);

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            handleInput(event);
        }

        render();
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
