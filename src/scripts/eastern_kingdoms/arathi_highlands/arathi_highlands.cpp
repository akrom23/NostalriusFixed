/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Arathi Highlands
SD%Complete: 100
SDComment: Quest support: 665
SDCategory: Arathi Highlands
EndScriptData */

/* ContentData
npc_professor_phizzlethorpe
EndContentData */

#include "scriptPCH.h"

/*######
## npc_professor_phizzlethorpe
######*/

enum
{
    SAY_PROGRESS_1          = -1000264,
    SAY_PROGRESS_2          = -1000265,
    SAY_PROGRESS_3          = -1000266,
    EMOTE_PROGRESS_4        = -1000267,
    SAY_AGGRO               = -1000268,
    SAY_PROGRESS_5          = -1000269,
    SAY_PROGRESS_6          = -1000270,
    SAY_PROGRESS_7          = -1000271,
    EMOTE_PROGRESS_8        = -1000272,
    SAY_PROGRESS_9          = -1000273,

    QUEST_SUNKEN_TREASURE   = 665,
    ENTRY_VENGEFUL_SURGE    = 2776
};

struct npc_professor_phizzlethorpeAI : public npc_escortAI
{
    npc_professor_phizzlethorpeAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
            case 4:
                DoScriptText(SAY_PROGRESS_2, m_creature, pPlayer);
                break;
            case 5:
                DoScriptText(SAY_PROGRESS_3, m_creature, pPlayer);
                break;
            case 8:
                DoScriptText(EMOTE_PROGRESS_4, m_creature);
                break;
            case 9:
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2056.41f, -2144.01f, 20.59f, 5.70f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2050.17f, -2140.02f, 19.54f, 5.17f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                break;
            case 10:
                DoScriptText(SAY_PROGRESS_5, m_creature, pPlayer);
                break;
            case 11:
                DoScriptText(SAY_PROGRESS_6, m_creature, pPlayer);
                SetRun();
                break;
            case 19:
                DoScriptText(SAY_PROGRESS_7, m_creature, pPlayer);
                break;
            case 20:
                DoScriptText(EMOTE_PROGRESS_8, m_creature);
                DoScriptText(SAY_PROGRESS_9, m_creature, pPlayer);
                pPlayer->GroupEventHappens(QUEST_SUNKEN_TREASURE, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
    }
};

bool QuestAccept_npc_professor_phizzlethorpe(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUNKEN_TREASURE)
    {
        pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
        DoScriptText(SAY_PROGRESS_1, pCreature, pPlayer);

        if (npc_professor_phizzlethorpeAI* pEscortAI = dynamic_cast<npc_professor_phizzlethorpeAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest, true);
    }
    return true;
}

CreatureAI* GetAI_npc_professor_phizzlethorpe(Creature* pCreature)
{
    return new npc_professor_phizzlethorpeAI(pCreature);
}

/*####
# npc_shakes_o_breen
####*/

enum
{
    SAY_BREEN_START1          = -1000501,
    SAY_BREEN_START2          = -1000502,
    SAY_BREEN_CANNON          = -1000503,
    SAY_BREEN_DONE            = -1000507,
    SAY_BREEN_GRATITUDE       = -1000508,
    SAY_DAGGERSPINE           = -1000509,

    QUEST_DEATH_FROM_BELOW     = 667,

    NPC_DAGGERSPINE_RAIDER     = 2595,
    NPC_DAGGERSPINE_SORCERESS  = 2596,
};

static float m_afNagaCoord[3][3] =
{
    { -2154.049072f, -1969.737671f, 15.371008f},
    { -2157.605713f, -1972.530151f, 15.551609f},
    { -2157.532959f, -1968.904053f, 15.410400f},
};

struct npc_shakes_o_breenAI : public npc_escortAI
{
    npc_shakes_o_breenAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiWaveId = 0;
        iSumCreaCnt = 0;
        iSumCreaDead = 0;
        Reset();
    }

    uint32 m_uiWaveId;
    uint32 m_uiEventTimer;
    int iSumCreaCnt;
    int iSumCreaDead;

    void Reset()
    {
        m_uiEventTimer = 10000;

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiWaveId = 0;
            iSumCreaCnt = 0;
            iSumCreaDead = 0;
        }
    }

    void AttackStart(Unit *) {}
    void AttackedBy(Unit *) {}

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_BREEN_START2, m_creature, pPlayer);
                break;
            case 8:
                if (Player* pPlayer = GetPlayerForEscort())
                {
                    DoScriptText(SAY_BREEN_CANNON, m_creature, pPlayer);
                    SetEscortPaused(true);
                }
                break;
            case 18:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_BREEN_GRATITUDE, m_creature, pPlayer);
                break;
        }
    }

    void DoWaveSummon()
    {
        switch (m_uiWaveId)
        {
            case 1:
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[0][0], m_afNagaCoord[0][1], m_afNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[1][0], m_afNagaCoord[1][1], m_afNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                m_creature->SummonCreature(NPC_DAGGERSPINE_SORCERESS, m_afNagaCoord[2][0], m_afNagaCoord[2][1], m_afNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                break;
            case 2:
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[0][0], m_afNagaCoord[0][1], m_afNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[1][0], m_afNagaCoord[1][1], m_afNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                break;
            case 3:
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[0][0], m_afNagaCoord[0][1], m_afNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                m_creature->SummonCreature(NPC_DAGGERSPINE_RAIDER, m_afNagaCoord[1][0], m_afNagaCoord[1][1], m_afNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                m_creature->SummonCreature(NPC_DAGGERSPINE_SORCERESS, m_afNagaCoord[2][0], m_afNagaCoord[2][1], m_afNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                break;
            case 4:
                SetEscortPaused(false);
                DoScriptText(SAY_BREEN_DONE, m_creature);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_DEATH_FROM_BELOW, m_creature);
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
        DoScriptText(SAY_DAGGERSPINE, pSummoned);
        iSumCreaCnt++;
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        iSumCreaDead++;
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiEventTimer < uiDiff)
                {
                    if (iSumCreaDead >= iSumCreaCnt)
                    {
                        ++m_uiWaveId;
                        DoWaveSummon();
                        m_uiEventTimer = 5000;
                    }
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }


    }
};

bool QuestAccept_npc_shakes_o_breen(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DEATH_FROM_BELOW)
    {
        if (npc_shakes_o_breenAI* pEscortAI = dynamic_cast<npc_shakes_o_breenAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_BREEN_START1, pCreature);
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);

            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
        }
    }

    return true;
}

CreatureAI* GetAI_npc_shakes_o_breen(Creature* pCreature)
{
    return new npc_shakes_o_breenAI(pCreature);
}

/*######
## npc_kinelory
######*/

enum
{
    SAY_START               = -1000948,
    SAY_REACH_BOTTOM        = -1000949,
    SAY_AGGRO_KINELORY      = -1000950,
    SAY_AGGRO_JORELL        = -1000951,
    SAY_WATCH_BACK          = -1000952,
    EMOTE_BELONGINGS        = -1000953,
    SAY_DATA_FOUND          = -1000954,
    SAY_ESCAPE              = -1000955,
    SAY_FINISH              = -1000956,
    EMOTE_HAND_PACK         = -1000957,

    SPELL_REJUVENATION      = 3627,
    SPELL_BEAR_FORM         = 4948,

    NPC_JORELL              = 2733,
    NPC_QUAE                = 2712,

    QUEST_HINTS_NEW_PLAGUE  = 660
};

struct npc_kineloryAI : public npc_escortAI
{
    npc_kineloryAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiBearFormTimer;
    uint32 m_uiHealTimer;

    void Reset() override
    {
        m_uiBearFormTimer = urand(5000, 7000);
        m_uiHealTimer     = urand(2000, 5000);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 9:
                DoScriptText(SAY_REACH_BOTTOM, m_creature);
                break;
            case 16:
                DoScriptText(SAY_WATCH_BACK, m_creature);
                DoScriptText(EMOTE_BELONGINGS, m_creature);
                break;
            case 17:
                DoScriptText(SAY_DATA_FOUND, m_creature);
                break;
            case 18:
                DoScriptText(SAY_ESCAPE, m_creature);
                if (Player* pPlayer = GetPlayerForEscort())
                    m_creature->SetFacingToObject(pPlayer);
                SetRun();
                break;
            case 33:
                DoScriptText(SAY_FINISH, m_creature);
                if (Creature* pQuae = GetClosestCreatureWithEntry(m_creature, NPC_QUAE, 10.0f))
                {
                    DoScriptText(EMOTE_HAND_PACK, m_creature, pQuae);
                    m_creature->SetFacingToObject(pQuae);
                }
                break;
            case 34:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_HINTS_NEW_PLAGUE, m_creature);
                break;
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (pWho->GetEntry() == NPC_JORELL)
            DoScriptText(SAY_AGGRO_JORELL, pWho, m_creature);
        else if (roll_chance_i(10))
            DoScriptText(SAY_AGGRO_KINELORY, m_creature);
    }

    void UpdateEscortAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBearFormTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BEAR_FORM) == CAST_OK)
                m_uiBearFormTimer = urand(25000, 30000);
        }
        else
            m_uiBearFormTimer -= uiDiff;

        if (m_uiHealTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() < 80.0f)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_REJUVENATION) == CAST_OK)
                    m_uiHealTimer = urand(15000, 25000);
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kinelory(Creature* pCreature)
{
    return new npc_kineloryAI(pCreature);
}

bool QuestAccept_npc_kinelory(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_HINTS_NEW_PLAGUE)
    {
        if (npc_kineloryAI* pKineloryAI = dynamic_cast<npc_kineloryAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_START, pCreature);
            pKineloryAI->Start(false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

void AddSC_arathi_highlands()
{
    Script * newscript;

    newscript = new Script;
    newscript->Name = "npc_professor_phizzlethorpe";
    newscript->GetAI = &GetAI_npc_professor_phizzlethorpe;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_professor_phizzlethorpe;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_shakes_o_breen";
    newscript->GetAI = &GetAI_npc_shakes_o_breen;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_shakes_o_breen;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kinelory";
    newscript->GetAI = &GetAI_npc_kinelory;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_kinelory;
    newscript->RegisterSelf();
}
