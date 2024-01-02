#pragma once

#include "dungeon.h"

void dgn_build_labyrinth_level();
void labyrinth_mark_deadends(const dgn_region &region);

bool shrink_lab(int elapsed);
