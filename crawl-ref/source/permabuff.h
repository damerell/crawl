// Anything new pertaining to permabuffs
#pragma once
#include "duration-type.h"

#define PERMA_DURATION_DIVISOR 6

enum permabuff_type {
	PERMA_NO_PERMA,
	PERMA_INFUSION,
	PERMA_SHROUD,
	PERMA_SONG,
	PERMA_REGEN,
	PERMA_PPROJ,
	PERMA_DMSL,
	PERMA_EXCRU,
	PERMA_DCHAN,
	PERMA_ROF,
	PERMA_BATTLESPHERE,
	PERMA_APPENDAGE,
	PERMA_FIRST_PERMA = PERMA_INFUSION,
	// Don't FORGET TO CHANGE THIS, David
	PERMA_LAST_PERMA = PERMA_APPENDAGE,
    };

static const duration_type permabuff_durs[] = {
    NUM_DURATIONS, // bah, hope this won't be used
    DUR_INFUSION,
    DUR_SHROUD_OF_GOLUBRIA,
    DUR_SONG_OF_SLAYING,
    DUR_REGENERATION,
    DUR_PORTAL_PROJECTILE,
    DUR_DEFLECT_MISSILES,
    DUR_EXCRUCIATING_WOUNDS,
    DUR_DEATH_CHANNEL,
    DUR_FIRE_SHIELD, // bah
    DUR_BATTLESPHERE,
    DUR_APPENDAGE, // new
};

static const spell_type permabuff_spell[] = {
    NUM_SPELLS,
    SPELL_INFUSION,
    SPELL_SHROUD_OF_GOLUBRIA,
    SPELL_SONG_OF_SLAYING,
    SPELL_REGENERATION,
    SPELL_PORTAL_PROJECTILE,
    SPELL_DEFLECT_MISSILES,
    SPELL_EXCRUCIATING_WOUNDS,
    SPELL_DEATH_CHANNEL,
    SPELL_RING_OF_FLAMES,
    SPELL_BATTLESPHERE,
    SPELL_BEASTLY_APPENDAGE,
};

// These PBs charge you MP regeneration based on their nominal duration.
// Others don't - Regen has its own MP reservoir, and Isk's Battlesphere
// charges you for charges.
// They all charge you hunger, however.
static const permabuff_type pb_ordinary_mpregen[] = {
    PERMA_INFUSION,
    PERMA_SHROUD,
    PERMA_SONG,
    PERMA_PPROJ,
    PERMA_DMSL,
    PERMA_EXCRU,
    PERMA_DCHAN,
    PERMA_ROF,
    PERMA_APPENDAGE,
};
static const int size_mpregen_pb = ARRAYSZ(pb_ordinary_mpregen);
