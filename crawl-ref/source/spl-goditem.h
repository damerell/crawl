#pragma once

#include "enchant-type.h"
#include "holy-word-source-type.h"
#include "spell-type.h"
#include "spl-cast.h"
#include "torment-source-type.h"

spret cast_healing(int pow, bool fail);
bool heal_monster(monster& patient, int amount);

/// List of monster enchantments which can be dispelled.
const enchant_type dispellable_enchantments[] =
{
    ENCH_SLOW,
    ENCH_HASTE,
    ENCH_SWIFT,
    ENCH_MIGHT,
    ENCH_AGILE,
    ENCH_FEAR,
    ENCH_FEAR_INSPIRING,
    ENCH_CONFUSION,
    ENCH_CORONA,
    ENCH_STICKY_FLAME,
    ENCH_CHARM,
    ENCH_PARALYSIS,
    ENCH_PETRIFYING,
    ENCH_PETRIFIED,
    ENCH_REGENERATION,
    ENCH_LOWERED_MR,
    ENCH_SILENCE,
    ENCH_TP,
    ENCH_LIQUEFYING,
    ENCH_INNER_FLAME,
    ENCH_OZOCUBUS_ARMOUR,
    ENCH_WORD_OF_RECALL,
    ENCH_INJURY_BOND,
    ENCH_FLAYED,
    ENCH_WEAK,
    ENCH_DIMENSION_ANCHOR,
    ENCH_TOXIC_RADIANCE,
    ENCH_FIRE_VULN,
    ENCH_POISON_VULN,
    ENCH_AGILE,
    ENCH_FROZEN,
    ENCH_BLACK_MARK,
    ENCH_SHROUD,
    ENCH_SAP_MAGIC,
    ENCH_CORROSION,
    ENCH_REPEL_MISSILES,
    ENCH_DEFLECT_MISSILES,
    ENCH_RESISTANCE,
    ENCH_HEXED,
    ENCH_BRILLIANCE_AURA,
    ENCH_EMPOWERED_SPELLS,
    ENCH_BOUND_SOUL,
    ENCH_INFESTATION,
    ENCH_STILL_WINDS,
    ENCH_CONCENTRATE_VENOM,
    ENCH_MIRROR_DAMAGE,
};

bool player_is_debuffable();
void debuff_player(bool end_permas, bool end_perma_durs, 
		   bool increase_perma_durs);
bool monster_is_debuffable(const monster &mon);
void debuff_monster(monster &mon);

int detect_traps(int pow);
int detect_items(int pow);
int detect_creatures(int pow, bool telepathic = false);
bool remove_curse(bool alreadyknown = true, const string &pre_msg = "");
#if TAG_MAJOR_VERSION == 34
bool curse_item(bool armour, const string &pre_msg = "");
#endif

bool entomb(int pow);
bool cast_imprison(int pow, monster* mons, int source);

bool cast_smiting(int pow, monster* mons);

string unpacifiable_reason(const monster& mon);
string unpacifiable_reason(const monster_info& mi);

struct bolt;

void holy_word(int pow, holy_word_source_type source, const coord_def& where,
               bool silent = false, actor *attacker = nullptr);

void holy_word_monsters(coord_def where, int pow, holy_word_source_type source,
                        actor *attacker = nullptr);
void holy_word_player(holy_word_source_type source);

void torment(actor *attacker, torment_source_type taux, const coord_def& where);
void torment_cell(coord_def where, actor *attacker, torment_source_type taux);
void torment_player(actor *attacker, torment_source_type taux);

void setup_cleansing_flame_beam(bolt &beam, int pow, int caster,
                                coord_def where, actor *attacker = nullptr);
void cleansing_flame(int pow, int caster, coord_def where,
                     actor *attacker = nullptr);

spret cast_random_effects(int pow, bolt& beam, bool fail);
