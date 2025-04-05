#include "main.h"
#include "pros/imu.hpp"
#include <math.h>

// === Inertial Sensor ===
pros::Imu inertial_sensor(2); // Update port if needed

// === 3D Vector ===
struct Vec3 {
  float x, y, z;
};

struct Vec2 {
  int x, y;
};

// === Cube Vertices and Edges ===
Vec3 cubeVertices[] = {{-60, -60, -60}, {60, -60, -60}, {60, 60, -60},
                       {-60, 60, -60},  {-60, -60, 60}, {60, -60, 60},
                       {60, 60, 60},    {-60, 60, 60}};

int edges[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                  {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

// === Rotation Angles (in radians) ===
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

// === Screen Center ===
const int centerX = 240;
const int centerY = 136;
const float scale = 1.5f;

// === Project 3D to 2D ===
Vec2 project(const Vec3 &p) {
  float fov = 256.0f;
  float distance = 100.0f;

  float factor = fov / (distance + p.z);
  return {static_cast<int>(p.x * factor + centerX),
          static_cast<int>(p.y * factor + centerY)};
}

// === Draw the Cube ===
void drawCube() {
  Vec2 projected[8];

  for (int i = 0; i < 8; i++) {
    Vec3 p = cubeVertices[i];

    // Rotate around X
    float y = p.y * cos(angleX) - p.z * sin(angleX);
    float z = p.y * sin(angleX) + p.z * cos(angleX);
    p.y = y;
    p.z = z;

    // Rotate around Y
    float x = p.x * cos(angleY) + p.z * sin(angleY);
    z = -p.x * sin(angleY) + p.z * cos(angleY);
    p.x = x;
    p.z = z;

    // Rotate around Z
    float tempX = p.x * cos(angleZ) - p.y * sin(angleZ);
    float tempY = p.x * sin(angleZ) + p.y * cos(angleZ);
    p.x = tempX;
    p.y = tempY;

    // ✅ Push cube away from the camera
    p.z += 200;

    projected[i] = project(p);
  }

  // Clear screen
  lv_obj_clean(lv_scr_act());

  // Draw lines
  for (int i = 0; i < 12; i++) {
    Vec2 p1 = projected[edges[i][0]];
    Vec2 p2 = projected[edges[i][1]];

    lv_point_t *line_points = new lv_point_t[2];
    line_points[0] = lv_point_t{(lv_coord_t)p1.x, (lv_coord_t)p1.y};
    line_points[1] = lv_point_t{(lv_coord_t)p2.x, (lv_coord_t)p2.y};

    lv_obj_t *line = lv_line_create(lv_scr_act());
    lv_line_set_points(line, line_points, 2);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_color(line, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_line_rounded(line, false, 0);
  }
}

// === Cube Animation Loop ===
void cube_task() {
  while (true) {
    // Get pitch (X), roll (Y), and yaw (Z) in radians
    float pitch = inertial_sensor.get_pitch();
    float roll = inertial_sensor.get_roll();
    float yaw = inertial_sensor.get_yaw();

    // Convert to radians and apply sensitivity
    float sensitivity = 0.8f;
    angleX = pitch * M_PI / 180.0f * sensitivity;
    angleY = roll * M_PI / 180.0f * sensitivity;
    angleZ = yaw * M_PI / 180.0f * sensitivity;

    drawCube();

    pros::delay(33); // ~30 FPS
  }
}

// === Initialize Function ===
void initialize() {
  inertial_sensor.reset();
  while (inertial_sensor.is_calibrating()) {
    pros::delay(100);
  }

  lv_init();
  pros::delay(500);

  pros::Task cubeLoop(cube_task);
}
