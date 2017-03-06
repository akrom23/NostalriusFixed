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
SDName: Boss_Herod
SD%Complete: 95
SDComment: Should in addition spawn Myrmidons in the hallway outside
SDCategory: Scarlet Monastery
EndScriptData */

#include "scriptPCH.h"

#define SAY_AGGRO                   -1189000
#define SAY_WHIRLWIND               -1189001
#define SAY_ENRAGE                  -1189002
#define SAY_KILL                    -1189003
#define EMOTE_ENRAGE                -1189004

#define SAY_TRAINEE_SPAWN           -1189022

#define SPELL_RUSHINGCHARGE         8260
#define SPELL_CLEAVE                15496
#define SPELL_WHIRLWIND             8989
#define SPELL_FRENZY                8269

#define ENTRY_SCARLET_TRAINEE       6575
#define ENTRY_SCARLET_MYRMIDON      4295

struct boss_herodAI : public ScriptedAI
{
    boss_herodAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool Enrage;
    bool CanDoCharge;
    bool TraineeSay;
    uint8 NbTrainee;
    uint32 RushingCharge_Timer;
    uint32 Cleave_Timer;
    uint32 Whirlwind_Timer;

    void Reset()
    {
        Enrage = false;
        CanDoCharge = true;
        TraineeSay = false;
        NbTrainee = 0;
        RushingCharge_Timer = 1500;
        Cleave_Timer = 12000;
        Whirlwind_Timer = urand(10000, 20000);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        // make first Scarlet Trainee say text
        if (pSummoned->GetEntry() == ENTRY_SCARLET_TRAINEE && !TraineeSay)
        {
            DoScriptText(SAY_TRAINEE_SPAWN, pSummoned);
            TraineeSay = true;
        }

        // séparation des mobs en deux groupes de 10 pour occuper les deux escaliers
        if (NbTrainee < 10)
            pSummoned->GetMotionMaster()->MovePoint(0, 1940.257080f, -434.454315f, 17.094456f);
        else
            pSummoned->GetMotionMaster()->MovePoint(100, 1940.508301f, -428.826080f, 17.095098f);

        NbTrainee++;
    }

    void JustDied(Unit* killer)
    {
        for (uint8 i = 0; i < 20; ++i)
            m_creature->SummonCreature(ENTRY_SCARLET_TRAINEE, 1939.18f, -431.58f, 17.09f, 6.22f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //If we are <50% hp goes Enraged
        if (!Enrage && m_creature->GetHealthPercent() <= 50.0f && !m_creature->IsNonMeleeSpellCasted(false))
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
            {
                DoScriptText(EMOTE_ENRAGE, m_creature);
                DoScriptText(SAY_ENRAGE, m_creature);
                Enrage = true;
            }
        }

        //RushingCharge_Timer
        if (CanDoCharge)
        {
            if (RushingCharge_Timer < diff)
            {
                DoCastSpellIfCan(m_creature, SPELL_RUSHINGCHARGE);
                RushingCharge_Timer = 11000;
                CanDoCharge = false;
            }
            else RushingCharge_Timer -= diff;
        }

        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            Cleave_Timer = 12000;
        }
        else Cleave_Timer -= diff;

        if (Whirlwind_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND) == CAST_OK)
            {
                DoScriptText(SAY_WHIRLWIND, m_creature);
                Whirlwind_Timer = 30000;
                CanDoCharge = true;
            }
        }
        else Whirlwind_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_herod(Creature* pCreature)
{
    return new boss_herodAI(pCreature);
}

struct mob_scarlet_traineeAI : public ScriptedAI
{
    mob_scarlet_traineeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Start_Timer = urand(1000, 6000);
        group1 = false;
        group2 = false;
        Reset();
    }

    uint32 Start_Timer;
    bool group1;
    bool group2;

    void Reset() { }

    void UpdateAI(const uint32 diff)
    {
        if (Start_Timer)
        {
            if (Start_Timer <= diff)
            {
                m_creature->SetSpeedRate(MOVE_WALK, 2.20f, true);

                if (group1)
                    m_creature->GetMotionMaster()->MovePoint(1, 1946.433594f, -435.955109f, 16.367277f);
                else if (group2)
                    m_creature->GetMotionMaster()->MovePoint(101, 1940.257080f, -434.454315f, 17.094456f);

                Start_Timer = 0;
            }
            else Start_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void MovementInform(uint32 MovementType, uint32 id)
    {
        if (MovementType == POINT_MOTION_TYPE)
        {
            switch (id)
            {
                case 0:
                    group1 = true;
                    break;
                case 100:
                    group2 = true;
                    break;
                case 1:
                    m_creature->GetMotionMaster()->MovePoint(2, 1952.834717f, -447.514130f, 13.804327f);
                    break;
                case 101:
                    m_creature->GetMotionMaster()->MovePoint(102, 1953.056763f, -416.109863f, 13.861217f);
                    break;
                case 2:
                    m_creature->GetMotionMaster()->MovePoint(3, 1965.592041f, -451.153778f, 11.272284f);
                    break;
                case 102:
                    m_creature->GetMotionMaster()->MovePoint(103, 1965.369629f, -412.147949f, 11.272387f);
                    break;
                case 3:
                    m_creature->GetMotionMaster()->MovePoint(4, 1982.692749f, -441.514343f, 11.272284f);
                    break;
                case 103:
                    m_creature->GetMotionMaster()->MovePoint(104, 1980.908081f, -421.008026f, 11.272387f);
                    break;
                case 4:
                    m_creature->GetMotionMaster()->MovePoint(5, 1978.061890f, -428.549500f, 11.272232f);
                    break;
                case 104:
                    m_creature->GetMotionMaster()->MovePoint(105, 1979.139038f, -434.856934f, 11.272370f);
                    break;
                case 5:
                    m_creature->GetMotionMaster()->MovePoint(6, 1971.447144f, -419.629272f, 8.087179f);
                    break;
                case 105:
                    m_creature->GetMotionMaster()->MovePoint(106, 1972.044800f, -442.568573f, 8.434578f);
                    break;
                case 6:
                    m_creature->GetMotionMaster()->MovePoint(7, 1964.354004f, -418.632904f, 6.177466f);
                    break;
                case 106:
                    m_creature->GetMotionMaster()->MovePoint(107, 1964.691162f, -444.223022f, 6.177622f);
                    break;
                case 7:
                case 107:
                    m_creature->GetMotionMaster()->MovePoint(116, 1965.039795f, -431.733856f, 6.177539f);
                    break;
            }
        }
    }
};

CreatureAI* GetAI_mob_scarlet_trainee(Creature* pCreature)
{
    return new mob_scarlet_traineeAI(pCreature);
}

void AddSC_boss_herod()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_herod";
    newscript->GetAI = &GetAI_boss_herod;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_trainee";
    newscript->GetAI = &GetAI_mob_scarlet_trainee;
    newscript->RegisterSelf();
}
