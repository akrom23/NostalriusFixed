/*
 *
 */

#ifndef DEF_ETERNAL_BOND_H
#define DEF_ETERNAL_BOND_H

#include "scriptPCH.h"

enum
{
    // Dragons
    NPC_MERITHRA_OF_THE_DREAM           = 15378,
    NPC_CAELESTRASZ                     = 15379,
    NPC_ARYGOS                          = 15380,
    NPC_ANACHRONOS_THE_ANCIENT          = 15381,
    NPC_ANACHRONOS_QUEST_TRIGGER        = 15454,
    // marks some movement for the dragons

    // Elfs
    NPC_FANDRAL_STAGHELM                = 15382,
    NPC_KALDOREI_INFANTRY               = 15423,

    // Qiraji warriors
    NPC_QIRAJI_WASP                     = 15414,
    NPC_QIRAJI_DRONE                    = 15421,
    NPC_QIRAJI_TANK                     = 15422,
    NPC_ANUBISATH_CONQUEROR             = 15424,

    QUEST_A_PAWN_ON_THE_ETERNAL_BOARD   = 8519,

    // Yells -> in chronological order
    SAY_ANACHRONOS_INTRO_1              = -1000753,
    SAY_FANDRAL_INTRO_2                 = -1000754,
    SAY_MERITHRA_INTRO_3                = -1000755,
    EMOTE_ARYGOS_NOD                    = -1000756,
    SAY_CAELESTRASZ_INTRO_4             = -1000757,
    EMOTE_MERITHRA_GLANCE               = -1000758,
    SAY_MERITHRA_INTRO_5                = -1000759,

    SAY_MERITHRA_ATTACK_1               = -1000760,
    SAY_ARYGOS_ATTACK_2                 = -1000761,
    SAY_ARYGOS_ATTACK_3                 = -1000762,
    SAY_CAELESTRASZ_ATTACK_4            = -1000763,
    SAY_CAELESTRASZ_ATTACK_5            = -1000764,

    SAY_ANACHRONOS_SEAL_1               = -1000765,
    SAY_FANDRAL_SEAL_2                  = -1000766,
    SAY_ANACHRONOS_SEAL_3               = -1000767,
    SAY_ANACHRONOS_SEAL_4               = -1000768,
    SAY_ANACHRONOS_SEAL_5               = -1000769,
    SAY_FANDRAL_SEAL_6                  = -1000770,

    EMOTE_FANDRAL_EXHAUSTED             = -1000771,
    SAY_ANACHRONOS_EPILOGUE_1           = -1000772,
    SAY_ANACHRONOS_EPILOGUE_2           = -1000773,
    SAY_ANACHRONOS_EPILOGUE_3           = -1000774,
    EMOTE_ANACHRONOS_SCEPTER            = -1000775,
    SAY_FANDRAL_EPILOGUE_4              = -1000776,
    SAY_FANDRAL_EPILOGUE_5              = -1000777,
    EMOTE_FANDRAL_SHATTER               = -1000778,
    SAY_ANACHRONOS_EPILOGUE_6           = -1000779,
    SAY_FANDRAL_EPILOGUE_7              = -1000780,
    EMOTE_ANACHRONOS_DISPPOINTED        = -1000781,
    EMOTE_ANACHRONOS_PICKUP             = -1000782,
    SAY_ANACHRONOS_EPILOGUE_8           = -1000783,

    // The transform spell for Anachronos was removed from DBC
    DISPLAY_ID_BRONZE_DRAGON            = 15500,

    // Spells
    SPELL_GREEN_DRAGON_TRANSFORM        = 25105,
    SPELL_RED_DRAGON_TRANSFORM          = 25106,
    SPELL_BLUE_DRAGON_TRANSFORM         = 25107,
    SPELL_BRONZE_DRAGON_TRANSFORM       = 25108,            // Spell was removed - exists only before 2.0.1

    SPELL_MERITHRA_WAKE                 = 25145,            // should trigger 25172 on targets
    SPELL_ARYGOS_VENGEANCE              = 25149,            // should trigger 25168(visual)
    SPELL_CAELESTRASZ_MOLTEN_RAIN       = 25150,            // should trigger 25169(visual), 25170 on targets

    SPELL_TIME_STOP                     = 25158,            // Anachronos stops the battle - should trigger 25171
    SPELL_GLYPH_OF_WARDING              = 25166,            // Sends event 9427 - should activate Go 176148
    SPELL_PRISMATIC_BARRIER             = 25159,            // Sends event 9425 - should activate Go 176146
    SPELL_CALL_ANCIENTS                 = 25167,            // Sends event 9426 - should activate Go 176147
    SPELL_SHATTER_HAMMER                = 25182,            // Breakes the scepter - needs DB coords

    POINT_ID_DRAGON_ATTACK              = 1,
    POINT_ID_EXIT                       = 2,
    POINT_ID_GATE                       = 3,
    POINT_ID_SCEPTER_1                  = 4,
    POINT_ID_SCEPTER_2                  = 5,
    POINT_ID_EPILOGUE                   = 6,
    DATA_HANDLE_SCEPTER                 = 7,        // dummy members - used in dialogue helper
    DATA_MERITHRA_ATTACK                = 8,
    DATA_CAELASTRASZ_ATTACK             = 9,

    MAX_DRAGONS                         = 4,
    MAX_CONQUERORS                      = 3,
    MAX_QIRAJI                          = 6,
    MAX_KALDOREI                        = 20,
};

static const DialogueEntry aEventDialogue[] =
{
    { NPC_ANACHRONOS_THE_ANCIENT,    0,                          2000 },  // summon the dragons
    { SAY_ANACHRONOS_INTRO_1,        NPC_ANACHRONOS_THE_ANCIENT, 3000 },
    { EMOTE_ONESHOT_SHOUT,           NPC_ANACHRONOS_THE_ANCIENT, 3000 },  // make Anachronos shout and summon the warriors
    { SAY_FANDRAL_INTRO_2,           NPC_FANDRAL_STAGHELM,       6000 },
    { EMOTE_MERITHRA_GLANCE,         NPC_MERITHRA_OF_THE_DREAM,  2000 },
    { SAY_MERITHRA_INTRO_3,          NPC_MERITHRA_OF_THE_DREAM,  3000 },
    { EMOTE_ARYGOS_NOD,              NPC_ARYGOS,                 4000 },
    { SAY_CAELESTRASZ_INTRO_4,       NPC_CAELESTRASZ,            9000 },
    { SAY_MERITHRA_INTRO_5,          NPC_MERITHRA_OF_THE_DREAM,  5000 },
    { NPC_ANACHRONOS_QUEST_TRIGGER,  0,                          0 },     // send Merithra to fight
    { DATA_MERITHRA_ATTACK,          0,                          5000 },  // make Merithra wait
    { SAY_MERITHRA_ATTACK_1,         NPC_MERITHRA_OF_THE_DREAM,  1000 },
    { SPELL_GREEN_DRAGON_TRANSFORM,  0,                          6000 },
    { SAY_ARYGOS_ATTACK_2,           NPC_ARYGOS,                 5000 },
    { NPC_ARYGOS,                    0,                          1000 },  // send Arygos to fight
    { POINT_ID_EXIT,                 0,                          4000 },  // make Merithra exit
    { SAY_ARYGOS_ATTACK_3,           NPC_ARYGOS,                 4000 },
    { SPELL_BLUE_DRAGON_TRANSFORM,   0,                          5000 },
    { SPELL_ARYGOS_VENGEANCE,        0,                          7000 },
    { POINT_ID_DRAGON_ATTACK,        0,                          1000 },  // make Arygos exit
    { SAY_CAELESTRASZ_ATTACK_4,      NPC_CAELESTRASZ,            5000 },
    { NPC_CAELESTRASZ,               0,                          0 },     // send Caelestrasz to fight
    { DATA_CAELASTRASZ_ATTACK,       0,                          3000 },  // make Caelestrasz wait
    { SAY_CAELESTRASZ_ATTACK_5,      NPC_CAELESTRASZ,            5000 },
    { SPELL_RED_DRAGON_TRANSFORM,    0,                          4000 },  // transform Caelestrasz
    { SPELL_CAELESTRASZ_MOLTEN_RAIN, 0,                          6000 },  // Caelestrasz casts molten rain
    { SAY_ANACHRONOS_SEAL_1,         NPC_ANACHRONOS_THE_ANCIENT, 5000 },
    { SAY_FANDRAL_SEAL_2,            NPC_FANDRAL_STAGHELM,       3000 },
    { SAY_ANACHRONOS_SEAL_3,         NPC_ANACHRONOS_THE_ANCIENT, 1000 },
    { POINT_ID_GATE,                 0,                          1000 },  // send Anachronos to the gate
    { NPC_FANDRAL_STAGHELM,          0,                          0 },     // send Fandral to the gate
    { SPELL_TIME_STOP,               0,                          7000 },  // Anachronos casts Time Stop
    { SPELL_PRISMATIC_BARRIER,       0,                          15000 },
    { SPELL_GLYPH_OF_WARDING,        0,                          4000 },
    { SAY_ANACHRONOS_SEAL_5,         NPC_ANACHRONOS_THE_ANCIENT, 3000 },
    { SAY_FANDRAL_SEAL_6,            NPC_FANDRAL_STAGHELM,       9000 },
    { EMOTE_FANDRAL_EXHAUSTED,       NPC_FANDRAL_STAGHELM,       1000 },
    { SAY_ANACHRONOS_EPILOGUE_1,     NPC_ANACHRONOS_THE_ANCIENT, 6000 },
    { SAY_ANACHRONOS_EPILOGUE_2,     NPC_ANACHRONOS_THE_ANCIENT, 5000 },
    { SAY_ANACHRONOS_EPILOGUE_3,     NPC_ANACHRONOS_THE_ANCIENT, 15000 },
    { DATA_HANDLE_SCEPTER,           NPC_ANACHRONOS_THE_ANCIENT, 3000 },  // handle the scepter
    { SAY_FANDRAL_EPILOGUE_4,        NPC_FANDRAL_STAGHELM,       3000 },
    { POINT_ID_SCEPTER_2,            0,                          4000 },  // make Anachronos stand
    { SAY_FANDRAL_EPILOGUE_5,        NPC_FANDRAL_STAGHELM,       12000 },
    { EMOTE_FANDRAL_SHATTER,         NPC_FANDRAL_STAGHELM,       3000 },
    { SAY_ANACHRONOS_EPILOGUE_6,     NPC_ANACHRONOS_THE_ANCIENT, 0 },
    { SAY_FANDRAL_EPILOGUE_7,        NPC_FANDRAL_STAGHELM,       8000 },
    { POINT_ID_EPILOGUE,             0,                          4000 }, // move Fandral to Anachronos
    { EMOTE_ANACHRONOS_DISPPOINTED,  NPC_ANACHRONOS_THE_ANCIENT, 1000 },
    { POINT_ID_SCEPTER_1,            0,                          0 },    // make Anachronos pick the pieces
    { 0, 0, 0 },
};

struct EventLocations
{
    float m_fX, m_fY, m_fZ, m_fO;
    uint32 m_uiEntry;
};

static EventLocations aEternalBoardNPCs[MAX_DRAGONS] =
{
    { -8029.301f, 1534.612f, 2.609f, 3.121f, NPC_FANDRAL_STAGHELM },
    { -8034.227f, 1536.580f, 2.609f, 6.161f, NPC_ARYGOS },
    { -8031.935f, 1532.658f, 2.609f, 1.012f, NPC_CAELESTRASZ },
    { -8034.106f, 1534.224f, 2.609f, 0.290f, NPC_MERITHRA_OF_THE_DREAM },
};

static EventLocations aEternalBoardMovement[] =
{
    { -8159.951f, 1525.241f, 74.994f },           // 0 Flight position for dragons
    { -8106.238f, 1525.948f, 2.639f },            // 1 Anachronos gate location
    { -8103.861f, 1525.923f, 2.677f },            // 2 Fandral gate location
    { -8107.387f, 1523.641f, 2.609f },            // 3 Shattered scepter
    { -8100.921f, 1527.740f, 2.871f },            // 4 Fandral epilogue location
    { -8115.270f, 1515.926f, 3.305f },            // 5 Anachronos gather broken scepter 1
    { -8116.879f, 1530.615f, 3.762f },            // 6 Anachronos gather broken scepter 2
    { -7997.790f, 1548.664f, 3.738f },            // 7 Fandral exit location
    { -8061.933f, 1496.196f, 2.556f },            // 8 Anachronos launch location
    { -8008.705f, 1446.063f, 44.104f },           // 9 Anachronos flight location
    { -8085.748f, 1521.484f, 2.624f }             // 10 Anchor point for the army summoning
};

#endif
