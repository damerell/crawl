#include "AppHdr.h"

#include "status.h"

#include "areas.h"
#include "branch.h"
#include "cloud.h"
#include "duration-type.h"
#include "env.h"
#include "evoke.h"
#include "food.h"
#include "god-abil.h"
#include "god-passive.h"
#include "item-prop.h"
#include "level-state-type.h"
#include "mon-transit.h" // untag_followers() in duration-data
#include "mutation.h"
#include "options.h"
#include "orb.h" // orb_limits_translocation in fill_status_info
#include "player-equip.h"
#include "player-stats.h"
#include "random.h" // for midpoint_msg.offset() in duration-data
#include "religion.h"
#include "spl-selfench.h"
#include "spl-summoning.h" // NEXT_DOOM_HOUND_KEY in duration-data
#include "spl-transloc.h"
#include "spl-wpnench.h" // for _end_weapon_brand() in duration-data
#include "stringutil.h"
#include "throw.h"
#include "transform.h"
#include "traps.h"

#include "duration-data.h"

static int duration_index[NUM_DURATIONS];

void init_duration_index()
{
    COMPILE_CHECK(ARRAYSZ(duration_data) == NUM_DURATIONS);
    for (int i = 0; i < NUM_DURATIONS; ++i)
        duration_index[i] = -1;

    for (unsigned i = 0; i < ARRAYSZ(duration_data); ++i)
    {
        duration_type dur = duration_data[i].dur;
        ASSERT_RANGE(dur, 0, NUM_DURATIONS);
        // Catch redefinitions.
        ASSERT(duration_index[dur] == -1);
        duration_index[dur] = i;
    }
}

static const duration_def* _lookup_duration(duration_type dur)
{
    ASSERT_RANGE(dur, 0, NUM_DURATIONS);
    if (duration_index[dur] == -1)
        return nullptr;
    else
        return &duration_data[duration_index[dur]];
}

const char *duration_name(duration_type dur)
{
    return _lookup_duration(dur)->name();
}

bool duration_dispellable(duration_type dur)
{
    return _lookup_duration(dur)->duration_has_flag(D_DISPELLABLE);
}

static int _bad_ench_colour(int lvl, int orange, int red)
{
    if (lvl >= red)
        return RED;
    else if (lvl >= orange)
        return LIGHTRED;

    return YELLOW;
}

static int _dur_colour(int exp_colour, bool expiring)
{
    if (expiring)
        return exp_colour;
    else
    {
        switch (exp_colour)
        {
        case GREEN:
            return LIGHTGREEN;
        case BLUE:
            return LIGHTBLUE;
        case MAGENTA:
            return LIGHTMAGENTA;
        case LIGHTGREY:
            return WHITE;
        default:
            return exp_colour;
        }
    }
}

static void _mark_expiring(status_info& inf, bool expiring)
{
    if (expiring)
    {
        if (!inf.short_text.empty())
            inf.short_text += " (expiring)";
        if (!inf.long_text.empty())
            inf.long_text = "Expiring: " + inf.long_text;
    }
}

static string _ray_text()
{
    // i feel like we could do this with math instead...
    switch (you.attribute[ATTR_SEARING_RAY])
    {
        case 2:
            return "Ray+";
        case 3:
            return "Ray++";
        default:
            return "Ray";
    }
}

/**
 * Populate a status_info struct from the duration_data struct corresponding
 * to the given duration_type.
 *
 * @param[in] dur    The duration in question.
 * @param[out] inf   The status_info struct to be filled.
 * @return           Whether a duration_data struct was found.
 */
static bool _fill_inf_from_ddef(duration_type dur, status_info& inf)
{
    const duration_def* ddef = _lookup_duration(dur);
    if (!ddef)
        return false;

    inf.light_colour = ddef->light_colour;
    inf.light_text   = ddef->light_text;
    inf.short_text   = ddef->short_text;
    inf.long_text    = ddef->long_text;
    if (ddef->duration_has_flag(D_EXPIRES))
    {
        inf.light_colour = _dur_colour(inf.light_colour, dur_expiring(dur));
        _mark_expiring(inf, dur_expiring(dur));
    }

    return true;
}

static void _describe_airborne(status_info& inf);
static void _describe_glow(status_info& inf);
static void _describe_hunger(status_info& inf);
static void _describe_regen(status_info& inf);
static void _describe_rotting(status_info& inf);
static void _describe_sickness(status_info& inf);
static void _describe_speed(status_info& inf);
static void _describe_poison(status_info& inf);
static void _describe_transform(status_info& inf);
static void _describe_stat_zero(status_info& inf, stat_type st);
static void _describe_terrain(status_info& inf);
static void _describe_repel_missiles(status_info& inf);
static void _describe_invisible(status_info& inf);

bool fill_status_info(int status, status_info& inf)
{
    inf = status_info();

    bool found = false;

    // Sort out inactive durations, and fill in data from duration_data
    // for the simple durations.
    if (status >= 0 && status < NUM_DURATIONS)
    {
        duration_type dur = static_cast<duration_type>(status);

        if (!you.duration[dur])
            return false;

        found = _fill_inf_from_ddef(dur, inf);
    }

    // Now treat special status types and durations, possibly
    // completing or overriding the defaults set above.
    switch (status)
    {
    case STATUS_DIVINE_ENERGY:
        if (you.duration[DUR_NO_CAST])
        {
            inf.light_colour = RED;
            inf.light_text   = "-Cast";
            inf.short_text   = "no casting";
            inf.long_text    = "You are unable to cast spells.";
        }
        else if (you.attribute[ATTR_DIVINE_ENERGY])
        {
            inf.light_colour = WHITE;
            inf.light_text   = "+Cast";
            inf.short_text   = "divine energy";
            inf.long_text    = "You are calling on Sif Muna for divine "
                               "energy.";
        }
        break;

    case STATUS_IHPIX_INFUSE:
        if (you.attribute[ATTR_IHPIX_INFUSE]) {
            inf.light_text = "Ranged+";
            inf.short_text = "infusing ranged";
            if (enough_mp(1, true, false)) {
                inf.light_colour = LIGHTBLUE;
                inf.long_text = 
                "You are channeling magical energy into your ranged attacks.";
            } else {
                inf.light_colour = BLUE;
                inf.long_text = 
                "You have no magical energy to support your ranged attacks.";
                inf.short_text = "infusing ranged (no MP)";
            }
        }
        break;
        
    case DUR_CORROSION:
        inf.light_text = make_stringf("Corr (%d)",
                          (-4 * you.props["corrosion_amount"].get_int()));
        break;

    case DUR_NO_POTIONS:
        if (you_foodless())
            inf.light_colour = DARKGREY;
        break;

    case DUR_SWIFTNESS:
        if (you.attribute[ATTR_SWIFTNESS] < 0)
        {
            inf.light_text   = "-Swift";
            inf.light_colour = RED;
            inf.short_text   = "sluggish";
            inf.long_text    = "You are moving sluggishly.";
        }
        if (you.in_liquid())
            inf.light_colour = DARKGREY;
        break;

    case STATUS_AIRBORNE:
        _describe_airborne(inf);
        break;

    case STATUS_BEHELD:
        if (you.beheld())
        {
            inf.light_colour = RED;
            inf.light_text   = "Mesm";
            inf.short_text   = "mesmerised";
            inf.long_text    = "You are mesmerised.";
        }
        break;

    case STATUS_CONTAMINATION:
        _describe_glow(inf);
        break;

    case STATUS_BACKLIT:
        if (you.backlit())
        {
            inf.short_text = "glowing";
            inf.long_text  = "You are glowing.";
        }
        break;

    case STATUS_UMBRA:
        if (you.umbra())
        {
            inf.short_text   = "wreathed by umbra";
            inf.long_text    = "You are wreathed by an umbra.";
        }
        break;

    case STATUS_NET:
        if (you.attribute[ATTR_HELD])
        {
            inf.light_colour = RED;
            inf.light_text   = "Held";
            inf.short_text   = "held";
            inf.long_text    = make_stringf("You are %s.", held_status());
        }
        break;

    case STATUS_HUNGER:
        _describe_hunger(inf);
        break;

    case STATUS_REGENERATION:
        // PERMA_REGEN + some vampire and non-healing stuff
        _describe_regen(inf);
        break;

    case STATUS_ROT:
        _describe_rotting(inf);
        break;

    case STATUS_SICK:
        _describe_sickness(inf);
        break;

    case STATUS_SPEED:
        _describe_speed(inf);
        break;

    case STATUS_LIQUEFIED:
    {
        if (you.liquefied_ground())
        {
            inf.light_colour = BROWN;
            inf.light_text   = "SlowM";
            inf.short_text   = "slowed movement";
            inf.long_text    = "Your movement is slowed on this liquid ground.";
        }
        break;
    }

    case STATUS_AUGMENTED:
    {
        int level = augmentation_amount();

        if (level > 0)
        {
            inf.light_colour = (level == 3) ? WHITE :
                               (level == 2) ? LIGHTBLUE
                                            : BLUE;

            inf.light_text = "Aug";
        }
        break;
    }

    case DUR_CONFUSING_TOUCH:
    {
        inf.long_text = you.hands_act("are", "glowing red.");
        break;
    }

    case DUR_POISONING:
        _describe_poison(inf);
        break;

    case DUR_POWERED_BY_DEATH:
    {
        const int pbd_str = you.props[POWERED_BY_DEATH_KEY].get_int();
        if (pbd_str > 0)
        {
            inf.light_colour = LIGHTMAGENTA;
            inf.light_text   = make_stringf("Regen (%d)", pbd_str);
        }
        break;
    }

    case STATUS_DEFLECT_MISSILES:
    {
        if (you.permabuff[PERMA_DMSL] && 
            !you.duration[DUR_DEFLECT_MISSILES]) {
            inf.light_text   = "DMsl";
            inf.short_text   = "deflect missiles";
            if (you.props.exists(DMSL_RECHARGE) &&
                you.props[DMSL_RECHARGE].get_int()) {
                inf.light_text = "-DMsl";
                inf.short_text = "not deflecting";
                inf.light_colour = BLUE;
                string reason = you.cannot_renew_pbs_because();
                if (reason.empty()) {
                    inf.long_text = "Your magic regeneration is restoring your missile deflection.";
                } else {
                    inf.long_text = 
                        "You are not restoring missile deflection because " + 
                        reason + ".";
                }
            } else if (you.permabuff_working(PERMA_DMSL)) {
                inf.light_colour = LIGHTMAGENTA;
                inf.long_text    = "You deflect missiles.";
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "not deflecting";
                inf.long_text = "You are not deflecting missiles because " +
                    you.permabuff_whynot(PERMA_DMSL) + ".";
            }
        }
        break;
    }
    
    case DUR_DEFLECT_MISSILES:
        if (!you.permabuff[PERMA_DMSL]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Deflect Missiles, it will not take effect immediately.";
        }
        break;
        
    case STATUS_DCHAN: {
        if (you.permabuff[PERMA_DCHAN] &&
            !you.duration[DUR_DEATH_CHANNEL]) {
            inf.light_text = "DChan";
            inf.short_text = "death channel";
            if (you.permabuff_working(PERMA_DCHAN)) {
                inf.light_colour = LIGHTMAGENTA;
                inf.long_text = 
                    "You are channeling the dead.";
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "no death channel";
                inf.long_text = "You would be channeling the dead, but " +
                    you.permabuff_whynot(PERMA_DCHAN) + ".";
            }
        }
        break;
    }
    case DUR_DEATH_CHANNEL: {
        if (!you.permabuff[PERMA_DCHAN]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Death Channel, it will not take effect immediately.";
        }
    }
        
    case STATUS_ROF: {
        if (you.permabuff[PERMA_ROF] &&
            !you.duration[DUR_FIRE_SHIELD]) {
            inf.light_text = "RoF";
            inf.short_text = "ring of flames";
            if (you.permabuff_working(PERMA_ROF)) {
                inf.light_colour = LIGHTBLUE;
                inf.long_text = 
                    "You are surrounded by a ring of blazing flame.";
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "no ring of flames";
                inf.long_text = 
                    "You would be surrounded by a ring of flame, but " +
                    you.permabuff_whynot(PERMA_ROF) + ".";
            }
        }
        break;
    }
    case DUR_FIRE_SHIELD: {
        if (!you.permabuff[PERMA_ROF]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Ring of Flames, it will not take effect immediately.";
        }
        break;
    }

    case STATUS_BATTLESPHERE: {
        if (you.permabuff[PERMA_BATTLESPHERE] &&
            !you.duration[DUR_BATTLESPHERE]) {
            monster* battlesphere = find_battlesphere(&you);
            if (battlesphere && battlesphere->battlecharge && 
                you.can_see(*battlesphere)) {
                inf.short_text = "battlesphere";
                inf.long_text = "You have conjured a battlesphere.";
                break;
            }
            inf.light_text = "Sphere";
            if (you.permabuff_working(PERMA_BATTLESPHERE)) {
                inf.light_colour = BLUE;
                if (!battlesphere) {
                    inf.short_text = "conjuring battlesphere";
                    inf.long_text = 
                        "You are going to conjure a new battlesphere.";
                } else if (!battlesphere->battlecharge) {
                    inf.short_text = "battlesphere recharging";
                    inf.long_text = 
                        "Your battlesphere has no charges.";
                } else if (!you.can_see(*battlesphere)) {
                    inf.light_colour = LIGHTBLUE;
                    inf.short_text = "battlesphere out of sight";
                    inf.long_text = 
                        "You will soon recall your battlesphere.";
                }
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "no battlesphere";
                inf.long_text = 
                    "You would conjure a battlesphere, but " +
                    you.permabuff_whynot(PERMA_BATTLESPHERE) + ".";
            }
        }
        break;
    }
    case DUR_BATTLESPHERE: {
        if (!you.permabuff[PERMA_BATTLESPHERE]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Iskenderun's Battlesphere, it will not take effect immediately.";
        }
        break;
    }

    case STATUS_REPEL_MISSILES:
        _describe_repel_missiles(inf);
        break;

    case STATUS_INVISIBLE:
        _describe_invisible(inf);
        break;

    case STATUS_MANUAL:
    {
        string skills = manual_skill_names();
        if (!skills.empty())
        {
            inf.short_text = "studying " + manual_skill_names(true);
            inf.long_text = "You are studying " + skills + ".";
        }
        break;
    }

    case DUR_TRANSFORMATION:
        _describe_transform(inf);
        break;

    case STATUS_STR_ZERO:
        _describe_stat_zero(inf, STAT_STR);
        break;
    case STATUS_INT_ZERO:
        _describe_stat_zero(inf, STAT_INT);
        break;
    case STATUS_DEX_ZERO:
        _describe_stat_zero(inf, STAT_DEX);
        break;

    case STATUS_BONE_ARMOUR:
        if (you.attribute[ATTR_BONE_ARMOUR] > 0)
        {
            inf.short_text = "corpse armour";
            inf.long_text = "You are enveloped in carrion and bones.";
        }
        break;

    case STATUS_CONSTRICTED:
        if (you.is_constricted())
        {
            // Our constrictor isn't, valid so don't report this status.
            if (you.has_invalid_constrictor())
                return false;

            const monster * const cstr = monster_by_mid(you.constricted_by);
            ASSERT(cstr);

            const bool damage =
                cstr->constriction_does_damage(you.is_directly_constricted());

            inf.light_colour = YELLOW;
            inf.light_text   = damage ? "Constr"      : "Held";
            inf.short_text   = damage ? "constricted" : "held";
        }
        break;

    case STATUS_TERRAIN:
        _describe_terrain(inf);
        break;

    // Also handled by DUR_SILENCE, see duration-data.h
    case STATUS_SILENCE:
        if (silenced(you.pos()) && !you.duration[DUR_SILENCE])
        {
            // Only display the status light if not using the noise bar.
            if (Options.equip_bar)
            {
                inf.light_colour = LIGHTRED;
                inf.light_text   = "Sil";
            }
            inf.short_text   = "silenced";
            inf.long_text    = "You are silenced.";
        }
        if (Options.equip_bar && you.duration[DUR_SILENCE])
        {
            inf.light_colour = LIGHTMAGENTA;
            inf.light_text = "Sil";
        }
        break;

    case STATUS_RESIDUAL_HARM:
        if (you.props.exists(RESIDUAL_HARM)) {
            inf.light_colour = YELLOW;
            inf.light_text = "Harm";
            inf.short_text   = "residual harm";
            inf.long_text    = "You are still taking more damage after removing an amulet of harm.";
        }
        break; 

    case STATUS_SERPENTS_LASH:
        if (you.attribute[ATTR_SERPENTS_LASH] > 0)
        {
            inf.light_colour = WHITE;
            inf.light_text
               = make_stringf("Lash (%u)",
                              you.attribute[ATTR_SERPENTS_LASH]);
            inf.short_text = "serpent's lash";
            inf.long_text = "You are moving at supernatural speed.";
        }
        break;

    case STATUS_HEAVENLY_STORM:
        if (you.attribute[ATTR_HEAVENLY_STORM] > 0)
        {
            inf.light_colour = WHITE;
            inf.light_text
               = make_stringf("Storm (%u)",
                              you.attribute[ATTR_HEAVENLY_STORM]);
            inf.short_text = "heavenly storm";
            inf.long_text = "Heavenly clouds are increasing your damage and "
                             "accuracy.";
        }
        break;

    case STATUS_SONG:
    {
        if (you.permabuff[PERMA_SONG] && !you.duration[DUR_SONG_OF_SLAYING]) {
            inf.light_text
                = make_stringf("Slay (%u)",
                               you.props[SONG_OF_SLAYING_KEY].get_int());
            inf.short_text = "singing";
            if (you.permabuff_working(PERMA_SONG)) {
                inf.light_colour = LIGHTBLUE;
                inf.long_text = you.props[SONG_OF_SLAYING_KEY].get_int() ?
                    "Your attacks are strengthened by your song." :
                    "You are singing a song of slaying.";
            } else {            
                inf.light_colour = DARKGREY;
                inf.long_text = "You would sing a song of slaying, but " +
                    you.permabuff_whynot(PERMA_SONG) + ".";
            }
        }
        break;
    }
    case DUR_SONG_OF_SLAYING:
        if (!you.permabuff[PERMA_SONG]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Song of Slaying, it will not take effect immediately.";
        }
        break;

    case DUR_REGENERATION:
        if (!you.permabuff[PERMA_REGEN]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Regeneration, it will not take effect immediately.";
        }
        break;

    case STATUS_BEOGH:
        if (env.level_state & LSTATE_BEOGH && can_convert_to_beogh())
        {
            inf.light_colour = WHITE;
            inf.light_text = "Beogh";
        }
        break;

    case STATUS_RECALL:
        if (you.attribute[ATTR_NEXT_RECALL_INDEX] > 0)
        {
            inf.light_colour = WHITE;
            inf.light_text   = "Recall";
            inf.short_text   = "recalling";
            inf.long_text    = "You are recalling your allies.";
        }
        break;

    case DUR_WATER_HOLD:
        inf.light_text   = "Engulf";
        if (you.res_water_drowning())
        {
            inf.short_text   = "engulfed";
            inf.long_text    = "You are engulfed in water.";
            if (you.can_swim())
                inf.light_colour = DARKGREY;
            else
                inf.light_colour = YELLOW;
        }
        else
        {
            inf.short_text   = "engulfed (cannot breathe)";
            inf.long_text    = "You are engulfed in water and unable to breathe.";
            inf.light_colour = RED;
        }
        break;

    case STATUS_DRAINED:
        if (you.attribute[ATTR_XP_DRAIN] > 450)
        {
            inf.light_colour = MAGENTA;
            inf.light_text   = "Drain";
            inf.short_text   = "extremely drained";
            inf.long_text    = "Your life force is extremely drained.";
        }
        else if (you.attribute[ATTR_XP_DRAIN] > 250)
        {
            inf.light_colour = RED;
            inf.light_text   = "Drain";
            inf.short_text   = "very heavily drained";
            inf.long_text    = "Your life force is very heavily drained.";
        }
        else if (you.attribute[ATTR_XP_DRAIN] > 100)
        {
            inf.light_colour = LIGHTRED;
            inf.light_text   = "Drain";
            inf.short_text   = "heavily drained";
            inf.long_text    = "Your life force is heavily drained.";
        }
        else if (you.attribute[ATTR_XP_DRAIN])
        {
            inf.light_colour = YELLOW;
            inf.light_text   = "Drain";
            inf.short_text   = "drained";
            inf.long_text    = "Your life force is drained.";
        }
        break;

    case STATUS_RAY:
        if (you.attribute[ATTR_SEARING_RAY])
        {
            inf.light_colour = WHITE;
            inf.light_text   = _ray_text().c_str();
        }
        break;

    case STATUS_DIG:
        if (you.digging)
        {
            inf.light_colour = WHITE;
            inf.light_text   = "Dig";
        }
        break;

    case STATUS_ELIXIR:
        if (you.duration[DUR_ELIXIR_HEALTH] || you.duration[DUR_ELIXIR_MAGIC])
        {
            if (you.duration[DUR_ELIXIR_HEALTH] && you.duration[DUR_ELIXIR_MAGIC])
                inf.light_colour = WHITE;
            else if (you.duration[DUR_ELIXIR_HEALTH])
                inf.light_colour = LIGHTGREEN;
            else
                inf.light_colour = LIGHTBLUE;
            inf.light_text   = "Elixir";
        }
        break;

    case STATUS_MAGIC_SAPPED:
        if (you.props[SAP_MAGIC_KEY].get_int() >= 3)
        {
            inf.light_colour = RED;
            inf.light_text   = "-Wiz";
            inf.short_text   = "extremely magic sapped";
            inf.long_text    = "Your control over your magic has "
                                "been greatly sapped.";
        }
        else if (you.props[SAP_MAGIC_KEY].get_int() == 2)
        {
            inf.light_colour = LIGHTRED;
            inf.light_text   = "-Wiz";
            inf.short_text   = "very magic sapped";
            inf.long_text    = "Your control over your magic has "
                                "been significantly sapped.";
        }
        else if (you.props[SAP_MAGIC_KEY].get_int() == 1)
        {
            inf.light_colour = YELLOW;
            inf.light_text   = "-Wiz";
            inf.short_text   = "magic sapped";
            inf.long_text    = "Your control over your magic has "
                                "been sapped.";
        }
        break;

    case STATUS_BRIBE:
    {
        int bribe = 0;
        vector<const char *> places;
        for (int i = 0; i < NUM_BRANCHES; i++)
        {
            branch_type br = gozag_fixup_branch(static_cast<branch_type>(i));

            if (branch_bribe[br] > 0)
            {
                if (player_in_branch(static_cast<branch_type>(i)))
                    bribe = branch_bribe[br];

                places.push_back(branches[static_cast<branch_type>(i)]
                                 .longname);
            }
        }

        if (bribe > 0)
        {
            inf.light_colour = (bribe >= 2000) ? WHITE :
                                (bribe >= 1000) ? LIGHTBLUE
                                                : BLUE;

            inf.light_text = "Bribe";
            inf.short_text = make_stringf("bribing [%s]",
                                           comma_separated_line(places.begin(),
                                                                places.end(),
                                                                ", ", ", ")
                                                                .c_str());
            inf.long_text = "You are bribing "
                             + comma_separated_line(places.begin(),
                                                    places.end())
                             + ".";
        }
        break;
    }

    case DUR_HORROR:
    {
        const int horror = you.props[HORROR_PENALTY_KEY].get_int();
        inf.light_text = make_stringf("Horr(%d)", -1 * horror);
        if (horror >= HORROR_LVL_OVERWHELMING)
        {
            inf.light_colour = RED;
            inf.short_text   = "overwhelmed with horror";
            inf.long_text    = "Horror overwhelms you!";
        }
        else if (horror >= HORROR_LVL_EXTREME)
        {
            inf.light_colour = LIGHTRED;
            inf.short_text   = "extremely horrified";
            inf.long_text    = "You are extremely horrified!";
        }
        else if (horror)
        {
            inf.light_colour = YELLOW;
            inf.short_text   = "horrified";
            inf.long_text    = "You are horrified!";
        }
        break;
    }

    case STATUS_CLOUD:
    {
        cloud_type cloud = cloud_type_at(you.pos());
        if (Options.cloud_status && cloud != CLOUD_NONE)
        {
            inf.light_text = "Cloud";
            // TODO: make the colour based on the cloud's color; requires elemental
            // status lights, though.
            inf.light_colour =
                is_damaging_cloud(cloud, true, cloud_is_yours_at(you.pos())) ? LIGHTRED : DARKGREY;
        }
        break;
    }

    case DUR_CLEAVE:
    {
        const item_def* weapon = you.weapon();

        if (weapon && item_attack_skill(*weapon) == SK_AXES)
            inf.light_colour = DARKGREY;

        break;
    }

    case STATUS_PPROJ:
    {
        if (you.permabuff[PERMA_PPROJ] && 
            !you.duration[DUR_PORTAL_PROJECTILE]) {
            inf.light_text = "PProj";
            inf.short_text = "portal projectile";
            if (you.permabuff_working(PERMA_PPROJ)) {
                if (enough_mp((you.props[CHARMS_DEBT].get_int() > 0) ?
                              2 : 1, true, false)) {
                    inf.light_colour = LIGHTBLUE;
                    inf.long_text = 
                        "You are teleporting projectiles to their destination.";
                } else {
                    inf.light_colour = BLUE;
                    inf.long_text = 
                    "You would be teleporting projectiles to their destination, but you don't have enough MP.";
                }
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "not teleporting projectiles";
                inf.long_text = "You would be teleporting projectiles, but " + 
                    you.permabuff_whynot(PERMA_PPROJ) + ".";
            }
        }
        break;
    }

    case DUR_PORTAL_PROJECTILE:
        if (!you.permabuff[PERMA_PPROJ]) inf.light_colour = DARKGREY;
        inf.long_text = "If you recast Portal Projectile, it will not take effect immediately.";
        break;

    case STATUS_EXCRU:
        if (you.permabuff[PERMA_EXCRU] &&
            !you.duration[DUR_EXCRUCIATING_WOUNDS]) {
            if (you.permabuff_working(PERMA_EXCRU)) {
                inf.light_text = "Excru";
                inf.short_text = "excruciating";
                inf.light_colour = LIGHTBLUE;
                inf.long_text = "You are infusing your attacks with the essence of pain.";
            } else {
                inf.light_text = "-Excru";
                inf.short_text = "cannot excruciate";
                inf.light_colour = BLUE;
                inf.long_text = "You would inflict excruciating wounds, but "
                    + you.permabuff_whynot(PERMA_EXCRU) + ".";
            }
        }
        break;

    case DUR_EXCRUCIATING_WOUNDS:
        if (!you.permabuff[PERMA_EXCRU]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Excruciating Wounds, it will not take effect immediately.";
        }
        break;

    case STATUS_INFUSION:
    {
        if (you.permabuff[PERMA_INFUSION] && !you.duration[DUR_INFUSION]) {
            inf.light_text = "Infus";
            inf.short_text = "infusing";
            if (you.permabuff_working(PERMA_INFUSION)) {
                if (enough_mp(1, true, false)) {
                    inf.light_colour = LIGHTBLUE;
                    inf.long_text = 
                        "You are infusing your attacks with magical energy.";
                } else {
                    inf.light_colour = BLUE;
                    inf.long_text = 
                        "You would infuse your attacks, but you don't have any MP.";
                }
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "not infusing";
                inf.long_text = "You would be infusing your attacks, but " + 
                    you.permabuff_whynot(PERMA_INFUSION) + ".";
            }
        }
        break;
    }

    case DUR_INFUSION:
        if (!you.permabuff[PERMA_INFUSION]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Infusion, it will not take effect immediately.";
        }
        break;
        
    case STATUS_SHROUD:
    {
        if (you.permabuff[PERMA_SHROUD] && 
            !you.duration[DUR_SHROUD_OF_GOLUBRIA]) {
            inf.light_text = "Shroud";
            inf.short_text = "shrouded";
            if (you.props.exists(SHROUD_RECHARGE) &&
                you.props[SHROUD_RECHARGE].get_int()){
                inf.light_text = "-Shroud";
                inf.short_text = "unshrouded";
                inf.light_colour = BLUE;
                string reason = you.cannot_renew_pbs_because();
                if (reason.empty()) {
                    inf.long_text = 
                        "Your magic regeneration is reconstructing your distorting shroud.";
                } else {
                    inf.long_text = 
                        "You are not reconstructing your shroud because " + 
                        reason + ".";
                }
            } else if (you.permabuff_working(PERMA_SHROUD)) {
                inf.light_colour = LIGHTBLUE;
                inf.long_text = 
                    "You are protected by a distorting shroud.";
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "not shrouded";
                inf.long_text = "Your shroud is not working because " +
                    you.permabuff_whynot(PERMA_SHROUD) + ".";
            }
        }
        break;
    }
    case DUR_SHROUD_OF_GOLUBRIA:
        if (!you.permabuff[PERMA_SHROUD]) {
            inf.light_colour = DARKGREY;
            inf.long_text = "If you recast Shroud of Golubria, it will not take effect immediately.";
        }
        break;

    case STATUS_APPENDAGE:
    {
        if (you.permabuff[PERMA_APPENDAGE] && 
            !you.duration[DUR_APPENDAGE]) {
            inf.light_text = "App";
            inf.short_text = "beastly appendage";
            if (you.permabuff_working(PERMA_APPENDAGE)) {
                if (you.form == transformation::appendage) {
                    if (you.attribute[ATTR_APPENDAGE] == 
                        MUT_RESIDUAL_APPENDAGE) {
                        inf.light_colour = GREEN;
                        inf.long_text = "The magic of Beastly Appendage has become too weak.";
                    } else {
                        inf.light_colour = LIGHTGREEN;
                        const Form * const form = get_form();
                        inf.long_text = form->get_description();
                    }
                } else {
                    inf.light_colour = LIGHTBLUE;
                    inf.long_text = "You will grow a monstrous appendage in melee.";                   
                }
            } else {
                inf.light_colour = DARKGREY;
                inf.short_text = "no beastly appendage";
                inf.long_text = "You would grow a beastly appendage, but " + 
                    you.permabuff_whynot(PERMA_APPENDAGE) + ".";
            }
        }
        break;
    }

    case DUR_APPENDAGE:
        if (!you.permabuff[PERMA_APPENDAGE]) inf.light_colour = DARKGREY;
        inf.long_text = "If you recast Beastly Appendage, it will not take effect immediately.";
        break;

    case STATUS_ORB:
    {
        if (player_has_orb())
        {
            inf.light_colour = LIGHTMAGENTA;
            inf.light_text = "Orb";
        }
        else if (orb_limits_translocation())
        {
            inf.light_colour = MAGENTA;
            inf.light_text = "Orb";
        }

        break;
    }

    case STATUS_STILL_WINDS:
        if (env.level_state & LSTATE_STILL_WINDS)
        {
            inf.light_colour = BROWN;
            inf.light_text = "-Clouds";
        }
        break;

    case STATUS_CHARMS_MP:
        if (you.props[CHARMS_ALL_MPREGEN].get_bool()) {
            inf.light_colour = BROWN;
            inf.light_text = "Charms";
            inf.short_text = "charms draining MP";
            inf.long_text = "The benefits from your permanent enchantments are presently consuming all your magic regeneration.";
        }
        break;
        
    case STATUS_MAX_PIETY:
        if (at_almost_max_piety()) {
            inf.short_text = "max piety";
            inf.long_text = "You have reached maximum piety.";
        }
        break;

    default:
        if (!found)
        {
            inf.light_colour = RED;
            inf.light_text   = "Missing";
            inf.short_text   = "missing status";
            inf.long_text    = "Missing status description.";
            return false;
        }
        else
            break;
    }
    return true;
}

static void _describe_hunger(status_info& inf)
{
    const bool vamp = (you.species == SP_VAMPIRE);

    switch (you.hunger_state)
    {
    case HS_ENGORGED:
        inf.light_colour = (vamp ? GREEN : LIGHTGREEN);
        inf.light_text   = (vamp ? "Alive" : "Engorged");
        break;
    case HS_VERY_FULL:
        inf.light_colour = GREEN;
        inf.light_text   = "Very Full";
        break;
    case HS_FULL:
        inf.light_colour = GREEN;
        inf.light_text   = "Full";
        break;
    case HS_HUNGRY:
        inf.light_colour = YELLOW;
        inf.light_text   = (vamp ? "Thirsty" : "Hungry");
        break;
    case HS_VERY_HUNGRY:
        inf.light_colour = YELLOW;
        inf.light_text   = (vamp ? "Very Thirsty" : "Very Hungry");
        break;
    case HS_NEAR_STARVING:
        inf.light_colour = YELLOW;
        inf.light_text   = (vamp ? "Near Bloodless" : "Near Starving");
        break;
    case HS_STARVING:
        inf.light_colour = LIGHTRED;
        inf.light_text   = (vamp ? "Bloodless" : "Starving");
        inf.short_text   = (vamp ? "bloodless" : "starving");
        break;
    case HS_FAINTING:
        inf.light_colour = RED;
        inf.light_text   = (vamp ? "Bloodless" : "Fainting");
        inf.short_text   = (vamp ? "bloodless" : "fainting");
        break;
    case HS_SATIATED: // no status light
    default:
        break;
    }
}

static void _describe_glow(status_info& inf)
{
    const int signed_cont = get_contamination_level();
    if (signed_cont <= 0)
        return;

    const unsigned int cont = signed_cont; // so we don't get compiler warnings
    if (player_severe_contamination())
    {
        inf.light_colour = _bad_ench_colour(cont, SEVERE_CONTAM_LEVEL + 1,
                                                  SEVERE_CONTAM_LEVEL + 2);
    }
    else if (cont > 1)
        inf.light_colour = LIGHTGREY;
    else
        inf.light_colour = DARKGREY;
    inf.light_text = "Contam";

    /// Mappings from contamination levels to descriptions.
    static const string contam_adjectives[] =
    {
        "",
        "very slightly ",
        "slightly ",
        "",
        "heavily ",
        "very heavily ",
        "very very heavily ", // this is silly but no one will ever see it
        "impossibly ",        // (likewise)
    };
    ASSERT(signed_cont >= 0);

    const int adj_i = min((size_t) cont, ARRAYSZ(contam_adjectives) - 1);
    inf.short_text = contam_adjectives[adj_i] + "contaminated";
    inf.long_text = describe_contamination(cont);
}

static void _describe_regen(status_info& inf)
{
    const bool regen = (you.permabuff_working(PERMA_REGEN)
                        || you.duration[DUR_TROGS_HAND] > 0);
    const bool no_heal = !player_regenerates_hp();
    // Does vampire hunger level affect regeneration rate significantly?
    const bool vampmod = !no_heal && !regen && you.species == SP_VAMPIRE
                         && you.hunger_state != HS_SATIATED;

    if (regen)
    {
        inf.light_colour = BLUE;
        if (you.duration[DUR_TROGS_HAND]) {
            inf.light_colour = _dur_colour(BLUE, dur_expiring(DUR_TROGS_HAND));
        }
        if (you.permabuff_working(PERMA_REGEN) && 
            ((you.props[REGEN_RESERVE].get_int() > 0) ||
             (!you.props[CHARMS_ALL_MPREGEN].get_bool() &&
              you.can_renew_pbs()))) {
            inf.light_colour = LIGHTBLUE;
        }

        inf.light_text   = "Regen";
        if (you.duration[DUR_TROGS_HAND])
            inf.light_text += " MR++";
        else if (no_heal)
            inf.light_colour = DARKGREY;
    } 

    if ((you.disease && !regen) || no_heal)
       inf.short_text = "non-regenerating";
    else if (regen)
    {
        if (you.disease)
        {
            inf.short_text = "recuperating";
            inf.long_text  = "You are recuperating from your illness.";
        }
        else
        {
            if (you.permabuff_working(PERMA_REGEN) && 
                !(you.props[REGEN_RESERVE].get_int() > 0)) {
                if (you.props[CHARMS_ALL_MPREGEN].get_bool()) {
                    inf.short_text = "not regenerating (no MP regeneration)";
                    inf.long_text = "You are not regenerating because your other permanent charms are consuming all your MP regeneration.";
                } else {
                    string reason = you.cannot_renew_pbs_because();
                    if (!reason.empty()) {
                        inf.short_text = 
                            "not regenerating (cannot renew charms)";
                        inf.long_text = 
                            "You are not regenerating; you cannot renew your charms because " + reason + ".";
                    }
                } 
            } else {
                inf.short_text = "regenerating";
                inf.long_text  = "You are regenerating.";
            }
        }
    }
    else if (vampmod)
    {
        if (you.disease)
            inf.short_text = "recuperating";
        else
            inf.short_text = "regenerating";

        if (you.hunger_state < HS_SATIATED)
            inf.short_text += " slowly";
        else
            inf.short_text += " quickly";
    } 
    if (you.permabuff[PERMA_REGEN] && !regen && 
        !you.duration[DUR_REGENERATION]) {
        inf.light_text   = "Regen"; inf.light_colour = DARKGREY;
        inf.short_text = "not magically regenerating";
        inf.long_text = "You would be magically regenerating, but " +
            you.permabuff_whynot(PERMA_REGEN) + ".";
    }
}

static void _describe_poison(status_info& inf)
{
    int pois_perc = (you.hp <= 0) ? 100
                                  : ((you.hp - max(0, poison_survival())) * 100 / you.hp);
    inf.light_colour = (player_res_poison(false) >= 3
                         ? DARKGREY : _bad_ench_colour(pois_perc, 35, 100));
    inf.light_text   = "Pois";
    const string adj =
         (pois_perc >= 100) ? "lethally" :
         (pois_perc > 65)   ? "seriously" :
         (pois_perc > 35)   ? "quite"
                            : "mildly";
    inf.short_text   = adj + " poisoned"
        + make_stringf(" (%d -> %d)", you.hp, poison_survival());
    inf.long_text    = "You are " + inf.short_text + ".";
}

static void _describe_speed(status_info& inf)
{
    bool slow = you.duration[DUR_SLOW] || have_stat_zero();
    bool fast = you.duration[DUR_HASTE];

    if (slow && fast)
    {
        inf.light_colour = MAGENTA;
        inf.light_text   = "Fast+Slow";
        inf.short_text   = "hasted and slowed";
        inf.long_text = "You are under both slowing and hasting effects.";
    }
    else if (slow)
    {
        inf.light_colour = RED;
        inf.light_text   = "Slow";
        inf.short_text   = "slowed";
        inf.long_text    = "You are slowed.";
    }
    else if (fast)
    {
        inf.light_colour = _dur_colour(BLUE, dur_expiring(DUR_HASTE));
        inf.light_text   = "Fast";
        inf.short_text = "hasted";
        inf.long_text = "Your actions are hasted.";
        _mark_expiring(inf, dur_expiring(DUR_HASTE));
    }
}

static void _describe_airborne(status_info& inf)
{
    if (!you.airborne())
        return;

    const bool perm      = you.permanent_flight();
    const bool expiring  = (!perm && dur_expiring(DUR_FLIGHT));
    const bool emergency = you.props[EMERGENCY_FLIGHT_KEY].get_bool();
    const string desc    = you.tengu_flight() ? " quickly and evasively" : "";

    inf.light_colour = perm ? WHITE : emergency ? LIGHTRED : BLUE;
    inf.light_text   = "Fly";
    inf.short_text   = "flying" + desc;
    inf.long_text    = "You are flying" + desc + ".";
    inf.light_colour = _dur_colour(inf.light_colour, expiring);
    _mark_expiring(inf, expiring);
}

static void _describe_rotting(status_info& inf)
{
    if (you.species == SP_GHOUL)
    {
        inf.short_text = "rotting";
        inf.long_text = "Your flesh is rotting";
        int rot = 1 + (1 << max(0, HS_SATIATED - you.hunger_state));
        if (rot > 15)
            inf.long_text += " before your eyes";
        else if (rot > 8)
            inf.long_text += " away quickly";
        else if (rot > 4)
            inf.long_text += " badly";
        else if (rot > 2)
            inf.long_text += " faster than usual";
        else
            inf.long_text += " at the usual pace";
        inf.long_text += ".";
    }
}

static void _describe_sickness(status_info& inf)
{
    if (you.disease)
    {
        const int high = 120 * BASELINE_DELAY;
        const int low  =  40 * BASELINE_DELAY;

        inf.light_colour   = _bad_ench_colour(you.disease, low, high);
        inf.light_text     = "Sick";

        string mod = (you.disease > high) ? "badly "  :
                     (you.disease >  low) ? ""
                                          : "mildly ";

        inf.short_text = mod + "diseased";
        inf.long_text  = "You are " + mod + "diseased.";
    }
}

/**
 * Populate a status info struct with a description of the player's current
 * form.
 *
 * @param[out] inf  The status info struct to be populated.
 */
static void _describe_transform(status_info& inf)
{
    // appendage handled by STATUS_APPENDAGE now
    if ((you.form == transformation::none) or 
        (you.form == transformation::appendage)) {
        return;
    }

    const Form * const form = get_form();
    inf.light_text = form->short_name;
    inf.short_text = form->get_long_name();
    inf.long_text = form->get_description();

    const bool vampbat = (you.species == SP_VAMPIRE
                          && you.form == transformation::bat);
    const bool expire  = dur_expiring(DUR_TRANSFORMATION) && !vampbat;

    inf.light_colour = _dur_colour(GREEN, expire);
    _mark_expiring(inf, expire);
}

static const char* s0_names[NUM_STATS] = { "Collapse", "Brainless", "Clumsy", };

static void _describe_stat_zero(status_info& inf, stat_type st)
{
    if (you.duration[stat_zero_duration(st)])
    {
        inf.light_colour = you.stat(st) ? LIGHTRED : RED;
        inf.light_text   = s0_names[st];
        inf.short_text   = make_stringf("lost %s", stat_desc(st, SD_NAME));
        inf.long_text    = make_stringf(you.stat(st) ?
                "You are recovering from loss of %s." : "You have no %s!",
                stat_desc(st, SD_NAME));
    }
}

static void _describe_terrain(status_info& inf)
{
    switch (grd(you.pos()))
    {
    case DNGN_SHALLOW_WATER:
        inf.light_colour = LIGHTBLUE;
        inf.light_text = "Water";
        break;
    case DNGN_DEEP_WATER:
        inf.light_colour = BLUE;
        inf.light_text = "Water";
        break;
    case DNGN_LAVA:
        inf.light_colour = RED;
        inf.light_text = "Lava";
        break;
    default:
        ;
    }
}

static void _describe_repel_missiles(status_info& inf)
{
    const int level = you.missile_deflection();

    if (level == 1) {
        bool perm = you.get_mutation_level(MUT_DISTORTION_FIELD) == 3
            || you.wearing_ego(EQ_ALL_ARMOUR, SPARM_REPULSION)
            || you.scan_artefacts(ARTP_RMSL)
                    || have_passive(passive_t::upgraded_storm_shield);
        inf.light_colour = perm ? WHITE : LIGHTBLUE;
        inf.light_text   = "RMsl";
        inf.short_text   = "repel missiles";
        inf.long_text    = "You repel missiles.";
    }         
}

static void _describe_invisible(status_info& inf)
{
    if (!you.duration[DUR_INVIS] && you.form != transformation::shadow)
        return;

    if (you.form == transformation::shadow)
    {
        inf.light_colour = _dur_colour(WHITE,
                                        dur_expiring(DUR_TRANSFORMATION));
    }
    else if (you.attribute[ATTR_INVIS_UNCANCELLABLE])
        inf.light_colour = _dur_colour(BLUE, dur_expiring(DUR_INVIS));
    else
        inf.light_colour = _dur_colour(MAGENTA, dur_expiring(DUR_INVIS));
    inf.light_text   = "Invis";
    inf.short_text   = "invisible";
    if (you.backlit())
    {
        inf.light_colour = DARKGREY;
        inf.short_text += " (but backlit and visible)";
    }
    inf.long_text = "You are " + inf.short_text + ".";
    _mark_expiring(inf, dur_expiring(you.form == transformation::shadow
                                     ? DUR_TRANSFORMATION
                                     : DUR_INVIS));
}

/**
 * Does a given duration tick down simply over time?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      Whether the duration's end_msg is non-null.
 */
bool duration_decrements_normally(duration_type dur)
{
    return _lookup_duration(dur)->decr.end.msg != nullptr;
}

/**
 * What message should a given duration print when it expires, if any?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      A message to print for the duration when it ends.
 */
const char *duration_end_message(duration_type dur)
{
    return _lookup_duration(dur)->decr.end.msg;
}

/**
 * What message should a given duration print when it reaches 50%, if any?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      A message to print for the duration when it hits 50%.
 */
const char *duration_mid_message(duration_type dur)
{
    return _lookup_duration(dur)->decr.mid_msg.msg;
}

/**
 * How much should the duration be decreased by when it hits the midpoint (to
 * fuzz the remaining time), if at all?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      A random value to reduce the remaining duration by; may be 0.
 */
int duration_mid_offset(duration_type dur)
{
    return _lookup_duration(dur)->decr.mid_msg.offset();
}

/**
 * At what number of turns remaining is the given duration considered to be
 * 'expiring', for purposes of messaging & status light colouring?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      The maximum number of remaining turns at which the duration
 *              is considered 'expiring'; may be 0.
 */
int duration_expire_point(duration_type dur)
{
    return _lookup_duration(dur)->expire_threshold * BASELINE_DELAY;
}

/**
 * What channel should the duration messages be printed in?
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 * @return      The appropriate message channel, e.g. MSGCH_RECOVERY.
 */
msg_channel_type duration_mid_chan(duration_type dur)
{
    return _lookup_duration(dur)->decr.recovery ? MSGCH_RECOVERY
                                                : MSGCH_DURATION;
}

/**
 * If a duration has some special effect when ending, trigger it.
 *
 * @param dur   The duration in question (e.g. DUR_PETRIFICATION).
 */
void duration_end_effect(duration_type dur)
{
    if (_lookup_duration(dur)->decr.end.on_end)
        _lookup_duration(dur)->decr.end.on_end();
}
