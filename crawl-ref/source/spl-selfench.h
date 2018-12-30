
#pragma once

#include "spl-cast.h"
#include "permabuff.h"
#include "transformation.h"

#define SHROUD_RECHARGE "shroud_recharge"
#define DMSL_RECHARGE "dmsl_recharge"
#define MP_TO_CHARMS "mp_to_charms"
#define PPROJ_DEBT "pproj_debt"
#define CHARMS_DEBT "charms_debt"
#define REGEN_RESERVE "regen_reserve"
#define CHARMS_ALL_MPREGEN "charms_all_mpregen"
#define REGEN_REPORTING_PERCENT "regen_reporting_percent"
#define APPENDAGE_TIME "appendage_time"

int allowed_deaths_door_hp();
spret cast_deaths_door(int pow, bool fail);
void remove_ice_armour();
spret ice_armour(int pow, bool fail);

int harvest_corpses(const actor &harvester,
                    bool dry_run = false, bool defy_god = false);
spret corpse_armour(int pow, bool fail);

spret deflection(int pow, bool fail);

spret cast_regen(int pow, bool fail);
spret cast_revivification(int pow, bool fail);

spret cast_swiftness(int power, bool fail);

int cast_selective_amnesia(const string &pre_msg = "");
spret cast_silence(int pow, bool fail = false);

spret cast_infusion(int pow, bool fail);
spret cast_song_of_slaying(int pow, bool fail);
void check_sos_miscast();
spret cast_appendage(int pow, bool fail);

spret cast_liquefaction(int pow, bool fail);
spret cast_shroud_of_golubria(int pow, bool fail);
spret cast_transform(int pow, transformation which_trans, bool fail);

void spell_drop_permabuffs(bool turn_off, bool end_durs, bool increase_durs,
			   bool voluntary);
bool permabuff_fail_check(permabuff_type pb, const string &message, 
			  bool ignoredur = false);
