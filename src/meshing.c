#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "meshing.h"

#define PI 3.14159265359

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

Point new_point(float mass) {
  Point result = {0};
  result.mass = mass;
  return result;
}

size_t find_points(Circle circle, Point *points, size_t max_dim) {
  size_t pt_ind = 0;
  size_t num_rings = ceil(circle.radius / max_dim);

  points[pt_ind] = new_point(1.0);
  points[pt_ind].x=circle.x;
  points[pt_ind].y=circle.y;
  pt_ind++;

  for (size_t ring=1; ring<num_rings+1; ring++) {
    double angle_start = 0.0f;
    double angle_step = 60.0 / ring;
    for (size_t pt=0; pt<ring*6; pt++) {
      double angle_deg = angle_start + pt * angle_step;
      double angle_rad = angle_deg * PI / 180.0;
      points[pt_ind] = new_point(1.0);
      points[pt_ind].x = circle.x + ring * (circle.radius / num_rings) * cos(angle_rad);
      points[pt_ind].y = circle.y + ring * (circle.radius / num_rings) * sin(angle_rad);
      pt_ind++;
    }
  }

  return pt_ind;
}

Spring new_spring(float k) {
  Spring result = {0};
  result.k = k;
  return result;
}

size_t find_springs(Circle circle, Spring *springs, Point *points, size_t max_dim) {
  float default_k = 100.0;
  size_t spring_ctr = 0;
  size_t num_rings = ceil(circle.radius / max_dim);

  for (size_t ring=1; ring<=num_rings; ring++) {
    size_t first = first_ring_point(ring);
    size_t last = last_ring_point(ring);
    springs[spring_ctr] = new_spring(default_k);
    springs[spring_ctr].pt1 = &points[first];
    springs[spring_ctr].pt2 = &points[last];
    spring_ctr++;
    for (size_t pt=first; pt<=last; pt++) {
      if (pt!=first) {
        springs[spring_ctr] = new_spring(default_k);
        springs[spring_ctr].pt1 = &points[pt];
        springs[spring_ctr].pt2 = &points[pt-1];
        spring_ctr++;
      }
    }
  }

  size_t start, end;
  for (size_t ring=0; ring<num_rings; ring++) {
    if (ring==0) {
      for (size_t i=0; i<6; i++) {
        springs[spring_ctr] = new_spring(default_k);
        springs[spring_ctr].pt1 = &points[0];
        springs[spring_ctr].pt2 = &points[i+1];
        spring_ctr++;
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

      springs[spring_ctr] = new_spring(default_k);
      springs[spring_ctr].pt1 = &points[start];
      springs[spring_ctr].pt2 = &points[end];
      spring_ctr++;
      for (size_t j=0; j<quads_per_triangle; j++) {

        end++;
        springs[spring_ctr] = new_spring(default_k);
        springs[spring_ctr].pt1 = &points[start];
        springs[spring_ctr].pt2 = &points[end];
        spring_ctr++;

        first++;
        start = first>last ? first_ring_point(ring) : first;

        springs[spring_ctr] = new_spring(default_k);
        springs[spring_ctr].pt1 = &points[start];
        springs[spring_ctr].pt2 = &points[end];
        spring_ctr++;
      }
      extra_step++;
    }
  }

  return spring_ctr;
}

void draw_spring(Olivec_Canvas oc, Spring spring, uint32_t colour, size_t spring_ctr) {
  olivec_line(oc, spring.pt1->x, spring.pt1->y, spring.pt2->x, spring.pt2->y, colour);
  char str[10];
  sprintf(str, "%d", (int)spring_ctr);
  Point spring_midpt = get_spring_centre(spring);
  olivec_text(oc, str, spring_midpt.x, spring_midpt.y, olivec_default_font, 1, GREEN);
}

size_t calc_num_points(Circle circle, size_t max_dim) {
  size_t num_rings = ceil(circle.radius / max_dim);
  size_t num_points = 1 + (6/2) * num_rings * (num_rings+1);

  return num_points;
}

size_t calc_num_springs(Circle circle, size_t max_dim) {
  size_t num_rings = ceil(circle.radius / max_dim);
  return 3*num_rings * (3*num_rings + 1);
}


