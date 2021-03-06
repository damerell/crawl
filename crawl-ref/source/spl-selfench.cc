/**
 * @file
 * @brief Self-enchantment spells.
**/

#include "AppHdr.h"

#include "spl-selfench.h"

#include <cmath>

#include "areas.h"
#include "art-enum.h"
#include "butcher.h" // butcher_corpse
#include "coordit.h" // radius_iterator
#include "delay.h"
#include "god-conduct.h"
#include "god-passive.h"
#include "hints.h"
#include "items.h" // stack_iterator
#include "libutil.h"
#include "macro.h"
#include "message.h"
#include "output.h"
#include "permabuff.h"
#include "prompt.h"
#include "religion.h"
#include "showsymb.h"
#include "spl-transloc.h"
#include "spl-util.h"
#include "spl-summoning.h"
#include "spl-wpnench.h"
#include "stringutil.h"
#include "transform.h"
#include "tilepick.h"
#include "view.h"
#include "viewchar.h"

int allowed_deaths_door_hp()
{
    int hp = calc_spell_power(SPELL_DEATHS_DOOR, true) / 10;

    return max(hp, 1);
}

spret_type cast_deaths_door(int pow, bool fail)
{
    fail_check();
    mpr("You stand defiantly in death's doorway!");
    mprf(MSGCH_SOUND, "You seem to hear sand running through an hourglass...");

    set_hp(allowed_deaths_door_hp());
    deflate_hp(you.hp_max, false);

    you.set_duration(DUR_DEATHS_DOOR, 10 + random2avg(13, 3)
                                       + (random2(pow) / 10));

    if (you.duration[DUR_DEATHS_DOOR] > 25 * BASELINE_DELAY)
        you.duration[DUR_DEATHS_DOOR] = (23 + random2(5)) * BASELINE_DELAY;
    return SPRET_SUCCESS;
}

void remove_ice_armour()
{
    mprf(MSGCH_DURATION, "Your icy armour melts away.");
    you.redraw_armour_class = true;
    you.duration[DUR_ICY_ARMOUR] = 0;
}

spret_type ice_armour(int pow, bool fail)
{
    fail_check();

    if (you.duration[DUR_ICY_ARMOUR])
        mpr("Your icy armour thickens.");
    else if (you.form == transformation::ice_beast)
        mpr("Your icy body feels more resilient.");
    else
        mpr("A film of ice covers your body!");

    if (you.attribute[ATTR_BONE_ARMOUR] > 0)
    {
        you.attribute[ATTR_BONE_ARMOUR] = 0;
        mpr("Your corpse armour falls away.");
    }

    you.increase_duration(DUR_ICY_ARMOUR, random_range(80, 100), 100);
    you.props[ICY_ARMOUR_KEY] = pow;
    you.redraw_armour_class = true;

    return SPRET_SUCCESS;
}

spret_type deflection(int pow, bool fail)
{
    if (you.permabuff[PERMA_DMSL]) {
        bool wasworking = you.permabuff_working(PERMA_DMSL);
        const int orig_defl = you.missile_deflection();
        if (you.props.exists(DMSL_RECHARGE) &&
            you.props[DMSL_RECHARGE].get_int()) {
            you.props.erase(DMSL_RECHARGE); 
// Why 25 for SoG and DMsl but 10 for Regeneration? I dunno but I guess it
// is because Regen will recharge faster if you have lots of MP anyway, so
// preemptively cancelling it is less attractive?
            you.increase_duration(DUR_DEFLECT_MISSILES, 25, 50);
        }
        you.pb_off(PERMA_DMSL); 
        mprf("You are %s.",
             !wasworking ?
             "no longer attempting to deflect missiles" : 
             (you.missile_deflection() < orig_defl ? 
              "less protected from missiles" :
              // I don't think you can currently get this message since the
              // spell is the only way to get DMsl
              "no longer protected from missiles by your spell"));
        return SPRET_PERMACANCEL;
    } else {
        fail_check();
        mpr (you.duration[DUR_DEFLECT_MISSILES] ? 
             "You will soon be deflecting missiles." :
             "You feel very safe from missiles.");
        you.pb_on(PERMA_DMSL); return SPRET_SUCCESS;
    }    
}

spret_type cast_regen(int pow, bool fail)
{
    if (you.permabuff[PERMA_REGEN]) {
        mpr("Your skin stops crawling.");
                // This stops HOM dropping and recasting to refill the reserve
        you.increase_duration(DUR_REGENERATION, 10, 50);
        you.pb_off(PERMA_REGEN); return SPRET_PERMACANCEL;
    } else {
        fail_check();
        mpr("Your skin crawls.");
        // Made this large again, with the spell reserving a like number of MP
        you.props[REGEN_RESERVE] = spell_mana(SPELL_REGENERATION) * 100;
        you.props[REGEN_REPORTING_PERCENT] = 100;
        you.pb_on(PERMA_REGEN); return SPRET_SUCCESS;
    }
}

spret_type cast_revivification(int pow, bool fail)
{
    fail_check();
    mpr("Your body is healed in an amazingly painful way.");

    const int loss = 6 + binomial(9, 8, pow);
    dec_max_hp(loss * you.hp_max / 100);
    set_hp(you.hp_max);

    if (you.duration[DUR_DEATHS_DOOR])
    {
        mprf(MSGCH_DURATION, "Your life is in your own hands once again.");
        // XXX: better cause name?
        paralyse_player("Death's Door abortion");
        you.duration[DUR_DEATHS_DOOR] = 0;
    }
    return SPRET_SUCCESS;
}

spret_type cast_swiftness(int power, bool fail)
{
    fail_check();

    if (you.in_liquid())
    {
        // Hint that the player won't be faster until they leave the liquid.
        mprf("The %s foams!", you.in_water() ? "water"
                                             : "liquid ground");
    }

    you.set_duration(DUR_SWIFTNESS, 12 + random2(power)/2, 30,
                     "You feel quick.");
    you.attribute[ATTR_SWIFTNESS] = you.duration[DUR_SWIFTNESS];

    return SPRET_SUCCESS;
}

int cast_selective_amnesia(const string &pre_msg)
{
    if (you.spell_no == 0)
    {
        canned_msg(MSG_NO_SPELLS);
        return 0;
    }

    int keyin = 0;
    spell_type spell;
    int slot;

    // Pick a spell to forget.
    keyin = list_spells(false, false, false, false, "Forget which spell?");
    redraw_screen();

    if (isaalpha(keyin))
    {
        spell = get_spell_by_letter(keyin);
        slot = get_spell_slot_by_letter(keyin);

        if (spell != SPELL_NO_SPELL)
        {
            const string prompt = make_stringf(
                    "Forget %s, freeing %d spell level%s for a total of %d?%s",
                    spell_title(spell), spell_levels_required(spell),
                    spell_levels_required(spell) != 1 ? "s" : "",
                    player_spell_levels() + spell_levels_required(spell),
                    you.spell_library[spell] ? "" :
                    " This spell is not in your library!");

            if (yesno(prompt.c_str(), true, 'n', false))
            {
                if (!pre_msg.empty())
                    mpr(pre_msg);
                del_spell_from_memory_by_slot(slot);
                return 1;
            }
        }
    }

    canned_msg(MSG_OK);
    return -1;
}

spret_type cast_infusion(int pow, bool fail)
{
    if (you.permabuff[PERMA_INFUSION]) {
        mpr(you.permabuff_working(PERMA_INFUSION) ? 
            "You stop infusing your attacks with magical energy." :
            "You stop attempting to infuse your attacks with magical energy.");
        you.pb_off(PERMA_INFUSION); return SPRET_PERMACANCEL;
    } else {
        fail_check();
        mpr(you.duration[DUR_INFUSION] ? 
            "You will soon be infusing your attacks with magical energy." :
            "You begin infusing your attacks with magical energy.");
        // Power is calculated every time we attack
        you.pb_on(PERMA_INFUSION); return SPRET_SUCCESS;
    }
}

spret_type cast_song_of_slaying(int pow, bool fail)
{
    if (you.permabuff[PERMA_SONG]) {
        mpr(you.permabuff_working(PERMA_SONG) ? 
            "You stop singing a song of slaying." :
            "You stop trying to sing a song of slaying.");
        you.props[SONG_OF_SLAYING_KEY] = 0;
        you.pb_off(PERMA_SONG); return SPRET_PERMACANCEL;
    } else {
        fail_check();
        mpr(you.duration[DUR_SONG_OF_SLAYING] ? 
            "You will soon be singing a song of slaying." :
            "You start singing a song of slaying.");
        you.props[SONG_OF_SLAYING_KEY] = 0;
        you.pb_on(PERMA_SONG); return SPRET_SUCCESS;
    }
}

void check_sos_miscast() {
    if (you.permabuff_working(PERMA_SONG) &&
        you.props[SONG_OF_SLAYING_KEY].get_int()) {
        permabuff_fail_check(PERMA_SONG,
                             "You stumble over the syllables of your song.");
    }
}

spret_type cast_silence(int pow, bool fail)
{
    fail_check();
    mpr("A profound silence engulfs you.");

    you.increase_duration(DUR_SILENCE, 10 + pow/4 + random2avg(pow/2, 2), 100);
    invalidate_agrid(true);

    if (you.beheld())
        you.update_beholders();

    learned_something_new(HINT_YOU_SILENCE);
    return SPRET_SUCCESS;
}

spret_type cast_liquefaction(int pow, bool fail)
{
    fail_check();
    flash_view_delay(UA_PLAYER, BROWN, 80);
    flash_view_delay(UA_PLAYER, YELLOW, 80);
    flash_view_delay(UA_PLAYER, BROWN, 140);

    mpr("The ground around you becomes liquefied!");

    you.increase_duration(DUR_LIQUEFYING, 10 + random2avg(pow, 2), 100);
    invalidate_agrid(true);
    return SPRET_SUCCESS;
}

spret_type cast_shroud_of_golubria(int pow, bool fail)
{
    if (you.permabuff[PERMA_SHROUD]) {
        mpr("You dispel your protective shroud.");
        if (you.props.exists(SHROUD_RECHARGE) &&
            you.props[SHROUD_RECHARGE].get_int()) {
            you.props.erase(SHROUD_RECHARGE); 
            you.increase_duration(DUR_SHROUD_OF_GOLUBRIA, 25, 50);
        }
        you.pb_off(PERMA_SHROUD); return SPRET_PERMACANCEL;
    } else {
        fail_check();
        mpr (you.duration[DUR_SHROUD_OF_GOLUBRIA] ? 
             "You will soon reconstruct your protective shroud." :
             "Space distorts slightly along a thin shroud covering your body.");
        you.pb_on(PERMA_SHROUD); return SPRET_SUCCESS;
    }
}

spret_type cast_transform(int pow, transformation which_trans, bool fail)
{
    if (!transform(pow, which_trans, false, true)
        || !check_form_stat_safety(which_trans))
    {
        return SPRET_ABORT;
    }

    fail_check();
    transform(pow, which_trans);
    return SPRET_SUCCESS;
}

// Should this be one function?
void spell_drop_permabuffs(bool turn_off, bool end_durs, bool increase_durs,
                           int num, int size) {
    if (!(turn_off || end_durs || increase_durs)) {
        mprf(MSGCH_ERROR, "BUG: drop_permabuffs called to do nothing.");
    }
    if (end_durs && increase_durs) {
        mprf(MSGCH_ERROR, "BUG: drop_permabuffs called to end and increase.");
    }
    for (int i = PERMA_FIRST_PERMA; i <= PERMA_LAST_PERMA ; i++) {
        if (end_durs) you.duration[permabuff_durs[i]] = 0;
        if (increase_durs && you.permabuff[i]) {
            you.duration[permabuff_durs[i]] 
                // default 4d10
                = max(you.duration[permabuff_durs[i]],
                      BASELINE_DELAY * roll_dice(num,size));
        }
        if (turn_off) you.pb_off((permabuff_type) i);
        if (turn_off && end_durs) {
            you.perma_benefit[i] = you.perma_hunger[i] = you.perma_mp[i] =
                0;
        }
    }
    if ((increase_durs || turn_off) && you.props.exists(ORIGINAL_BRAND_KEY)) {
        end_weapon_brand(*you.weapon(), true);
    }
    if (turn_off) {
        you.props.erase(SHROUD_RECHARGE); you.props.erase(DMSL_RECHARGE);
        you.props[SONG_OF_SLAYING_KEY] = 0;
    }
}

// Why is this in spl-selfench? Hysterical raisins
bool permabuff_fail_check(permabuff_type pb, const string &message, 
                          bool ignoredur) {
    permabuff_track(pb);
    spell_type spell = permabuff_spell[pb];
    if (ignoredur ||
        one_chance_in((pb == PERMA_BATTLESPHERE) ?
                      battlesphere_max_charges() :
                      (BASELINE_DELAY * nominal_duration(spell)) /
                      (max(1, you.time_taken) * pb_dur_fudge[pb]))) {
        int fail = failure_check(spell, true);
        if (fail) {
            mprf(MSGCH_DURATION, "%s", message.c_str());
            apply_miscast(spell, fail, false);
            you.increase_duration(permabuff_durs[pb],roll_dice(2,10) + fail/4);
            you.perma_miscast[pb] = true;
            if (pb == PERMA_SONG) you.props[SONG_OF_SLAYING_KEY] = 0;
            interrupt_activity(AI_PB_MISCAST);
            return true;
        }
    }
    return false;
}
