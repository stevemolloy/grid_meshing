#include <stdio.h>
#include <math.h>

#define OLIVEC_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "olive.c"
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

#define BACKGROUND_COLOUR 0xFF202020
#define RED  0xFF2020FF
#define BLUE 0xFFFF2020

#define PI 3.14159265359
#define MAX_DIMENSION 10

#define draw_point(oc, pt, colour) olivec_circle((oc), (pt.x), (pt.y), 1, (colour))

typedef struct {
  double x;
  double y;
  double radius;
} Circle;

typedef struct {
  double x;
  double y;
} Point;

double circle_circumference(Circle circle) {
  return PI * circle.radius * circle.radius;
}

int main(void) {
  const char *filename = "testing.png";
  int png_comp = 4;
  uint32_t pixels[WIDTH*HEIGHT];

  Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
  olivec_fill(oc, BACKGROUND_COLOUR);

  Circle circle = {.x = WIDTH/2.0, .y = HEIGHT/2.0, .radius = WIDTH/10.0};
  size_t num_steps = ceil(circle.radius / MAX_DIMENSION);
  double step = circle.radius / num_steps;

  olivec_circle(oc, (int)circle.x, (int)circle.y, (int)circle.radius, BLUE);

  Point points[1 + 3*(num_steps*(num_steps+1))];

  size_t ind = 0;
  points[ind++] = (Point){.x=circle.x, .y=circle.y};
  draw_point(oc, points[0], RED);
  for (size_t ring=0; ring<num_steps; ring++) {
    for (size_t angle_deg = 0; angle_deg<360; angle_deg+=60/(ring+1)) {
      double angle_rad = angle_deg * PI / 180.0;
      points[ind] = (Point){
        .x=circle.x + (ring+1)*step*cos(angle_rad),
        .y=circle.y + (ring+1)*step*sin(angle_rad)
      };
      draw_point(oc, points[ind], RED);
      ind++;
    }
  }

  stbi_write_png(filename, (int)oc.width, (int)oc.height, png_comp, oc.pixels, 4*(int)oc.stride);

  return 0;
}

