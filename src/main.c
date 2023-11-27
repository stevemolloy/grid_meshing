#include <stdio.h>
#include <math.h>

#define OLIVEC_IMPLEMENTATION
#define OLIVEC_AA_RES 2
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "olive.c"
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 800

#define BACKGROUND_COLOUR 0xFF202020
#define BLUE  0xFFA8661F
#define GREEN 0xFF20FF20
#define RED   0xFF2020FF
#define WHITE 0xFFFFFFFF

#define PI 3.14159265359
#define MAX_DIMENSION 60

#define draw_point(oc, x, y, colour) \
  olivec_circle((oc), (x), (y), 5, (colour))

size_t spring_ctr = 0;

typedef struct {
  double x;
  double y;
  double radius;
} Circle;

typedef struct {
  double x;
  double y;
} Point;

typedef struct {
  Point *pt1;
  Point *pt2;
} Spring;

size_t first_ring_point(size_t ring_num) {
  assert(ring_num > 0);
  return 3*ring_num * (ring_num - 1) + 1;
}

size_t last_ring_point(size_t ring_num) {
  assert(ring_num > 0);
  return first_ring_point(ring_num + 1) - 1;
}

Point get_spring_centre(Spring spring) {
  Point pt1 = *spring.pt1;
  Point pt2 = *spring.pt2;
  Point result = {
    .x = (pt1.x + pt2.x)/2,
    .y = (pt1.y + pt2.y)/2,
  };
  return result;
}

void draw_spring(Olivec_Canvas oc, Spring spring, uint32_t colour) {
  spring_ctr++;
  olivec_line(oc, spring.pt1->x, spring.pt1->y, spring.pt2->x, spring.pt2->y, colour);
  char str[10];
  sprintf(str, "%d", (int)spring_ctr);
  Point spring_midpt = get_spring_centre(spring);
  olivec_text(oc, str, spring_midpt.x, spring_midpt.y, olivec_default_font, 1, GREEN);
}

int main(void) {
  const char *filename = "testing.png";
  int png_comp = 4;
  uint32_t pixels[WIDTH*HEIGHT];

  Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
  olivec_fill(oc, BACKGROUND_COLOUR);

  Circle circle = {.x = WIDTH/2.0, .y = HEIGHT/2.0, .radius = HEIGHT/3.0};
  size_t num_rings = ceil(circle.radius / MAX_DIMENSION);
  double step = circle.radius / num_rings;
  size_t num_points = 1 + (6/2) * num_rings * (num_rings+1);

  olivec_circle(oc, (int)circle.x, (int)circle.y, (int)circle.radius, BLUE);

  Point points[num_points];
  size_t pt_ind = 0;

  points[pt_ind++] = (Point){.x=circle.x, .y=circle.y};
  for (size_t ring=1; ring<num_rings+1; ring++) {
    double angle_start = 0.0f;
    double angle_step = 60.0 / ring;
    for (size_t pt=0; pt<ring*6; pt++) {
      double angle_deg = angle_start + pt * angle_step;
      double angle_rad = angle_deg * PI / 180.0;
      points[pt_ind] = (Point){
        .x = circle.x + ring*step*cos(angle_rad),
        .y = circle.y + ring*step*sin(angle_rad),
      };
      pt_ind++;
    }
  }
  for (size_t i=0; i<num_points; i++) {
    draw_point(oc, points[i].x, points[i].y, RED);
    // char str[10];
    // sprintf(str, "%d", (int)i);
    // olivec_text(oc, str, points[i].x, points[i].y, olivec_default_font , 2, WHITE);
  }

  Spring spring;
  for (size_t ring=1; ring<=num_rings; ring++) {
    size_t first = first_ring_point(ring);
    size_t last = last_ring_point(ring);
    spring = (Spring) { .pt1 = &points[first], .pt2 = &points[last] };
    draw_spring(oc, spring, WHITE);
    for (size_t pt=first; pt<=last; pt++) {
      if (pt!=first) {
        spring = (Spring) { .pt1 = &points[pt], .pt2 = &points[pt-1] };
        draw_spring(oc, spring, WHITE);
      }
    }
  }

  size_t start, end;
  for (size_t ring=0; ring<num_rings; ring++) {
    if (ring==0) {
      for (size_t i=0; i<6; i++) {
        spring = (Spring) { .pt1 = &points[0], .pt2 = &points[i+1] };
        draw_spring(oc, spring, WHITE);
      }
      continue;
    }
    size_t first = first_ring_point(ring);
    size_t last = last_ring_point(ring);
    size_t num_quads = 6;
    size_t quads_per_triangle = ring;
    size_t pts_in_ring = last_ring_point(ring) - first_ring_point(ring) + 1;
    size_t extra_step = 0;
    for (size_t i=0; i<num_quads; i++) {
      start = first>last ? first_ring_point(ring) : first;
      end = first+pts_in_ring+extra_step;

      spring = (Spring) { .pt1 = &points[start], .pt2 = &points[end] };
      draw_spring(oc, spring, WHITE);
      for (size_t j=0; j<quads_per_triangle; j++) {

        end++;
        spring = (Spring) { .pt1 = &points[start], .pt2 = &points[end] };
        draw_spring(oc, spring, WHITE);

        first++;
        start = first>last ? first_ring_point(ring) : first;

        spring = (Spring) { .pt1 = &points[start], .pt2 = &points[end] };
        draw_spring(oc, spring, WHITE);
      }
      extra_step++;
    }
  }

  printf("Num of springs = %zu\n", spring_ctr);
  stbi_write_png(filename, (int)oc.width, (int)oc.height, png_comp, oc.pixels, 4*(int)oc.stride);

  return 0;
}

