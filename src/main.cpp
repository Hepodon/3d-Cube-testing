#include "main.h"
#include <cmath>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 240
#define CUBE_SIZE 50

// Cube 3D point struct
struct Vec3 {
  float x, y, z;
};

// 2D projected point
struct Vec2 {
  int x, y;
};

// Cube points
Vec3 cubeVertices[] = {{-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
                       {1, -1, -1},  {1, -1, 1},  {1, 1, -1},  {1, 1, 1}};

// Cube edges (index pairs from vertices)
int edges[][2] = {{0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3},
                  {2, 6}, {3, 7}, {4, 5}, {4, 6}, {5, 7}, {6, 7}};

// Rotation angles
float angleX = 0, angleY = 0;

// Projects a 3D point to 2D using simple perspective
Vec2 project(Vec3 point) {
  float distance = 3.5f;
  float factor = CUBE_SIZE / (point.z + distance);
  int x =
      static_cast<int>(point.x * factor + static_cast<float>(SCREEN_WIDTH) / 2);
  int y = static_cast<int>(point.y * factor +
                           static_cast<float>(SCREEN_HEIGHT) / 2);
  return {x, y};
}

// Rotate the cube and draw
void drawCube() {
  Vec2 projected[8];
  for (int i = 0; i < 8; i++) {
    Vec3 p = cubeVertices[i];

    // Rotate around X axis
    float y = p.y * cos(angleX) - p.z * sin(angleX);
    float z = p.y * sin(angleX) + p.z * cos(angleX);
    p.y = y;
    p.z = z;

    // Rotate around Y axis
    float x = p.x * cos(angleY) + p.z * sin(angleY);
    z = -p.x * sin(angleY) + p.z * cos(angleY);
    p.x = x;
    p.z = z;

    projected[i] = project(p);
  }

  // Clear the screen
  lv_obj_clean(lv_scr_act());

  // Draw edges
  for (int i = 0; i < 12; i++) {
    Vec2 p1 = projected[edges[i][0]];
    Vec2 p2 = projected[edges[i][1]];

    lv_obj_t *line = lv_line_create(lv_scr_act());
    static lv_point_t line_points[2];
    line_points[0] = {static_cast<lv_coord_t>(p1.x),
                      static_cast<lv_coord_t>(p1.y)};
    line_points[1] = {static_cast<lv_coord_t>(p2.x),
                      static_cast<lv_coord_t>(p2.y)};

    lv_line_set_points(line, line_points, 2);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_color(line, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_line_rounded(line, false, 0);
  }
}

// Task that updates the cube every frame
void cube_task(void *) {
  while (true) {
    drawCube();
    angleX += 0.03f;
    angleY += 0.05f;
    pros::delay(30); // ~33 FPS
  }
}

void initialize() {
  lv_init();
  pros::Task cubeLoop(cube_task);
}
