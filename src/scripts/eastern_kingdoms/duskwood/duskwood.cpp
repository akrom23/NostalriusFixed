/*
 * Auteur        : Chakor
 * All rights reserved */

/* ScriptData
SDName: Duskwood
SD%Complete: 100
SDComment: Quest support: 252
SDCategory: Duskwood
EndScriptData */

/* ContentData
npc_lord_ello_ebonlocke
EndContentData */

#include "scriptPCH.h"

/*######
## npc_lord_ello_ebonlocke
######*/

enum
{
    NPC_TWILIGHT_CORRUPTER          = 15625,
    NPC_STITCHES                    = 412,

    SPELL_SOUL_CORRUPTION           = 25805,
    SPELL_CREATURE_OF_NIGHTMARE     = 25806,
    SPELL_SWELL_OF_SOULS            = 21307,

    QUEST_NIGHTMARE_CORRUPTION      = 8735,
    QUEST_TRANSLATION_TO_ELO        = 252,
};

bool GossipHello_npc_lord_ello_ebonlocke(Player* pPlayer, Creature* pCreature)
{
    /** Show quest menu */
    pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool QuestRewarded_npc_lord_ello_ebonlocke(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TRANSLATION_TO_ELO)
        pCreature->AI()->DoAction();
    return true;
}

struct elloEbonlockeAI : public ScriptedAI
{
    elloEbonlockeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    ObjectGuid stitchesGuid;
    void Reset() {}
    void DoAction()
    {
        if (m_creature->GetMap()->GetCreature(stitchesGuid))
            return;

        Creature* stitches = m_creature->SummonCreature(NPC_STITCHES, -10277.63f, 54.27f, 42.2f, 4.22f, TEMPSUMMON_DEAD_DESPAWN, 0, true);
        if (stitches)
            stitchesGuid = stitches->GetObjectGuid();
    }
};

CreatureAI* GetAI_ElloEbonlocke(Creature* pCreature)
{
    return new elloEbonlockeAI(pCreature);
}

bool AreaTrigger_at_twilight_grove(Player* pPlayer, const AreaTriggerEntry* pAt)
{
    if (pPlayer->isDead())
        return false;

    char wMessage[200];
    sprintf(wMessage, "Come, %s. See what the Nightmare brings...", pPlayer->GetName());

    if (pPlayer->GetQuestStatus(QUEST_NIGHTMARE_CORRUPTION) == QUEST_STATUS_INCOMPLETE)
        if (!pPlayer->FindNearestCreature(NPC_TWILIGHT_CORRUPTER, 350.0f))
            if (Creature* corrupter = pPlayer->SummonCreature(NPC_TWILIGHT_CORRUPTER, -10335.9f, -489.051f, 50.6233f, 2.59373f, TEMPSUMMON_DEAD_DESPAWN, 0))
                corrupter->MonsterWhisper(wMessage, (Unit*)pPlayer);
    return false;
}


struct npc_twilight_corrupterAI : public ScriptedAI
{
    npc_twilight_corrupterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSoulCorruptionTimer;
    uint32 m_uiCreatureOfNightmareTimer;
    uint32 m_uiCheckTimer;
    bool bEngaged;
    uint64 CoNPlayerGuid;
    float CoNPlayerAggro;
    uint64 GUIDs[40];

    void Reset()
    {
        m_uiSoulCorruptionTimer         = urand(6000, 18000);
        m_uiCreatureOfNightmareTimer    = urand(10000, 20000);
        m_uiCheckTimer  = 1000;
        CoNPlayerGuid   = 0;
        CoNPlayerAggro  = 0;
        bEngaged        = false;

        for (int i = 0; i < 40; i++)
            GUIDs[i] = 0;
    }

    void Aggro(Unit* pWho)
    {
        if (!bEngaged)
        {
            m_creature->MonsterSay("The Nightmare cannot be stopped!", 0, 0);
            bEngaged = true;
        }
    }

    void FillPlayerList()
    {
        for (int i = 0; i < 40; i++)
            GUIDs[i] = 0;

        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator i = tList.begin(); i != tList.end(); ++i)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*i)->getUnitGuid());

            if (pUnit && pUnit->IsPlayer())
                for (int i = 0; i < 40; i++)
                    if (GUIDs[i] == 0)
                        GUIDs[i] = pUnit->GetGUID();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSoulCorruptionTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SOUL_CORRUPTION) == CAST_OK)
                m_uiSoulCorruptionTimer = urand(20000, 30000);
        }
        else
            m_uiSoulCorruptionTimer -= uiDiff;

        if (CoNPlayerGuid)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetPlayer(CoNPlayerGuid))
            {
                if (!pTarget->HasAura(SPELL_CREATURE_OF_NIGHTMARE))
                {
                    m_creature->getThreatManager().modifyThreatPercent(pTarget, -100);
                    m_creature->getThreatManager().addThreatDirectly(pTarget, CoNPlayerAggro);
                    CoNPlayerGuid = 0;
                    CoNPlayerAggro = 0;
                }
            }
            else
            {
                CoNPlayerGuid = 0;
                CoNPlayerAggro = 0;
            }
        }

        if (m_uiCreatureOfNightmareTimer < uiDiff)
        {
            Unit* pTarg = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarg && pTarg->IsPlayer())
            {
                CoNPlayerGuid = pTarg->GetGUID();
                CoNPlayerAggro = m_creature->getThreatManager().getThreat(pTarg);
                if (DoCastSpellIfCan(pTarg, SPELL_CREATURE_OF_NIGHTMARE) == CAST_OK)
                    m_uiCreatureOfNightmareTimer = urand(35000, 40000);
            }
        }
        else
            m_uiCreatureOfNightmareTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff)
        {
            FillPlayerList();
            for (int i = 0; i < 40; i++)
            {
                if (Player* player = m_creature->GetMap()->GetPlayer(GUIDs[i]))
                {
                    if (player->isDead())
                    {
                        char eMessage[200];
                        sprintf(eMessage, "Twilight Corrupter squeezes the last bit of life out of %s and swallows their soul.", player->GetName());
                        m_creature->MonsterTextEmote(eMessage, NULL, false);
                        m_creature->CastSpell(m_creature, SPELL_SWELL_OF_SOULS, true);
                        GUIDs[i] = 0;
                    }
                }
            }
            m_uiCheckTimer = 1000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_twilight_corrupter(Creature* pCreature)
{
    return new npc_twilight_corrupterAI(pCreature);
}


struct watcherBlombergAI : public ScriptedAI
{
    watcherBlombergAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();

        m_creature->MonsterSay("The abomination is coming ! Doods ! Paige ! Come here and help us !", 0, 0);

        std::list<Creature*> m_EscortList;
        std::list<Creature*> m_EscortList2;

        GetCreatureListWithEntryInGrid(m_EscortList, m_creature, 888, 800.0f);
        for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
            (*it)->GetMotionMaster()->MovePoint(0, -10903.043945f, -377.539124f, 40.065773f);

        GetCreatureListWithEntryInGrid(m_EscortList2, m_creature, 499, 800.0f);
        for (std::list<Creature*>::iterator it = m_EscortList2.begin(); it != m_EscortList2.end(); ++it)
            (*it)->GetMotionMaster()->MovePoint(0, -10906.221680f, -375.957214f, 39.960278f);
        isEngaged = false;
        timerSpeach = 1000;
    }

    bool isEngaged;
    uint32 timerSpeach;

    void Reset()
    {
        m_creature->SetWalk(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (timerSpeach < uiDiff)
        {
            if (!isEngaged)
            {
                m_creature->MonsterSay("The abomination is coming! Doods! Paige! Come here and help us !", 0, 0);
                isEngaged = true;
            }
        }
        else
            timerSpeach -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_watcherBlomberg(Creature* pCreature)
{
    return new watcherBlombergAI(pCreature);
}

struct watcherSelkinAI : public npc_escortAI
{
    watcherSelkinAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
        isEngaged = false;
    }

    bool isEngaged;

    void Reset()
    {
        m_creature->SetWalk(false);
    }

    void WaypointReached(uint32 i)
    {
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!isEngaged)
        {
            isEngaged = true;
            Start(true, NULL, NULL, false);
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_watcherSelkin(Creature* pCreature)
{
    return new watcherSelkinAI(pCreature);
}

struct commanderFelstromAI : public ScriptedAI
{
    commanderFelstromAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool b_suicide;
    uint32 m_uiSuicide_Timer;

    void Reset()
    {
        m_uiSuicide_Timer = 1500;
        b_suicide = false;
        m_creature->ForceValuesUpdateAtIndex(UNIT_DYNAMIC_FLAGS);
    }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetEntry() == 771)
            m_creature->SetLootRecipient(NULL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSuicide_Timer < uiDiff)
        {
            if (m_creature->GetHealthPercent() <= 10.0f)
            {
                if (!b_suicide)
                {
                    DoCastSpellIfCan(m_creature, 3488, true);
                    b_suicide = true;
                }
            }
        }
        else
            m_uiSuicide_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_commanderFelstrom(Creature* pCreature)
{
    return new commanderFelstromAI(pCreature);
}

enum
{
    STITCHES_FIRST_WP         = 21,
    STITCHES_SECOND_WP        = 31,
    STITCHES_THIRD_WP         = 41,
    STITCHES_FOURFH_WP        = 48,
    STITCHES_FIFH_WP          = 64,

    NPC_WATCHER_CORWIN        = 1204,
    NPC_WATCHER_SARYS         = 1203,
};

struct StitchesAI : public npc_escortAI
{
    StitchesAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
        isEngaged = false;
        m_creature->SetActiveObjectState(true);
    }

    bool isEngaged;

    void Reset()
    {
        m_creature->CastSpell(m_creature, 3106, false);
    }

    void JustDied(Unit* pKiller)
    {
        std::list<Creature*> m_EscortList;

        GetCreatureListWithEntryInGrid(m_EscortList, m_creature, NPC_WATCHER_CORWIN, 800.0f);
        for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
        {
            if ((*it)->isAlive())
                (*it)->DisappearAndDie();
        }

        GetCreatureListWithEntryInGrid(m_EscortList, m_creature, NPC_WATCHER_SARYS, 800.0f);
        for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
        {
            if ((*it)->isAlive())
                (*it)->DisappearAndDie();
        }

        GetCreatureListWithEntryInGrid(m_EscortList, m_creature, 468, 400.0f);
        for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
        {
            if ((*it)->isAlive())
                (*it)->MonsterYell("The beast is slain! All's well in Darkshire!", 0, 0);
        }
    }

    void WaypointReached(uint32 i)
    {
        if (i == 10 || i == 29 || i == 61)
            m_creature->MonsterYellToZone(-1500000, 0, 0);
        if (i == STITCHES_FIRST_WP)
        {
            Creature* firstWP1 = m_creature->SummonCreature(1001,
                                 -10912.09f, -394.11f, 41.11f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            Creature* firstWP2 = m_creature->SummonCreature(1000,
                                 -10909.53f, -397.48f, 41.14f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);

            firstWP1->GetMotionMaster()->MovePoint(0, -10905.52f, -374.1f, 39.88f);
            firstWP1->SetCombatStartPosition(-10905.52f, -374.1f, 39.88f);
            firstWP1->SetHomePosition(-10905.52f, -374.1f, 39.88f, 0.0f);
            firstWP1->SetWalk(false);

            firstWP2->GetMotionMaster()->MovePoint(0, -10902.211914f, -375.488495f, 40.000954f);
            firstWP2->SetCombatStartPosition(-10902.211914f, -375.488495f, 40.000954f);
            firstWP2->SetHomePosition(-10902.211914f, -375.488495f, 40.000954f, 0.0f);
            firstWP2->SetWalk(false);
        }
        if (i == STITCHES_FIRST_WP + 2)
        {
            std::list<Creature*> m_EscortList;

            GetCreatureListWithEntryInGrid(m_EscortList, m_creature, 468, 2000.0f);
            for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
            {
                if ((*it)->isAlive())
                    (*it)->MonsterYell("The abomination has overrun the Night Watch camp! Quickly, we must intercept it before it reaches town!", 0, 0);
            }
        }
        else if (i == STITCHES_SECOND_WP + 2)
        {
            Creature* secondWP1 = m_creature->SummonCreature(1436,
                                  -10910.273438f, -519.029175f, 52.996330f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            secondWP1->SetWalk(false);

            secondWP1->GetMotionMaster()->MovePoint(0, -10904.029297f, -374.387299f, 39.899269f);
            secondWP1->SetCombatStartPosition(-10904.029297f, -374.387299f, 39.899269f);
            secondWP1->SetHomePosition(-10904.029297f, -374.387299f, 39.899269f, 0.0f);
        }
        else if (i == STITCHES_THIRD_WP)
        {
            Creature* leader = m_creature->SummonCreature(1100,
                               -10618.215820f, -1185.354858f, 28.577244f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            Creature* npc1 = m_creature->SummonCreature(1099,
                             -10618.901367f, -1182.151367f, 28.570557f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            Creature* npc2 = m_creature->SummonCreature(1098,
                             -10616.639648f, -1181.667236f, 28.490921f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            Creature* npc3 = m_creature->SummonCreature(1101,
                             -10615.996094f, -1184.561646f, 28.458422f, 5.93f, TEMPSUMMON_CORPSE_DESPAWN, 10000);

            npc1->JoinCreatureGroup(leader,
                                    leader->GetAngle(npc1) - (npc1)->GetOrientation(),
                                    0.4f,
                                    OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER | OPTION_EVADE_TOGETHER);
            npc2->JoinCreatureGroup(leader,
                                    leader->GetAngle(npc2) - (npc2)->GetOrientation(),
                                    0.4f,
                                    OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER | OPTION_EVADE_TOGETHER);
            npc3->JoinCreatureGroup(leader,
                                    leader->GetAngle(npc3) - (npc3)->GetOrientation(),
                                    0.4f,
                                    OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER | OPTION_EVADE_TOGETHER);

            leader->SetWalk(false);

            //leader->Start();

        }
        else if (i == 61)
        {
            m_creature->SummonCreature(NPC_WATCHER_SARYS,
                                       -10574.304688f, -1179.062134f, 28.033930f, 3.05f, TEMPSUMMON_CORPSE_DESPAWN, 10000);
            m_creature->SummonCreature(NPC_WATCHER_CORWIN,
                                       -10575.126953f, -1170.067261f, 28.251972f, 3.61f, TEMPSUMMON_CORPSE_DESPAWN, 10000);

            std::list<Creature*> m_EscortList;

            GetCreatureListWithEntryInGrid(m_EscortList, m_creature, 468, 800.0f);
            for (std::list<Creature*>::iterator it = m_EscortList.begin(); it != m_EscortList.end(); ++it)
            {
                if ((*it)->isAlive())
                    (*it)->MonsterYell("The abomination has come! Forward!", 0, 0);
            }
        }
        else if (i == STITCHES_FIFH_WP)
        {
            m_creature->SetHomePosition(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);
            Stop();
        }
    }

    void Aggro(Unit* pWho)
    {
        uint32 yellChance = urand(0, 3);
        if (!yellChance)
            m_creature->MonsterYell("ROAAAARR!", 0, 0);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!isEngaged)
        {
            isEngaged = true;
            Start(false, NULL, NULL, false);
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_stitches(Creature* pCreature)
{
    return new StitchesAI(pCreature);
}


void AddSC_duskwood()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "commander_felstrom";
    newscript->GetAI = &GetAI_commanderFelstrom;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "watcher_selkin";
    newscript->GetAI = &GetAI_watcherSelkin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "watcher_blomberg";
    newscript->GetAI = &GetAI_watcherBlomberg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_stitches";
    newscript->GetAI = &GetAI_stitches;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lord_ello_ebonlocke";
    newscript->GetAI = &GetAI_ElloEbonlocke;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_lord_ello_ebonlocke;
    newscript->pGossipHello = &GossipHello_npc_lord_ello_ebonlocke;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_twilight_grove";
    newscript->pAreaTrigger = &AreaTrigger_at_twilight_grove;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_twilight_corrupter";
    newscript->GetAI = &GetAI_npc_twilight_corrupter;
    newscript->RegisterSelf();
}
