// This needs to be re-ordered when TAG_MAJOR_VERSION changes!

// Some Stoat Soup thoughts:
// If a book has more than six spells it is probably time to add a new book
// by looking at vanilla commit
// c126db5c3d258b00bf7526fc3b1a6620a9f6fd19
// or perhaps the history of http://crawl.chaosforge.org/Book
// for ideas for names and stealing spells from books with 5+ spells to fill
// it out.

// If you add a book don't forget to edit book_rarity in spl-book.cc
// and item-name.cc... and alphabet shops!

// If you add a spell it should probably turn up in about one non-starting
// book or two if it's meant to shore up a school that needs help
// Don't forget to check if Vehumet should support if it's not a Conjuration.

// It seems we are unlikely to divorce backgrounds from books as vanilla has
// done. To me, getting a "conjurer's kit" is pleasing. Those books could
// become rarer if book size in general is shrunk as vanilla has done.

// The Necronomicon, Grand Grimoire, and Annihilations are special. Spells
// in these don't appear in normal randbooks, Sif doesn't gift the books but
// can put the spells in her randbooks. So don't put a random spell looking
// for a home in one.

// item-prop.cc lists removed books.

static const vector<spell_type> spellbook_templates[] =
{

{   // Book of Minor Magic (wizard)
    SPELL_MAGIC_DART,
    SPELL_BLINK,
    SPELL_CALL_IMP,
    SPELL_SLOW,
    SPELL_CONJURE_FLAME,
    SPELL_MEPHITIC_CLOUD,
},

{   // Book of Conjurations (conjurer)
    SPELL_MAGIC_DART,
    SPELL_SEARING_RAY,
    SPELL_DAZZLING_SPRAY,
    SPELL_FULMINANT_PRISM,
    SPELL_FORCE_LANCE,
    SPELL_ISKENDERUNS_MYSTIC_BLAST,
},

{   // Book of Flames (fire elementalist)
    SPELL_FLAME_TONGUE,
    SPELL_THROW_FLAME,
    SPELL_CONJURE_FLAME,
    SPELL_INNER_FLAME,
    SPELL_STICKY_FLAME,
    SPELL_FIREBALL,
},

{   // Book of Frost (ice elementalist)
    SPELL_FREEZE,
    SPELL_THROW_FROST,
    SPELL_OZOCUBUS_ARMOUR,
    SPELL_THROW_ICICLE,
    SPELL_SUMMON_ICE_BEAST,
},

{   // Book of Summonings
    SPELL_RECALL,
    SPELL_AURA_OF_ABJURATION,
    SPELL_SUMMON_DEMON,
    SPELL_SUMMON_FOREST,
    SPELL_SUMMON_MANA_VIPER,
    SPELL_SHADOW_CREATURES,
},

{   // Book of Fire
    SPELL_IGNITE_POISON,
    SPELL_FIREBALL,
    SPELL_BOLT_OF_FIRE,
    SPELL_RING_OF_FLAMES,
    SPELL_IGNITION,
},

{   // Book of Ice
    SPELL_ICE_FORM,
    SPELL_ENGLACIATION,
    SPELL_OZOCUBUS_REFRIGERATION,
    SPELL_BOLT_OF_COLD,
    SPELL_FREEZING_CLOUD,
    SPELL_SIMULACRUM,
},

{   // Book of Spatial Translocations (warper)
    SPELL_BLINK,
    SPELL_SHROUD_OF_GOLUBRIA,
    SPELL_BECKONING,
    SPELL_GRAVITAS,
    SPELL_TELEPORT_OTHER,
    SPELL_GOLUBRIAS_PASSAGE,
},

{   // Book of Enchantments
    SPELL_SEE_INVISIBLE,
    SPELL_VIOLENT_UNRAVELLING,
    SPELL_SILENCE,
    SPELL_DEFLECT_MISSILES,
    SPELL_DISCORD,
    SPELL_HASTE,
},

{   // Young Poisoner's Handbook (venom mage)
    SPELL_STING,
    SPELL_POISONOUS_VAPOURS,
    SPELL_MEPHITIC_CLOUD,
    SPELL_OLGREBS_TOXIC_RADIANCE,
    SPELL_VENOM_BOLT,
},

{   // Book of the Tempests
    SPELL_DISCHARGE,
    SPELL_LIGHTNING_BOLT,
    SPELL_IGNITION,
    SPELL_TORNADO,
    SPELL_SHATTER,
},

{   // Book of Death
    SPELL_CORPSE_ROT,
    SPELL_SUBLIMATION_OF_BLOOD,
    SPELL_AGONY,
    SPELL_DISPEL_UNDEAD,
    SPELL_EXCRUCIATING_WOUNDS,
    SPELL_BOLT_OF_DRAINING,
},

{   // Book of Misfortune
    SPELL_CONFUSING_TOUCH,
    SPELL_CONFUSE,
    SPELL_GRAVITAS,
    SPELL_PETRIFY,
    SPELL_ENGLACIATION,
    SPELL_VIOLENT_UNRAVELLING,
},

{   // Book of Changes (transmuter)
    SPELL_BEASTLY_APPENDAGE,
    SPELL_STICKS_TO_SNAKES,
    SPELL_SPIDER_FORM,
    SPELL_ICE_FORM,
    SPELL_BLADE_HANDS,
},

{   // Book of Transfigurations
    SPELL_IRRADIATE,
    SPELL_STATUE_FORM,
    SPELL_HYDRA_FORM,
    SPELL_DRAGON_FORM,
},

{   // Fen Folio
    SPELL_CORPSE_ROT,
    SPELL_STONE_ARROW,
    SPELL_SUMMON_FOREST,
    SPELL_NOXIOUS_BOG,
    SPELL_HYDRA_FORM,
    SPELL_SUMMON_HYDRA,
},

#if TAG_MAJOR_VERSION > 34
{   // Book of Battle (skald)
    SPELL_INFUSION,
    SPELL_SHROUD_OF_GOLUBRIA,
    SPELL_SONG_OF_SLAYING,
    SPELL_SPECTRAL_WEAPON,
    SPELL_REGENERATION,
},
#endif
{   // Book of Clouds
    SPELL_POISONOUS_VAPOURS,
    SPELL_MEPHITIC_CLOUD,
    SPELL_CONJURE_FLAME,
    SPELL_FREEZING_CLOUD,
    SPELL_RING_OF_FLAMES,
},

{   // Book of Necromancy (necromancer)
    SPELL_NECROTISE,
    SPELL_VAMPIRIC_DRAINING,
    SPELL_REGENERATION,
    SPELL_ANIMATE_DEAD,
},

{   // Book of Callings
    SPELL_SUMMON_SMALL_MAMMAL,
    SPELL_CALL_IMP,
    SPELL_CALL_CANINE_FAMILIAR,
    SPELL_SUMMON_GUARDIAN_GOLEM,
    SPELL_SUMMON_LIGHTNING_SPIRE,
    SPELL_SUMMON_ICE_BEAST,
},

{   // Book of Maledictions (enchanter)
    SPELL_HIBERNATION,
    SPELL_CONFUSE,
    SPELL_TUKIMAS_DANCE,
    SPELL_DAZZLING_SPRAY,
},

{   // Book of Air (air elementalist)
    SPELL_SHOCK,
    SPELL_SWIFTNESS,
    SPELL_DISCHARGE,
    SPELL_AIRSTRIKE,
    SPELL_LIGHTNING_BOLT,
},

{   // Book of the Sky
    SPELL_SUMMON_LIGHTNING_SPIRE,
    SPELL_INSULATION,
    SPELL_SILENCE,
    SPELL_DEFLECT_MISSILES,
    SPELL_CONJURE_BALL_LIGHTNING,
    SPELL_TORNADO,
},

{   // Book of the Warp
    SPELL_GRAVITAS,
    SPELL_PORTAL_PROJECTILE,
    SPELL_FORCE_LANCE,
    SPELL_DISPERSAL,
    SPELL_CONTROLLED_BLINK,
    SPELL_DISJUNCTION,
},

{   // Book of Envenomations
    SPELL_SPIDER_FORM,
    SPELL_OLGREBS_TOXIC_RADIANCE,
    SPELL_VENOM_BOLT,
    SPELL_INTOXICATE,
    SPELL_NOXIOUS_BOG,
    SPELL_PERFECTED_RADIANCE,
},

{   // Book of Unlife
    SPELL_RECALL,
    SPELL_ANIMATE_DEAD,
    SPELL_BORGNJORS_VILE_CLUTCH,
    SPELL_DEATH_CHANNEL,
    SPELL_SIMULACRUM,
},

#if TAG_MAJOR_VERSION == 34
{   // Book of Control (removed)
    SPELL_ENGLACIATION,
},

{   // Book of Battle (skald)
    SPELL_INFUSION,
    SPELL_SHROUD_OF_GOLUBRIA,
    SPELL_SONG_OF_SLAYING,
    SPELL_SPECTRAL_WEAPON,
    SPELL_REGENERATION,
},
#endif

{   // Book of Geomancy (earth elementalist)
    SPELL_SANDBLAST,
    SPELL_PASSWALL,
    SPELL_STONE_ARROW,
    SPELL_PETRIFY,
    SPELL_LRD,
},

{   // Book of Earth
    SPELL_LEDAS_LIQUEFACTION,
    SPELL_BOLT_OF_MAGMA,
    SPELL_STATUE_FORM,
    SPELL_IRON_SHOT,
    SPELL_SHATTER,
},

#if TAG_MAJOR_VERSION == 34
{   // Book of Wizardry (removed)
    SPELL_FORCE_LANCE,
    SPELL_AGONY,
    SPELL_INVISIBILITY,
    SPELL_SPELLFORGED_SERVITOR,
    SPELL_HASTE,
},
#endif

{   // Book of Power
    SPELL_BATTLESPHERE,
    SPELL_BOLT_OF_MAGMA,
    SPELL_IRON_SHOT,
    SPELL_PERFECTED_RADIANCE,
    SPELL_IOOD,
    SPELL_SPELLFORGED_SERVITOR,
},

{   // Book of Cantrips
    SPELL_CONFUSING_TOUCH,
    SPELL_NECROTISE,
    SPELL_SUMMON_SMALL_MAMMAL,
    SPELL_APPORTATION,
},

{   // Book of Party Tricks
    SPELL_SUMMON_BUTTERFLIES,
    SPELL_APPORTATION,
    SPELL_BECKONING,
    SPELL_TUKIMAS_DANCE,
    SPELL_INTOXICATE,
    SPELL_INVISIBILITY
},

#if TAG_MAJOR_VERSION == 34
{   // Akashic Record (removed, theme was high tloc)
    SPELL_DISPERSAL,
    SPELL_MALIGN_GATEWAY,
    SPELL_CONTROLLED_BLINK,
    SPELL_DISJUNCTION,
},
#endif

{   // Book of Debilitation (arcane marksman)
    SPELL_SLOW,
    SPELL_INNER_FLAME,
    SPELL_PORTAL_PROJECTILE,
    SPELL_CAUSE_FEAR,
    SPELL_LEDAS_LIQUEFACTION,
},

{   // Book of the Dragon
    SPELL_FLAME_TONGUE,
    SPELL_CAUSE_FEAR,
    SPELL_BOLT_OF_FIRE,
    SPELL_DRAGON_FORM,
    SPELL_DRAGON_CALL,
},

{   // Book of Burglary
    SPELL_SWIFTNESS,
    SPELL_PASSWALL,
    SPELL_GOLUBRIAS_PASSAGE,
    SPELL_LRD,
    SPELL_DARKNESS,
    SPELL_INVISIBILITY,
},

{   // Book of Dreams
    SPELL_HIBERNATION,
    SPELL_BORGNJORS_VILE_CLUTCH,
    SPELL_SILENCE,
    SPELL_DARKNESS,
    SPELL_SHADOW_CREATURES,
},

{   // Book of Alchemy
    SPELL_SUBLIMATION_OF_BLOOD,
    SPELL_IGNITE_POISON,
    SPELL_PETRIFY,
    SPELL_INTOXICATE,
    SPELL_IRRADIATE,
    SPELL_NOXIOUS_BOG,
},

{   // Book of Beasts
    SPELL_SUMMON_BUTTERFLIES,
    SPELL_CALL_CANINE_FAMILIAR,
    SPELL_SUMMON_ICE_BEAST,
    SPELL_SUMMON_MANA_VIPER,
    SPELL_SUMMON_HYDRA,
},

{   // Book of Annihilations (special)
    SPELL_POISON_ARROW,
    SPELL_CHAIN_LIGHTNING,
    SPELL_LEHUDIBS_CRYSTAL_SPEAR,
    SPELL_GLACIATE,
    SPELL_FIRE_STORM,
    SPELL_ISKENDERUNS_UNDOING,
},

{   // Grand Grimoire (special)
    SPELL_MONSTROUS_MENAGERIE,
    SPELL_SUMMON_GREATER_DEMON,
    SPELL_MALIGN_GATEWAY,
    SPELL_SUMMON_HORRIBLE_THINGS,
},

{   // Necronomicon (special)
    SPELL_HAUNT,
    SPELL_BORGNJORS_REVIVIFICATION,
    SPELL_DEATHS_DOOR,
    SPELL_INFESTATION,
    SPELL_NECROMUTATION,
},

};

COMPILE_CHECK(ARRAYSZ(spellbook_templates) == 1 + MAX_FIXED_BOOK);
