/**
 * @file
 * @brief Throwing and launching stuff.
**/

#pragma once

#include <string>

#include "enum.h"

enum fire_type
{
    FIRE_NONE      = 0x0000,
    FIRE_LAUNCHER  = 0x0001,
    FIRE_DART      = 0x0002,
    FIRE_STONE     = 0x0004,
    FIRE_JAVELIN   = 0x0010,
    FIRE_ROCK      = 0x0100,
    FIRE_NET       = 0x0200,
    FIRE_BOOMERANG = 0x0800,
    FIRE_INSCRIBED = 0x1000,   // Only used for _get_fire_order
};

struct bolt;
class dist;

bool is_penetrating_attack(const actor& attacker, const item_def* weapon,
                           const item_def& projectile);
bool item_is_quivered(const item_def &item);
bool fire_warn_if_impossible(bool silent = false, bool ihpixammo = false);
int get_next_fire_item(int current, int offset);
int get_ammo_to_shoot(int item, dist &target, bool with_ihpix,
		      bool teleport = false);
bool is_pproj_active();
void fire_thing(int item = -1);
void throw_item_no_quiver();

bool throw_it(bolt &pbolt, int throw_2, bool with_ihpix, 
	      dist *target = nullptr);

bool thrown_object_destroyed(item_def *item, const coord_def& where);

void setup_monster_throw_beam(monster* mons, bolt &beam);
bool mons_throw(monster* mons, bolt &beam, int msl, bool teleport = false);
