/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Spell_Scripts
SD%Complete: 100
SDComment: Spell scripts for dummy effects (if script need access/interaction with parts of other AI or instance, add it in related source files)
SDCategory: Spell
EndScriptData */

/* ContentData
spell 34665
spell 19512
spell 8913
spell 29528
spell 46770
spell 46023
spell 47575
spell 50706
EndContentData */

#include "scriptPCH.h"

/* When you make a spell effect:
- always check spell id and effect index
- always return true when the spell is handled by script
*/

enum
{
    // quest 6124/6129
    SPELL_APPLY_SALVE                   = 19512,
    SPELL_SICKY_CRITTER_AURA            = 19502,

    NPC_SICKLY_DEER                     = 12298,
    NPC_SICKLY_GAZELLE                  = 12296,

    NPC_CURED_DEER                      = 12299,
    NPC_CURED_GAZELLE                   = 12297,

    // target morbent fel
    SPELL_SACRED_CLEANSING              = 8913,
    NPC_MORBENT                         = 1200,
    NPC_WEAKENED_MORBENT                = 24782,

    // target blazerunner
    SPELL_BLAZERUNNER_DISPELL           = 14247,
    NPC_BLAZERUNNER                     = 9376,
    SPELL_BLAZERUNNER_AURA              = 13913
};

bool EffectDummyCreature_spell_dummy_npc(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    switch (uiSpellId)
    {
        case SPELL_BLAZERUNNER_DISPELL:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                if (pCreatureTarget->GetEntry() != NPC_BLAZERUNNER)
                    return true;

                pCreatureTarget->RemoveAurasDueToSpell(SPELL_BLAZERUNNER_AURA);
            }
            return true;
        }
        case SPELL_SACRED_CLEANSING:
        {
            if (uiEffIndex == EFFECT_INDEX_1)
            {
                if (pCreatureTarget->GetEntry() != NPC_MORBENT)
                    return true;

                pCreatureTarget->UpdateEntry(NPC_WEAKENED_MORBENT);
                return true;
            }
            return true;
        }
        case SPELL_APPLY_SALVE:
        {
            if (uiEffIndex == EFFECT_INDEX_0)
            {
                Player* playerCaster = pCaster->ToPlayer();
                if (!playerCaster)
                    return true;

                if (pCreatureTarget->GetEntry() == NPC_SICKLY_DEER && playerCaster->GetTeam() == ALLIANCE)
                {
                    pCaster->ToPlayer()->RewardPlayerAndGroupAtCast(pCreatureTarget, uiSpellId);
                    pCreatureTarget->UpdateEntry(NPC_CURED_DEER);
                    pCreatureTarget->RemoveAurasDueToSpell(SPELL_SICKY_CRITTER_AURA);
                }

                if (pCreatureTarget->GetEntry() == NPC_SICKLY_GAZELLE && playerCaster->GetTeam() == HORDE)
                {
                    pCaster->ToPlayer()->RewardPlayerAndGroupAtCast(pCreatureTarget, uiSpellId);
                    pCreatureTarget->UpdateEntry(NPC_CURED_GAZELLE);
                    pCreatureTarget->RemoveAurasDueToSpell(SPELL_SICKY_CRITTER_AURA);
                }

                return true;
            }
            return true;
        }
    }

    return false;
}

void AddSC_spell_scripts()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "spell_dummy_npc";
    newscript->pEffectDummyCreature = &EffectDummyCreature_spell_dummy_npc;
    newscript->RegisterSelf();
}
