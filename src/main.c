#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "meshing.h"
#include "sim.h"

#define WIDTH 800
#define HEIGHT 800
#define PNG_COMPONENTS 4
#define MAX_DIMENSION 30

int main(void) {
  // The geometry to mesh
  Circle circle = {.x = WIDTH/2.0, .y = HEIGHT/2.0, .radius = HEIGHT/3.0};

  // Determine the number of points and connections
  size_t num_points = calc_num_points(circle, MAX_DIMENSION);
  size_t num_springs = calc_num_springs(circle, MAX_DIMENSION);

  // Reserve some heap memory
  Point *points   = calloc(num_points, sizeof(Point));
  Spring *springs = calloc(num_springs, sizeof(Spring));

  // MESHING
  find_points(circle, points, MAX_DIMENSION);
  find_springs(circle, springs, points, MAX_DIMENSION);

  float dt = 0.001;
  points[0].z = 100;
  for (size_t i=0; i<500; i++) {
    timestep_sim(springs, num_springs, points, num_points, dt);
    printf("points[0].z = %f\n", points[0].z);
  }

  // Display results graphically with olivec
  uint32_t *pixels = calloc(WIDTH*HEIGHT, sizeof(uint32_t));
  Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
  olivec_fill(oc, BACKGROUND_COLOUR);
  olivec_circle(oc, (int)circle.x, (int)circle.y, (int)circle.radius, BLUE);
  for (size_t i=0; i<num_points; i++) draw_point(oc, points[i].x, points[i].y, RED);
  for (size_t i=0; i<num_springs; i++) draw_spring(oc, springs[i], WHITE, i);

  // Create PNG output
  const char *output_filename = "testing.png";
  stbi_write_png(output_filename, (int)oc.width, (int)oc.height, PNG_COMPONENTS, oc.pixels, 4*(int)oc.stride);

  // Cleanup before exiting
  free(pixels);
  free(springs);
  free(points);

  return 0;
}

