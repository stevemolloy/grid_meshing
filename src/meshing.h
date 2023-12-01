#ifndef _MESHING_H
#define _MESHING_H

#include <assert.h>
#include <stdio.h>

#define OLIVEC_IMPLEMENTATION
#define OLIVEC_AA_RES 2
#include "olive.c"

#define BACKGROUND_COLOUR 0xFF202020
#define BLUE  0xFFFF3030
#define GREEN 0xFF30FF30
#define RED   0xFF3030FF
#define WHITE 0xFFFFFFFF

#define draw_point(oc, x, y, colour) \
  olivec_circle((oc), (x), (y), 3, (colour))

typedef struct {
  double x;
  double y;
  double radius;
} Circle;

typedef struct {
  double x;
  double y;
  double z;
  double v_z;
  double dz;
  double dv_z;
  double mass;
} Point;

typedef struct {
  Point *pt1;
  Point *pt2;
  double k;
} Spring;

size_t first_ring_point(size_t ring_num);

size_t last_ring_point(size_t ring_num);

Point get_spring_centre(Spring spring);

void draw_spring(Olivec_Canvas oc, Spring spring, uint32_t colour, size_t spring_ctr);

size_t calc_num_points(Circle circle, size_t max_dim);

size_t calc_num_springs(Circle circle, size_t max_dim);

size_t find_points(Circle circle, Point *points, size_t max_dim);

size_t find_springs(Circle circle, Spring *springs, Point *points, size_t max_dim);

#endif // !_MESHING_H

