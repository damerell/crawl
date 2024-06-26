/**
 * @file
 * @brief data handlers for player spell list
**/

#pragma once

#include <functional>

#include "enum.h"
#include "mon-info.h"
#include "permabuff.h"

enum spschool_flag_type
{
  SPTYP_NONE           = 0,
  SPTYP_CONJURATION    = 1<<0,
  SPTYP_HEXES          = 1<<1,
  SPTYP_CHARMS         = 1<<2,
  SPTYP_FIRE           = 1<<3,
  SPTYP_ICE            = 1<<4,
  SPTYP_TRANSMUTATION  = 1<<5,
  SPTYP_NECROMANCY     = 1<<6,
  SPTYP_SUMMONING      = 1<<7,
  SPTYP_TRANSLOCATION  = 1<<8,
  SPTYP_POISON         = 1<<9,
  SPTYP_EARTH          = 1<<10,
  SPTYP_AIR            = 1<<11,
  SPTYP_LAST_SCHOOL    = SPTYP_AIR,
  SPTYP_RANDOM         = SPTYP_LAST_SCHOOL << 1,
};
DEF_BITFIELD(spschools_type, spschool_flag_type, 11);
const int SPTYP_LAST_EXPONENT = spschools_type::last_exponent;
COMPILE_CHECK(spschools_type::exponent(SPTYP_LAST_EXPONENT)
              == SPTYP_LAST_SCHOOL);

struct bolt;
class dist;
struct direction_chooser_args;

enum spell_highlight_colours
{
    COL_UNKNOWN      = LIGHTGRAY,   // spells for which no known brand applies.
    COL_UNMEMORIZED  = LIGHTBLUE,   // spell hasn't been memorized (used reading spellbooks)
    COL_MEMORIZED    = LIGHTGRAY,   // spell has been memorized
    COL_USELESS      = DARKGRAY,    // ability would have no useful effect
    COL_INAPPLICABLE = COL_USELESS, // ability cannot be meanifully applied (eg, no targets)
    COL_FORBIDDEN    = LIGHTRED,    // The player's god hates this ability
    COL_EMPOWERED    = LIGHTGREEN,  // The ability is made stronger by the player's status
    COL_DANGEROUS    = LIGHTRED,    // ability/spell use could be dangerous
};

bool is_valid_spell(spell_type spell);
void init_spell_descs();
void init_spell_name_cache();
spell_type spell_by_name(string name, bool partial_match = false);

spschool_flag_type school_by_name(string name);

int get_spell_slot_by_letter(char letter);
int get_spell_letter(spell_type spell);
spell_type get_spell_by_letter(char letter);

bool add_spell_to_memory(spell_type spell);
bool del_spell_from_memory_by_slot(int slot);
bool del_spell_from_memory(spell_type spell);

int spell_hunger(spell_type which_spell);
int spell_mana(spell_type which_spell);
int spell_difficulty(spell_type which_spell);
bool spell_range_varies(spell_type spell);
int spell_power_cap(spell_type spell);
int spell_range(spell_type spell, int pow, bool allow_bonus = true);
int spell_noise(spell_type spell);
int spell_effect_noise(spell_type spell);
bool spell_is_empowered(spell_type spell);

const char *get_spell_target_prompt(spell_type which_spell);
tileidx_t get_spell_tile(spell_type which_spell);

bool spell_is_direct_explosion(spell_type spell);
bool spell_harms_target(spell_type spell);
bool spell_harms_area(spell_type spell);
int spell_levels_required(spell_type which_spell);

unsigned int get_spell_flags(spell_type which_spell);

bool spell_typematch(spell_type which_spell, spschool_flag_type which_disc);
spschools_type get_spell_disciplines(spell_type which_spell);
int count_bits(uint64_t bits);

template <class E, int Exp>
int count_bits(enum_bitfield<E, Exp> bits)
{
    return count_bits(bits.flags);
}

const char *spell_title(spell_type which_spell);
const char* spelltype_short_name(spschool_flag_type which_spelltype);
const char* spelltype_long_name(spschool_flag_type which_spelltype);

typedef function<int (coord_def where)> cell_func;
typedef function<int (coord_def where, int pow, int spreadrate,
                       cloud_type type, const actor* agent, int excl_rad)>
        cloud_func;

int apply_area_visible(cell_func cf, const coord_def& where);

int apply_random_around_square(cell_func cf, const coord_def& where,
                               bool hole_in_middle, int max_targs);

void apply_area_cloud(cloud_func func, const coord_def& where,
                      int pow, int number, cloud_type ctype,
                      const actor *agent, int spread_rate = -1,
                      int excl_rad = -1);

bool spell_direction(dist &spelld, bolt &pbolt,
                     direction_chooser_args *args = nullptr);

skill_type spell_type2skill(spschool_flag_type spelltype);
spschool_flag_type skill2spell_type(skill_type spell_skill);

skill_type arcane_mutation_to_skill(mutation_type mutation);
bool cannot_use_schools(spschools_type schools);

bool spell_is_form(spell_type spell) PURE;

bool spell_is_useless(spell_type spell, bool temp = true,
                      bool prevent = false, bool fake_spell = false) PURE;
string spell_uselessness_reason(spell_type spell, bool temp = true,
                                bool prevent = false,
                                bool fake_spell = false) PURE;

int spell_highlight_by_utility(spell_type spell,
			       int default_colour = COL_UNKNOWN,
			       bool transient = false,
			       bool memcheck = false,
			       bool permacancel = false);
bool spell_no_hostile_in_range(spell_type spell);

bool spell_is_soh_breath(spell_type spell);
const vector<spell_type> *soh_breath_spells(spell_type spell);

permabuff_type permabuff_is(spell_type spell); // sigh
bool is_permabuff(spell_type spell);
// return value is number of auts additional tracking added
int permabuff_track(int pb);
bool permabuff_uses_charms_reserve(permabuff_type pb);

int nominal_duration(spell_type spell);
