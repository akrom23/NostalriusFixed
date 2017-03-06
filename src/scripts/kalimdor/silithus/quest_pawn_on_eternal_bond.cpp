/*
 *
 */

#include "scriptPCH.h"
#include "quest_pawn_on_eternal_bond.h"

 /*###
 ## npc_anachronos_the_ancient
 ###*/

struct npc_anachronos_the_ancientAI : ScriptedAI, private DialogueHelper
{
    explicit npc_anachronos_the_ancientAI(Creature* pCreature) : ScriptedAI(pCreature), DialogueHelper(aEventDialogue)
    {
        npc_anachronos_the_ancientAI::Reset();
    }

    uint32 m_uiEventTimer;

    uint8 m_uiEventStage;

    ObjectGuid m_fandralGuid;
    ObjectGuid m_merithraGuid;
    ObjectGuid m_CaelestraszGuid;
    ObjectGuid m_arygosGuid;
    ObjectGuid m_playerGuid;
    ObjectGuid m_triggerGuid;

    GuidList m_lQirajiWarriorsList;

    void Reset() override
    {
        // We summon the rest of the dragons on timer
        m_uiEventTimer = 100;
        m_uiEventStage = 0;
        m_creature->setFaction(35);
    }

    void JustDidDialogueStep(int32 iEntry) override
    {
        switch (iEntry)
        {
        case NPC_ANACHRONOS_THE_ANCIENT:
            // Call the other dragons
            DoSummonDragons();
            break;
        case EMOTE_ONESHOT_SHOUT:
            // Summon warriors
            DoSummonWarriors();
            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
            break;
        case SAY_FANDRAL_INTRO_2:
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->SetFacingToObject(m_creature);
            break;
        case EMOTE_MERITHRA_GLANCE:
            if (Creature* pMerithra = m_creature->GetMap()->GetCreature(m_merithraGuid))
            {
                if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                    pFandral->SetFacingToObject(pMerithra);
            }
            break;
        case NPC_ANACHRONOS_QUEST_TRIGGER:
            // Move Merithra to attack
            if (Creature* pTrigger = GetClosestCreatureWithEntry(m_creature, NPC_ANACHRONOS_QUEST_TRIGGER, 35.0f))
            {
                m_triggerGuid = pTrigger->GetObjectGuid();
                if (Creature* pMerithra = m_creature->GetMap()->GetCreature(m_merithraGuid))
                {
                    pMerithra->SetWalk(false);
                    pMerithra->GetMotionMaster()->MovePoint(POINT_ID_DRAGON_ATTACK, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ());
                }
            }
            break;
        case SPELL_GREEN_DRAGON_TRANSFORM:
            if (Creature* pMerithra = m_creature->GetMap()->GetCreature(m_merithraGuid))
            {
                pMerithra->CastSpell(pMerithra, SPELL_GREEN_DRAGON_TRANSFORM, false);
                pMerithra->SetLevitate(true);
            }
            break;
        case SAY_ARYGOS_ATTACK_2:
            if (Creature* pMerithra = m_creature->GetMap()->GetCreature(m_merithraGuid))
            {
                pMerithra->CastSpell(pMerithra, SPELL_MERITHRA_WAKE, false);
                DoCastTriggerSpellOnEnemies(25172);
            }
            break;
        case NPC_ARYGOS:
            // Move Arygos to attack
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_triggerGuid))
            {
                if (Creature* pArygos = m_creature->GetMap()->GetCreature(m_arygosGuid))
                {
                    pArygos->SetWalk(false);
                    pArygos->GetMotionMaster()->MovePoint(POINT_ID_DRAGON_ATTACK, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ());
                }
            }
            break;
        case POINT_ID_EXIT:
            // Move Merithra to the exit point
            if (Creature* pMerithra = m_creature->GetMap()->GetCreature(m_merithraGuid))
            {
                pMerithra->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND /*| UNIT_BYTE1_FLAG_UNK_2 */);
                pMerithra->GetMotionMaster()->MovePoint(POINT_ID_EXIT, aEternalBoardMovement[0].m_fX, aEternalBoardMovement[0].m_fY, aEternalBoardMovement[0].m_fZ);
                pMerithra->ForcedDespawn(9000);
            }
            break;
        case SPELL_BLUE_DRAGON_TRANSFORM:
            if (Creature* pArygos = m_creature->GetMap()->GetCreature(m_arygosGuid))
            {
                pArygos->CastSpell(pArygos, SPELL_BLUE_DRAGON_TRANSFORM, false);
                pArygos->SetLevitate(true);
            }
            break;
        case SPELL_ARYGOS_VENGEANCE:
            if (Creature* pArygos = m_creature->GetMap()->GetCreature(m_arygosGuid))
            {
                pArygos->CastSpell(pArygos, SPELL_ARYGOS_VENGEANCE, false);
                DoCastTriggerSpellOnEnemies(25168);
            }
            break;
        case POINT_ID_DRAGON_ATTACK:
            // Move Arygos to the exit point
            if (Creature* pArygos = m_creature->GetMap()->GetCreature(m_arygosGuid))
            {
                pArygos->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND /*| UNIT_BYTE1_FLAG_UNK_2 */);
                pArygos->GetMotionMaster()->MovePoint(POINT_ID_EXIT, aEternalBoardMovement[0].m_fX, aEternalBoardMovement[0].m_fY, aEternalBoardMovement[0].m_fZ);
                pArygos->ForcedDespawn(9000);
            }
            break;
        case NPC_CAELESTRASZ:
            // Move Caelestrasz to attack
            if (Creature* pTrigger = m_creature->GetMap()->GetCreature(m_triggerGuid))
            {
                if (Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_CaelestraszGuid))
                {
                    pCaelestrasz->SetWalk(false);
                    pCaelestrasz->GetMotionMaster()->MovePoint(POINT_ID_DRAGON_ATTACK, pTrigger->GetPositionX(), pTrigger->GetPositionY(), pTrigger->GetPositionZ());
                }
            }
            break;
        case SPELL_RED_DRAGON_TRANSFORM:
            if (Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_CaelestraszGuid))
            {
                pCaelestrasz->CastSpell(pCaelestrasz, SPELL_RED_DRAGON_TRANSFORM, false);
                pCaelestrasz->SetLevitate(true);
            }
            break;
        case SPELL_CAELESTRASZ_MOLTEN_RAIN:
            if (Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_CaelestraszGuid))
            {
                pCaelestrasz->CastSpell(pCaelestrasz, SPELL_CAELESTRASZ_MOLTEN_RAIN, false);
                DoCastTriggerSpellOnEnemies(25169);
                DoCastTriggerSpellOnEnemies(25170);
            }
            break;
        case SAY_ANACHRONOS_SEAL_1:
            // Send Caelestrasz on flight
            if (Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_CaelestraszGuid))
            {
                pCaelestrasz->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND /*| UNIT_BYTE1_FLAG_UNK_2*/);
                pCaelestrasz->GetMotionMaster()->MovePoint(POINT_ID_EXIT, aEternalBoardMovement[0].m_fX, aEternalBoardMovement[0].m_fY, aEternalBoardMovement[0].m_fZ);
                pCaelestrasz->ForcedDespawn(9000);
            }
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                m_creature->SetFacingToObject(pFandral);
            break;
        case SAY_FANDRAL_SEAL_2:
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->SetFacingToObject(m_creature);
            break;
        case POINT_ID_GATE:
            // Send Anachronos to the gate
            m_creature->SetWalk(false);
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_GATE, aEternalBoardMovement[1].m_fX, aEternalBoardMovement[1].m_fY, aEternalBoardMovement[1].m_fZ);
            break;
        case NPC_FANDRAL_STAGHELM:
            // Send Fandral to the gate
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
            {
                pFandral->SetWalk(false);
                pFandral->GetMotionMaster()->MovePoint(POINT_ID_GATE, aEternalBoardMovement[2].m_fX, aEternalBoardMovement[2].m_fY, aEternalBoardMovement[2].m_fZ);
            }
            break;
        case SPELL_PRISMATIC_BARRIER:
            DoCastSpellIfCan(m_creature, SPELL_PRISMATIC_BARRIER);
            break;
        case SPELL_GLYPH_OF_WARDING:
            DoCastSpellIfCan(m_creature, SPELL_GLYPH_OF_WARDING);
            break;
        case SAY_FANDRAL_SEAL_6:
            // Here Anachronos should continue to cast something
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->CastSpell(pFandral, SPELL_CALL_ANCIENTS, false);
            break;
        case EMOTE_FANDRAL_EXHAUSTED:
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
            {
                pFandral->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_creature->SetFacingToObject(pFandral);
            }
            break;
        case DATA_HANDLE_SCEPTER:
            // Give the scepter to Fandral (it should equip it somehow)
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                DoScriptText(EMOTE_ANACHRONOS_SCEPTER, m_creature, pFandral);
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
            break;
        case SAY_FANDRAL_EPILOGUE_4:
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->SetStandState(UNIT_STAND_STATE_STAND);
            break;
        case POINT_ID_SCEPTER_2:
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);
            break;
        case EMOTE_FANDRAL_SHATTER:
            // Shatter the scepter
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->CastSpell(pFandral, SPELL_SHATTER_HAMMER, false);
            break;
        case SAY_ANACHRONOS_EPILOGUE_6:
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
            {
                pFandral->SetWalk(true);
                pFandral->GetMotionMaster()->MovePoint(POINT_ID_SCEPTER_1, aEternalBoardMovement[3].m_fX, aEternalBoardMovement[3].m_fY, aEternalBoardMovement[3].m_fZ);
            }
            break;
        case POINT_ID_EPILOGUE:
            // Make Fandral leave
            if (Creature* pFandral = m_creature->GetMap()->GetCreature(m_fandralGuid))
                pFandral->GetMotionMaster()->MovePoint(POINT_ID_EXIT, aEternalBoardMovement[7].m_fX, aEternalBoardMovement[7].m_fY, aEternalBoardMovement[7].m_fZ);
            break;
        case POINT_ID_SCEPTER_1:
            // Anachronos collects the pieces
            m_creature->SetWalk(true);
            m_creature->GetMotionMaster()->MovePoint(POINT_ID_SCEPTER_1, aEternalBoardMovement[5].m_fX, aEternalBoardMovement[5].m_fY, aEternalBoardMovement[5].m_fZ);
            break;
        }
    }

    Creature* GetSpeakerByEntry(uint32 uiEntry) override
    {
        switch (uiEntry)
        {
        case NPC_ANACHRONOS_THE_ANCIENT: return m_creature;
        case NPC_ARYGOS:                 return m_creature->GetMap()->GetCreature(m_arygosGuid);
        case NPC_CAELESTRASZ:            return m_creature->GetMap()->GetCreature(m_CaelestraszGuid);
        case NPC_MERITHRA_OF_THE_DREAM:  return m_creature->GetMap()->GetCreature(m_merithraGuid);
        case NPC_FANDRAL_STAGHELM:       return m_creature->GetMap()->GetCreature(m_fandralGuid);

        default:
            return nullptr;
        }
    }

    void DoSummonDragons() const
    {
        for (uint8 i = 0; i < MAX_DRAGONS; ++i)
            m_creature->SummonCreature(aEternalBoardNPCs[i].m_uiEntry, aEternalBoardNPCs[i].m_fX, aEternalBoardNPCs[i].m_fY, aEternalBoardNPCs[i].m_fZ, aEternalBoardNPCs[i].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }

    void DoSummonWarriors() const
    {
        float fX, fY, fZ;

        // Summon kaldorei warriors
        for (uint8 i = 0; i < MAX_KALDOREI; ++i)
        {
            m_creature->GetRandomPoint(aEternalBoardMovement[10].m_fX, aEternalBoardMovement[10].m_fY, aEternalBoardMovement[10].m_fZ, 10.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_KALDOREI_INFANTRY, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }

        // Summon the 3 anubisath conquerors
        for (uint8 i = 0; i < MAX_CONQUERORS; ++i)
        {
            m_creature->GetRandomPoint(aEternalBoardMovement[10].m_fX, aEternalBoardMovement[10].m_fY, aEternalBoardMovement[10].m_fZ, 20.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_ANUBISATH_CONQUEROR, fX, fY, fZ, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }

        // Summon Qiraji warriors
        for (uint8 i = 0; i < MAX_QIRAJI; ++i)
        {
            m_creature->GetRandomPoint(aEternalBoardMovement[10].m_fX, aEternalBoardMovement[10].m_fY, aEternalBoardMovement[10].m_fZ, 15.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_QIRAJI_WASP, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

            m_creature->GetRandomPoint(aEternalBoardMovement[10].m_fX, aEternalBoardMovement[10].m_fY, aEternalBoardMovement[10].m_fZ, 15.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_QIRAJI_DRONE, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

            m_creature->GetRandomPoint(aEternalBoardMovement[10].m_fX, aEternalBoardMovement[10].m_fY, aEternalBoardMovement[10].m_fZ, 15.0f, fX, fY, fZ);
            m_creature->SummonCreature(NPC_QIRAJI_TANK, fX, fY, fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }
    }

    void DoUnsummonArmy()
    {
        for (GuidList::const_iterator itr = m_lQirajiWarriorsList.begin(); itr != m_lQirajiWarriorsList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                pTemp->ForcedDespawn();
        }
    }

    void DoTimeStopArmy()
    {
        for (GuidList::const_iterator itr = m_lQirajiWarriorsList.begin(); itr != m_lQirajiWarriorsList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                // Cast stone visual, stop movement/attacks and freeze whole combat
                pTemp->RemoveAllAttackers();
                pTemp->AttackStop();
                pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pTemp->StopMoving();
                pTemp->GetMotionMaster()->Clear(true);
                pTemp->CastSpell(pTemp, 25171, true);
                pTemp->CastSpell(pTemp, 15533, true);
            }
        }
    }

    void DoCastTriggerSpellOnEnemies(uint32 spell)
    {
        for (GuidList::const_iterator itr = m_lQirajiWarriorsList.begin(); itr != m_lQirajiWarriorsList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                // Cast trigger spell only on enemies
                if (pTemp->GetEntry() == NPC_ANUBISATH_CONQUEROR || pTemp->GetEntry() == NPC_QIRAJI_DRONE ||
                    pTemp->GetEntry() == NPC_QIRAJI_TANK || pTemp->GetEntry() == NPC_QIRAJI_WASP)
                {
                    if (spell == 25172)
                    {
                        if (pTemp->GetEntry() == NPC_ANUBISATH_CONQUEROR)
                            pTemp->CastSpell(pTemp, spell, true);
                    }
                    else if (spell == 25170)
                    {
                        if (pTemp->GetEntry() == NPC_ANUBISATH_CONQUEROR)
                            pTemp->CastSpell(pTemp, spell, true);
                    }
                    else
                        pTemp->CastSpell(pTemp, spell, true);
                }
            }
        }
    }

    void JustSummoned(Creature* pSummoned) override
    {
        // Also remove npc flags where needed
        switch (pSummoned->GetEntry())
        {
        case NPC_FANDRAL_STAGHELM:
            m_fandralGuid = pSummoned->GetObjectGuid();
            pSummoned->setFaction(35);
            break;
        case NPC_MERITHRA_OF_THE_DREAM:
            m_merithraGuid = pSummoned->GetObjectGuid();
            pSummoned->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            pSummoned->setFaction(35);
            break;
        case NPC_CAELESTRASZ:
            m_CaelestraszGuid = pSummoned->GetObjectGuid();
            pSummoned->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            pSummoned->setFaction(35);
            break;
        case NPC_ARYGOS:
            m_arygosGuid = pSummoned->GetObjectGuid();
            pSummoned->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            pSummoned->setFaction(35);
            break;
        case NPC_ANUBISATH_CONQUEROR:
        case NPC_QIRAJI_WASP:
        case NPC_QIRAJI_DRONE:
        case NPC_QIRAJI_TANK:
            m_lQirajiWarriorsList.push_back(pSummoned->GetObjectGuid());
            pSummoned->setFaction(14);
            if (Creature* target = GetClosestCreatureWithEntry(pSummoned, NPC_KALDOREI_INFANTRY, 90.0f))
            {
                pSummoned->AddThreat(target, 100.0f);
                pSummoned->AI()->AttackStart(target);
                target->AddThreat(pSummoned, 100.0f);
                target->AI()->AttackStart(pSummoned);
            }
            break;
        case NPC_KALDOREI_INFANTRY:
            m_lQirajiWarriorsList.push_back(pSummoned->GetObjectGuid());
            pSummoned->setFaction(996);
            break;
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
        case POINT_ID_GATE:
            // Cast time stop when he reaches the gate
            DoCastSpellIfCan(m_creature, SPELL_TIME_STOP);
            DoTimeStopArmy();
            StartNextDialogueText(SPELL_TIME_STOP);
            break;
        case POINT_ID_SCEPTER_1:
            // Pickup the pieces
            DoScriptText(EMOTE_ANACHRONOS_PICKUP, m_creature);
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
            m_uiEventTimer = 2000;
            break;
        case POINT_ID_SCEPTER_2:
            // Pickup the pieces
            DoScriptText(SAY_ANACHRONOS_EPILOGUE_8, m_creature);
            m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
            m_uiEventTimer = 4000;
            break;
        case POINT_ID_EXIT:
            DoCastSpellIfCan(m_creature, SPELL_BRONZE_DRAGON_TRANSFORM);
            m_creature->SetLevitate(true);
            m_uiEventTimer = 4000;
            break;
        }
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (pSummoned->GetEntry() == NPC_FANDRAL_STAGHELM)
        {
            switch (uiPointId)
            {
            case POINT_ID_EPILOGUE:
                // Face Anachronos and restart the dialogue
                pSummoned->SetFacingToObject(m_creature);
                StartNextDialogueText(SAY_FANDRAL_EPILOGUE_7);
                DoUnsummonArmy();
                break;
            case POINT_ID_SCEPTER_1:
                pSummoned->GetMotionMaster()->MovePoint(POINT_ID_EPILOGUE, aEternalBoardMovement[4].m_fX, aEternalBoardMovement[4].m_fY, aEternalBoardMovement[4].m_fZ);
                break;
            case POINT_ID_EXIT:
                pSummoned->ForcedDespawn();
                break;
            }
        }
        else if (uiPointId == POINT_ID_DRAGON_ATTACK)
        {
            switch (pSummoned->GetEntry())
            {
            case NPC_MERITHRA_OF_THE_DREAM:
                StartNextDialogueText(DATA_MERITHRA_ATTACK);
                break;
            case NPC_CAELESTRASZ:
                StartNextDialogueText(DATA_CAELASTRASZ_ATTACK);
                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        DialogueUpdate(uiDiff);

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                switch (m_uiEventStage)
                {
                case 0:
                    // Start the dialogue
                    StartNextDialogueText(NPC_ANACHRONOS_THE_ANCIENT);
                    m_uiEventTimer = 0;
                    break;
                case 1:
                    // Do the epilogue movement
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_SCEPTER_2, aEternalBoardMovement[6].m_fX, aEternalBoardMovement[6].m_fY, aEternalBoardMovement[6].m_fZ);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    m_uiEventTimer = 0;
                    break;
                case 2:
                    // Complete quest and despawn gate
                    if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        pPlayer->GroupEventHappens(QUEST_A_PAWN_ON_THE_ETERNAL_BOARD, m_creature);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                    m_uiEventTimer = 4000;
                    break;
                case 3:
                    // Move to exit
                    m_creature->SetWalk(false);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_EXIT, aEternalBoardMovement[8].m_fX, aEternalBoardMovement[8].m_fY, aEternalBoardMovement[8].m_fZ);
                    m_uiEventTimer = 0;
                    break;
                case 4:
                    // Take off and fly
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND /* | UNIT_BYTE1_FLAG_UNK_2 */);
                    m_creature->GetMotionMaster()->MovePoint(0, aEternalBoardMovement[9].m_fX, aEternalBoardMovement[9].m_fY, aEternalBoardMovement[9].m_fZ);
                    m_creature->ForcedDespawn(10000);
                    m_uiEventTimer = 0;
                    break;
                }
                ++m_uiEventStage;
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_anachronos_the_ancient(Creature* pCreature)
{
    return new npc_anachronos_the_ancientAI(pCreature);
}

bool QuestAcceptGO_crystalline_tear(Player* pPlayer, GameObject* pGo, const Quest* pQuest)
{
    // Summon the controller dragon at GO position (orientation is wrong - hardcoded)
    if (pQuest->GetQuestId() == QUEST_A_PAWN_ON_THE_ETERNAL_BOARD)
    {
        // Check if event is already in progress first
        if (Creature* pAnachronos = GetClosestCreatureWithEntry(pGo, NPC_ANACHRONOS_THE_ANCIENT, 90.0f))
            return true;

        if (Creature* pAnachronos = pPlayer->SummonCreature(NPC_ANACHRONOS_THE_ANCIENT, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 3.75f, TEMPSUMMON_CORPSE_DESPAWN, 0))
        {
            // Send the player's guid in order to handle the quest complete
            if (npc_anachronos_the_ancientAI* pAnachronosAI = dynamic_cast<npc_anachronos_the_ancientAI*>(pAnachronos->AI()))
                pAnachronosAI->m_playerGuid = pPlayer->GetObjectGuid();
        }
    }

    return true;
}

void AddSC_quest_pawn_on_eternal_bond()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_anachronos_the_ancient";
    pNewScript->GetAI = &GetAI_npc_anachronos_the_ancient;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_crystalline_tear";
    pNewScript->pGOQuestAccept = &QuestAcceptGO_crystalline_tear;
    pNewScript->RegisterSelf();
}