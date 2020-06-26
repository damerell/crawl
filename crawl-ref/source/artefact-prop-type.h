#pragma once

// Be sure to update artefact_prop_data[] in artefact.cc. Also,
// _randart_propnames() in describe.cc, but order doesn't matter there.
enum artefact_prop_type
{
    ARTP_BRAND,
    ARTP_AC,
    ARTP_EVASION,
    ARTP_STRENGTH,
    ARTP_INTELLIGENCE,
    ARTP_DEXTERITY,
    ARTP_FIRE,
    ARTP_COLD,
    ARTP_ELECTRICITY,
    ARTP_POISON,
    ARTP_NEGATIVE_ENERGY,
    ARTP_MAGIC_RESISTANCE,
    ARTP_SEE_INVISIBLE,
    ARTP_INVISIBLE,
    ARTP_FLY,
    ARTP_BLINK,
    ARTP_BERSERK,
    ARTP_NOISE,
    ARTP_PREVENT_SPELLCASTING,
    ARTP_CAUSE_TELEPORTATION,
    ARTP_PREVENT_TELEPORTATION,
    ARTP_ANGRY,
#if TAG_MAJOR_VERSION == 34
    ARTP_METABOLISM,
#endif
    ARTP_CONTAM,
#if TAG_MAJOR_VERSION == 34
    ARTP_ACCURACY,
#endif
    ARTP_SLAYING,
    ARTP_CURSE,
    ARTP_STEALTH,
    ARTP_MAGICAL_POWER,
    ARTP_BASE_DELAY,
    ARTP_HP,
    ARTP_CLARITY,
    ARTP_BASE_ACC,
    ARTP_BASE_DAM,
    ARTP_RMSL,
    ARTP_FOG,
    ARTP_REGENERATION,
#if TAG_MAJOR_VERSION == 34
    ARTP_SUSTAT,
#endif
    ARTP_NO_UPGRADE,
    ARTP_RCORR,
    ARTP_RMUT,
#if TAG_MAJOR_VERSION == 34
    ARTP_TWISTER,
#endif
    ARTP_CORRODE,
    ARTP_DRAIN,
    ARTP_SLOW,
    ARTP_FRAGILE,
    ARTP_SHIELDING,
    ARTP_HARM,
    ARTP_NUM_PROPERTIES
};
