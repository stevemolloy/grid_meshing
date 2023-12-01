#ifndef _SIM_H
#define _SIM_H

#include <stdlib.h>
#include <math.h>

#include "meshing.h"

void timestep_sim(Spring *springs, size_t num_springs, Point *points, size_t num_points, float dt);

#endif // !_SIM_H

