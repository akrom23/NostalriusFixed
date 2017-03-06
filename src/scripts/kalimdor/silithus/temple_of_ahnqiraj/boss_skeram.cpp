/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 *
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
SDName: Boss_Skeram
SD%Complete: 75
SDComment: Mind Control buggy.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "scriptPCH.h"
#include "temple_of_ahnqiraj.h"

#define SAY_AGGRO                   -1531000
#define SAY_SLAY                    -1531003
#define SAY_DEATH                   -1531007

#define SPELL_ARCANE_EXPLOSION      26192
#define SPELL_EARTH_SHOCK           26194
#define SPELL_TRUE_FULFILLMENT      785
#define SPELL_BLINK_1               4804
#define SPELL_BLINK_2				8195
#define SPELL_BLINK_3				20449

class ov_mycoordinates
{
public:
    float x, y, z, r;
    ov_mycoordinates(float cx, float cy, float cz, float cr)
    {
        x = cx;
        y = cy;
        z = cz;
        r = cr;
    }
};

struct boss_skeramAI : public ScriptedAI
{
    boss_skeramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        IsImage = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 ArcaneExplosion_Timer;
    uint32 EarthShock_Timer;
    uint32 FullFillment_Timer;
    uint32 Blink_Timer;
    uint32 Invisible_Timer;
	//ObjectGuid m_MCTargetGUID;

    Creature *Image1, *Image2;

    bool Images75;
    bool Images50;
    bool Images25;
    bool IsImage;
    bool Invisible;

    void Reset()
    {
        ArcaneExplosion_Timer = urand(6000, 8000);
        EarthShock_Timer = 1000;
        FullFillment_Timer = urand(10000, 15000);
        Blink_Timer = urand(10000, 18000);
        Invisible_Timer = 500;

        Images75 = false;
        Images50 = false;
        Images25 = false;
        Invisible = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_ON);

        if (IsImage)
            m_creature->SetDeathState(JUST_DIED);
    }

    void KilledUnit(Unit* victim)
    {
		DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        if (!IsImage)
        {
            DoScriptText(SAY_DEATH, m_creature);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_SKERAM, DONE);
        }
    }

    void Aggro(Unit *who)
    {
		if (IsImage || Images75)
		{
			m_creature->SetInCombatWithZone();
			return;
		}

		DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKERAM, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKERAM, FAIL);
    }
	/*
	void RemoveMC()
	{
		target->SetCharmerGuid(ObjectGuid());
		if (target->GetTypeId() == TYPEID_PLAYER)
			((Player*)target)->setFactionForRace(target->getRace());
		m_creature->SetCharm(nullptr);
		target->CombatStop(true);
		target->DeleteThreatList();
		target->getHostileRefManager().deleteReferences();
	}
	*/

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCANE_EXPLOSION) == CAST_OK)
                ArcaneExplosion_Timer = urand(6000, 14000);
        }
        else ArcaneExplosion_Timer -= diff;

        //If we are within range melee the target
        if (m_creature->IsWithinMeleeRange(m_creature->getVictim()))
        {
            //Make sure our attack is ready and we arn't currently casting
            if (m_creature->isAttackReady() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->AttackerStateUpdate(m_creature->getVictim());
                m_creature->resetAttackTimer();
            }
        }
        else
        {
            //EarthShock_Timer
            if (EarthShock_Timer < diff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_EARTH_SHOCK) == CAST_OK)
                    EarthShock_Timer = 1000;
            }
            else EarthShock_Timer -= diff;
        }

        if (FullFillment_Timer < diff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && pTarget != m_creature->getVictim())
            {
                if (DoCastSpellIfCan(pTarget, SPELL_TRUE_FULFILLMENT, CAST_AURA_NOT_PRESENT) == CAST_OK)
                {
                    //pTarget->CastSpell(pTarget, 2313, true);		// remove until we can fix players retaining instant cast spells on true fulfillment early cancel
                    //pTarget->CastSpell(pTarget, 26525, true);
                    pTarget->CastSpell(pTarget, 26526, true);
					//m_MCTargetGUID = pTarget->GetObjectGuid();
                    FullFillment_Timer = urand(20000, 30000);
                }
            }
        }
        else FullFillment_Timer -= diff;

        //Blink_Timer
        if (Blink_Timer < diff)
        {
            switch (urand(0, 2))
            {
                case 0:
					DoCastSpellIfCan(m_creature, SPELL_BLINK_1);
                    DoResetThreat();
                    break;
                case 1:
                    DoCastSpellIfCan(m_creature, SPELL_BLINK_2);
                    DoResetThreat();
                    break;
                case 2:
                    DoCastSpellIfCan(m_creature, SPELL_BLINK_3);
                    DoResetThreat();
                    break;
            }
			
			//EarthShock_Timer = 1000;
			DoStopAttack();

            Blink_Timer = urand(1000, 18000);
        }
        else Blink_Timer -= diff;

        float procent = m_creature->GetHealthPercent();

        //Summoning 2 Images and teleporting to a random position on 75% health
        if (!Images75 && !IsImage && procent <= 75.0f && procent > 70.0f)
            DoSplit(75);

        //Summoning 2 Images and teleporting to a random position on 50% health
        if (!Images50 && !IsImage && procent <= 50.0f && procent > 45.0f)
            DoSplit(50);

        //Summoning 2 Images and teleporting to a random position on 25% health
        if (!Images25 && !IsImage && procent <= 25.0f && procent > 20.0f)
            DoSplit(25);

        //Invisible_Timer
        if (Invisible)
        {
            if (Invisible_Timer < diff)
            {
                //Making Skeram visible after splitting
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                Invisible_Timer = 500;
                Invisible = false;
            }
            else Invisible_Timer -= diff;
        } 
		else 
		{
			DoMeleeAttackIfReady();
		}
    }

    void DoSplit(int atPercent /* 75 50 25 */)
    {
        ov_mycoordinates *place1 = new ov_mycoordinates(-8340.782227f, 2083.814453f, 125.648788f, 0.0f);
        ov_mycoordinates *place2 = new ov_mycoordinates(-8341.546875f, 2118.504639f, 133.058151f, 0.0f);
        ov_mycoordinates *place3 = new ov_mycoordinates(-8318.822266f, 2058.231201f, 133.058151f, 0.0f);

        ov_mycoordinates *bossc = place1, *i1 = place2, *i2 = place3;

        switch (urand(0, 2))
        {
            case 0:
                bossc = place1;
                i1 = place2;
                i2 = place3;
                break;
            case 1:
                bossc = place2;
                i1 = place1;
                i2 = place3;
                break;
            case 2:
                bossc = place3;
                i1 = place1;
                i2 = place2;
                break;
        }

        m_creature->RemoveAllAuras();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetVisibility(VISIBILITY_OFF);

        m_creature->GetMap()->CreatureRelocation(m_creature, bossc->x, bossc->y, bossc->z, bossc->r);

        Invisible = true;
        DoResetThreat();
        DoStopAttack();

        switch (atPercent)
        {
            case 75:
                Images75 = true;
                break;
            case 50:
                Images50 = true;
                break;
            case 25:
                Images25 = true;
                break;
        }

        Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

        Image1 = m_creature->SummonCreature(15263, i1->x, i1->y, i1->z, i1->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image1)
        {
            Image1->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image1->SetHealth(m_creature->GetHealth() / 5);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image1->AI()))
                pImageAI->IsImage = true;

            if (target)
                Image1->AI()->AttackStart(target);
        }

        Image2 = m_creature->SummonCreature(15263, i2->x, i2->y, i2->z, i2->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (Image2)
        {
            Image2->SetMaxHealth(m_creature->GetMaxHealth() / 5);
            Image2->SetHealth(m_creature->GetHealth() / 5);

            if (boss_skeramAI* pImageAI = dynamic_cast<boss_skeramAI*>(Image2->AI()))
                pImageAI->IsImage = true;

            if (target)
                Image2->AI()->AttackStart(target);
        }
		
		ArcaneExplosion_Timer = urand(1000, 10000);
        EarthShock_Timer = 1000;
        FullFillment_Timer = urand(1000, 12000);
		if (IsImage)
		{
			Blink_Timer = urand(14000, 30000);
		} else {
			Blink_Timer = urand(1000, 12000);
		}

        Invisible = true;
        delete place1;
        delete place2;
        delete place3;
    }
};

CreatureAI* GetAI_boss_skeram(Creature* pCreature)
{
    return new boss_skeramAI(pCreature);
}

void AddSC_boss_skeram()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_skeram";
    newscript->GetAI = &GetAI_boss_skeram;
    newscript->RegisterSelf();
}
