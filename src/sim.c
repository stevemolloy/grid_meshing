#include "sim.h"

void timestep_sim(Spring *springs, size_t num_springs, Point *points, size_t num_points, float dt) {
  for (size_t i=0; i<num_springs; i++) {
    Spring spr = springs[i];
    Point *pt1 = spr.pt1;
    Point *pt2 = spr.pt2;

    float force = (pt1->z - pt2->z) * spr.k;
    pt1->dv_z = -force * dt / pt1->mass;
    pt2->dv_z =  force * dt / pt2->mass;

    pt1->dz = pt1->v_z * dt;
    pt2->dz = pt2->v_z * dt;
  }

  for (size_t i=0; i<num_points; i++) {
    points[i].z   += points[i].dz;
    points[i].v_z += points[i].dv_z;
  }
}

