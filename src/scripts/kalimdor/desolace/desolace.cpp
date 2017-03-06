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
SDName: Desolace
SD%Complete: 100
SDComment: Quest support: 5561, 5381
SDCategory: Desolace
EndScriptData */

/* ContentData
npc_aged_dying_ancient_kodo
go_hand_of_iruxos_crystal

EndContentData */

#include "scriptPCH.h"

enum
{
    SAY_SMEED_HOME_1                = -1000348,
    SAY_SMEED_HOME_2                = -1000349,
    SAY_SMEED_HOME_3                = -1000350,

    QUEST_KODO                      = 5561,

    NPC_SMEED                       = 11596,
    NPC_AGED_KODO                   = 4700,
    NPC_DYING_KODO                  = 4701,
    NPC_ANCIENT_KODO                = 4702,
    NPC_TAMED_KODO                  = 11627,

    SPELL_KODO_KOMBO_ITEM           = 18153,
    SPELL_KODO_KOMBO_PLAYER_BUFF    = 18172,                //spells here have unclear function, but using them at least for visual parts and checks
    SPELL_KODO_KOMBO_DESPAWN_BUFF   = 18377,
    SPELL_KODO_KOMBO_GOSSIP         = 18362

};

struct npc_aged_dying_ancient_kodoAI : public ScriptedAI
{
    npc_aged_dying_ancient_kodoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiDespawnTimer;
    ObjectGuid _playerForQuest;

    void Reset()
    {
        m_uiDespawnTimer = 0;
        _playerForQuest.Clear();
    }
    void ResetEvent()
    {
        Reset();
        m_creature->DisappearAndDie();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetObjectGuid() == _playerForQuest)
        {
            if (!who->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF))
                ResetEvent();
        }
        else if (who->GetEntry() == NPC_SMEED)
        {
            if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return;

            if (m_creature->IsWithinDistInMap(who, 10.0f))
            {
                switch (urand(0, 2))
                {
                    case 0:
                        DoScriptText(SAY_SMEED_HOME_1, who);
                        break;
                    case 1:
                        DoScriptText(SAY_SMEED_HOME_2, who);
                        break;
                    case 2:
                        DoScriptText(SAY_SMEED_HOME_3, who);
                        break;
                }

                //spell have no implemented effect (dummy), so useful to notify spellHit
                m_creature->CastSpell(m_creature, SPELL_KODO_KOMBO_GOSSIP, true);
            }
        }
    }

    void SpellHit(Unit* caster, SpellEntry const* spell)
    {
        if (spell->Id == SPELL_KODO_KOMBO_GOSSIP)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            m_uiDespawnTimer = 60000;
        }
        else if (spell->Id == SPELL_KODO_KOMBO_ITEM && !_playerForQuest)
            _playerForQuest = caster->GetObjectGuid();
    }

    void UpdateAI(const uint32 diff)
    {
        //timer should always be == 0 unless we already updated entry of creature. Then not expect this updated to ever be in combat.
        if (m_uiDespawnTimer && m_uiDespawnTimer <= diff)
        {
            if (!m_creature->getVictim() && m_creature->isAlive())
            {
                Reset();
                m_creature->SetDeathState(JUST_DIED);
                m_creature->Respawn();
                return;
            }
        }
        else m_uiDespawnTimer -= diff;

        if (m_creature->GetEntry() == NPC_TAMED_KODO)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_aged_dying_ancient_kodo(Creature* pCreature)
{
    return new npc_aged_dying_ancient_kodoAI(pCreature);
}

bool EffectDummyCreature_npc_aged_dying_ancient_kodo(Unit *caster, uint32 spellId, SpellEffectIndex effIndex, Creature *creatureTarget)
{
    //always check spellid and effectindex
    if (spellId == SPELL_KODO_KOMBO_ITEM && effIndex == EFFECT_INDEX_0)
    {
        //no effect if player/creature already have aura from spells
        if (caster->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) || creatureTarget->HasAura(SPELL_KODO_KOMBO_DESPAWN_BUFF))
            return true;

        if (creatureTarget->GetEntry() == NPC_AGED_KODO ||
                creatureTarget->GetEntry() == NPC_DYING_KODO ||
                creatureTarget->GetEntry() == NPC_ANCIENT_KODO)
        {
            caster->CastSpell(caster, SPELL_KODO_KOMBO_PLAYER_BUFF, true);

            creatureTarget->UpdateEntry(NPC_TAMED_KODO);
            creatureTarget->CastSpell(creatureTarget, SPELL_KODO_KOMBO_DESPAWN_BUFF, false);

            if (creatureTarget->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                creatureTarget->GetMotionMaster()->MoveIdle();

            creatureTarget->GetMotionMaster()->MoveFollow(caster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            creatureTarget->CombatStop(true);
        }

        //always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

bool GossipHello_npc_aged_dying_ancient_kodo(Player* player, Creature* creature)
{
    if (player->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) && creature->GetEntry() == NPC_TAMED_KODO)
    {
        ((npc_aged_dying_ancient_kodoAI*)creature->AI())->_playerForQuest.Clear();
        //the expected quest objective
        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());

        player->RemoveAurasDueToSpell(SPELL_KODO_KOMBO_PLAYER_BUFF);
        creature->GetMotionMaster()->MoveIdle();
    }

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
    return true;
}

/*######
## go_hand_of_iruxos_crystal
######*/

enum
{
    DEMON_SPIRIT  = 11876
};

bool GOHello_go_hand_of_iruxos_crystal(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
    {
        if (Creature* pCreature = pPlayer->SummonCreature(DEMON_SPIRIT, -346.84f, 1765.13f, 138.39f, 5.91f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
            pCreature->AI()->AttackStart(pPlayer);
    }

    return true;
}
/*######
## npc_melizza_brimbuzzle
######*/

enum
{
    QUEST_GET_ME_OUT_OF_HERE    = 6132,

    GO_MELIZZAS_CAGE            = 177706,

    SAY_MELIZZA_START           = -1000784,
    SAY_MELIZZA_FINISH          = -1000785,
    SAY_MELIZZA_1               = -1000786,
    SAY_MELIZZA_2               = -1000787,
    SAY_MELIZZA_3               = -1000788,

    NPC_MARAUDINE_MARAUDER      = 4659,
    NPC_MARAUDINE_BONEPAW       = 4660,
    NPC_MARAUDINE_WRANGLER      = 4655,
    NPC_HORNIZ_BRIMBUZZLE       = 6019,

    NPC_MELIZZA                 = 12277,

    POINT_ID_QUEST_COMPLETE     = 1,
    POINT_ID_EVENT_COMPLETE     = 2,

    MAX_MARAUDERS               = 2,
    MAX_WRANGLERS               = 3,
};

struct SummonLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SummonLocation aMarauderSpawn[] =
{
    { -1291.492f, 2644.650f, 111.556f},
    { -1306.730f, 2675.163f, 111.561f},
};

static const SummonLocation wranglerSpawn = { -1393.194f, 2429.465f, 88.689f };

struct npc_melizza_brimbuzzleAI : public npc_escortAI
{
    npc_melizza_brimbuzzleAI(Creature* m_creature) : npc_escortAI(m_creature)
    {
        Reset();
    }

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_dialogueStep = 20;
            m_dialogueTimer = 0;
        }
    }

    void JustStartedEscort() override
    {
        m_dialogueStep = 20;
        if (GameObject* pCage = GetClosestGameObjectWithEntry(m_creature, GO_MELIZZAS_CAGE, INTERACTION_DISTANCE))
            pCage->UseDoorOrButton();
    }
    uint16 m_dialogueStep;
    uint32 m_dialogueTimer;

    void WaypointReached(uint32 uiPointId) override
    {
        switch (uiPointId)
        {
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_MELIZZA_START, m_creature, pPlayer);

                m_creature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
                break;
            case 4:
                for (uint8 i = 0; i < MAX_MARAUDERS; ++i)
                {
                    for (uint8 j = 0; j < MAX_MARAUDERS; ++j)
                    {
                        // Summon 2 Marauders on each point
                        float fX, fY, fZ;
                        m_creature->GetRandomPoint(aMarauderSpawn[i].m_fX, aMarauderSpawn[i].m_fY, aMarauderSpawn[i].m_fZ, 7.0f, fX, fY, fZ);
                        m_creature->SummonCreature(NPC_MARAUDINE_MARAUDER, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                    }
                }
                break;
            case 9:
                for (uint8 i = 0; i < MAX_WRANGLERS; ++i)
                {
                    float fX, fY, fZ;
                    m_creature->GetRandomPoint(wranglerSpawn.m_fX, wranglerSpawn.m_fY, wranglerSpawn.m_fZ, 10.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_MARAUDINE_BONEPAW, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);

                    m_creature->GetRandomPoint(wranglerSpawn.m_fX, wranglerSpawn.m_fY, wranglerSpawn.m_fZ, 10.0f, fX, fY, fZ);
                    m_creature->SummonCreature(NPC_MARAUDINE_WRANGLER, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                }
                break;
            case 12:
                m_dialogueStep = 6;
                SetEscortPaused(true);
                SetMaxPlayerDistance(100); //Let's not have her despawn so easily.
                if (Player* player = GetPlayerForEscort())
                    m_creature->SetFacingToObject(player);
                m_dialogueTimer = 200;
                break;
            case 19:
                m_dialogueStep = 0;
                SetEscortPaused(true);
                break;
        }
    }
    void Dialogue(const uint32 uiDiff)
    {
        if (m_dialogueStep > 6)
            return;
        if (m_dialogueTimer < uiDiff)
        {
            //sLog.nostalrius("Melizza: DialogueStep n°%u",m_dialogueStep);
            switch (m_dialogueStep)
            {
                case 0:
                    if (Creature* pHorniz = m_creature->FindNearestCreature(NPC_HORNIZ_BRIMBUZZLE, 30.0f))
                        m_creature->SetFacingToObject(pHorniz);
                    DoScriptText(SAY_MELIZZA_1, m_creature);
                    m_dialogueTimer = 4000;
                    m_dialogueStep++;
                    break;
                case 1:
                    DoScriptText(SAY_MELIZZA_2, m_creature);
                    m_dialogueTimer = 5000;
                    m_dialogueStep++;
                    break;
                case 2:
                    DoScriptText(SAY_MELIZZA_3, m_creature);
                    m_dialogueTimer = 4000;
                    m_dialogueStep++;
                    break;
                case 3:
                    SetEscortPaused(false);
                    m_dialogueTimer = 0;
                    m_dialogueStep++;
                    break;
                case 6:
                    if (Player* pPlayer = GetPlayerForEscort())
                    {
                        DoScriptText(SAY_MELIZZA_FINISH, m_creature, pPlayer);
                        pPlayer->GroupEventHappens(QUEST_GET_ME_OUT_OF_HERE, m_creature);
                    }
                    m_dialogueTimer = 2000;
                    m_dialogueStep++;
                    m_creature->ClearTemporaryFaction();
                    SetRun(true);
                    SetEscortPaused(false);
                    break;
            }
        }
        else
            m_dialogueTimer  -= uiDiff;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        Dialogue(uiDiff);
        npc_escortAI::UpdateAI(uiDiff);
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_melizza_brimbuzzle(Creature* pCreature)
{
    return new npc_melizza_brimbuzzleAI(pCreature);
}

bool QuestAccept_npc_melizza_brimbuzzle(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_GET_ME_OUT_OF_HERE)
    {
        if (npc_melizza_brimbuzzleAI* pEscortAI = dynamic_cast<npc_melizza_brimbuzzleAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
    }

    return true;
}

/*######
## npc_dalinda_malem
######*/

enum
{
    QUEST_RETURN_TO_VAHLARRIEL  = 1440,
};

struct npc_dalinda_malemAI : public npc_escortAI
{
    npc_dalinda_malemAI(Creature* m_creature) : npc_escortAI(m_creature)
    {
        Reset();
    }

    void Reset() override {}

    void JustStartedEscort() override
    {
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void WaypointReached(uint32 uiPointId) override
    {
        if (uiPointId == 18)
        {
            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_RETURN_TO_VAHLARRIEL, m_creature);
        }
    }
};

CreatureAI* GetAI_npc_dalinda_malem(Creature* pCreature)
{
    return new npc_dalinda_malemAI(pCreature);
}

bool QuestAccept_npc_dalinda_malem(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RETURN_TO_VAHLARRIEL)
    {
        if (npc_dalinda_malemAI* pEscortAI = dynamic_cast<npc_dalinda_malemAI*>(pCreature->AI()))
        {
            // TODO This faction change needs confirmation, also possible that we need to drop her PASSIVE flag
            pCreature->SetFactionTemporary(FACTION_ESCORT_A_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);//TEMPFACTION_RESTORE_RESPAWN | TEMPFACTION_TOGGLE_PASSIVE
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}
enum
{
//guid 12609 entry 177673 Serpant statue
    NPC_LORD_KRAGARU            = 12369,
    QUEST_BOOK_OF_THE_ANCIENTS  = 6027

};
struct go_serpent_statueAI: public GameObjectAI
{
    go_serpent_statueAI(GameObject* pGo) : GameObjectAI(pGo)
    {
        timer = 0;
        state = 0;
        guid_kragaru = 0;
    }
    uint64 guid_kragaru;
    uint32 timer;
    bool state;//0 = usual, can launch. //1 = in use, cannot launch

    void UpdateAI(const uint32 uiDiff)
    {
        if (state)
        {
            if (timer < uiDiff)
            {
                state = 0;
                me->SetGoState(GO_STATE_READY);
                me->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
            }
            else
                timer -= uiDiff;
        }
    }
    bool CheckCanStartEvent()
    {
        if (!state && !me->GetMap()->GetCreature(guid_kragaru))
            return true;
        return false;
    }

    void SetInUse(Creature* kragaru)
    {
        guid_kragaru = kragaru->GetGUID();
        me->SetGoState(GO_STATE_ACTIVE);
        me->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
        state = 1;
        timer = 120000;
    }
};
GameObjectAI* GetAIgo_serpent_statue(GameObject *pGo)
{
    return new go_serpent_statueAI(pGo);
}
bool GOHello_go_serpent_statue(Player* pPlayer, GameObject* pGo)
{
    if (go_serpent_statueAI* pMarkAI = dynamic_cast<go_serpent_statueAI*>(pGo->AI()))
    {
        if (pMarkAI->CheckCanStartEvent())
        {
            if (pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
            {
                if (pPlayer->GetQuestStatus(QUEST_BOOK_OF_THE_ANCIENTS) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Creature* kragaru = pGo->SummonCreature(NPC_LORD_KRAGARU, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 310000))
                    {
                        kragaru->SetRespawnDelay(350000);
                        pMarkAI->SetInUse(kragaru);
                    }
                }
            }
        }
    }
    return true;
}
enum
{
    NPC_MAGRAMI_SPECTRE                 = 11560,
    GO_GHOST_MAGNET                     = 177746,
    GO_GHOST_MAGNET_AURA                = 177749,
    SPELL_CURSE_OF_THE_FALLEN_MAGRAM    = 18159,
    SPELL_PLACE_GHOST_MAGNET            = 19588,
    SPELL_DESTROY_MAGNET                = 19571,//not used maybe.
    SPELL_BLUE_AURA                     = 17327,//neutral, walk to target
    SPELL_GREEN_AURA                    = 18951, //aura becomes green once they get to magnet. and aggressive
    FACTION_NEUTRAL                     = 634, //or 58
    FACTION_ENNEMY                      = 16
};

void DefineMagramiMagnet(Creature * crea, uint64 gobjGUID);

struct go_ghost_magnetAI: public GameObjectAI
{
    go_ghost_magnetAI(GameObject* pGo) : GameObjectAI(pGo)
    {
        bigTimer = 120000;
        timer = 5000;
        nbToSpawn = 8;

        state = 1;
        std::list<GameObject*> lGobj;
        me->GetGameObjectListWithEntryInGrid(lGobj, GO_GHOST_MAGNET_AURA, 30.0f);
        for (std::list<GameObject*>::iterator it = lGobj.begin(); it != lGobj.end(); ++it)
        {
            if ((*it)->isSpawned())
            {
                state = 0;
                break;
            }
        }
        if (state)
        {
            float mx, my, mz;
            me->GetPosition(mx, my, mz);
            me->SummonGameObject(GO_GHOST_MAGNET_AURA, mx, my, mz, 0, 0, 0, 0, 0, 120);//120sec to go with the 12000 spectre spawn
        }
    }
    uint32 timer;
    uint32 bigTimer;
    uint16 nbToSpawn;
    bool state;//0 = already are functioning magnets, do not spawn spectre. //1 = spawning.

    void UpdateAI(const uint32 uiDiff)
    {
        if (state)
        {
            if (bigTimer < uiDiff)
            {
                state=0;
            }
            else
                bigTimer -= uiDiff;
            if(nbToSpawn>0)
            {
                if (timer < uiDiff)
                {
                    spawnSpetre();
                    timer = urand(3000, 8000);
                    --nbToSpawn;
                }
                else
                    timer -= uiDiff;
            }
        }
    }

    void spawnSpetre()
    {
        float x, y, z;
        float mx, my, mz;
        me->GetPosition(mx, my, mz);
        me->GetRandomPoint(mx,my,mz, 40, x, y, z);
        if(Creature* spectre=me->SummonCreature(NPC_MAGRAMI_SPECTRE, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000))
        {
            spectre->SetRespawnDelay(425000);
            DefineMagramiMagnet(spectre, me->GetGUID());
        }
    }
    void MagramiSpectreDied(uint64 guid)
    {
        if (state)
            spawnSpetre();
    }
};
GameObjectAI* GetAIgo_ghost_magnet(GameObject *pGo)
{
    return new go_ghost_magnetAI(pGo);
}
struct npc_magrami_spetreAI : public ScriptedAI
{
    npc_magrami_spetreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        isGreen = false;
        guidMagnet = 0;
        corpseTimer = 20000;
        Reset();
    }
    void Reset()
    {
        timer=0;
        curseTimer = urand(5000, 9000);
        if(isGreen)
             m_creature->AddAura(SPELL_GREEN_AURA);
        else
             m_creature->AddAura(SPELL_BLUE_AURA);
    }
    uint32 corpseTimer;
    uint32 timer;
    uint32 curseTimer;
    uint64 guidMagnet;
    bool isGreen;

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(isGreen)
            return;
        if (uiType != POINT_MOTION_TYPE || uiPointId!=2)
            return;
        turnGreen();
    }
    void JustReachedHome()
    {
        if(!isGreen)
            turnGreen();
    }
    void turnGreen()
    {
        m_creature->RemoveAurasDueToSpell(SPELL_BLUE_AURA);
        m_creature->AddAura(SPELL_GREEN_AURA);
        m_creature->setFaction(FACTION_ENNEMY);
        isGreen=true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if ( curseTimer < uiDiff)
        {
            if(m_creature->getVictim()->HasAura(SPELL_CURSE_OF_THE_FALLEN_MAGRAM))
                curseTimer = 5000;
            else
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_THE_FALLEN_MAGRAM) == CAST_OK)
                    curseTimer = urand(15000, 21000);
            }
        }
        else
            curseTimer -= uiDiff;
        DoMeleeAttackIfReady();
    }
    void UpdateAI_corpse(const uint32 uiDiff)
    {
        if ( corpseTimer < uiDiff)//purpose is to delay the new spawn.
        {
            if(GameObject* gobj=m_creature->GetMap()->GetGameObject(guidMagnet))
            {
                if (go_ghost_magnetAI* pMoundAI = dynamic_cast<go_ghost_magnetAI*>(gobj->AI()))
                {
                    pMoundAI->MagramiSpectreDied(m_creature->GetGUID());
                }
            }
            corpseTimer=150000;//after despawn.
        }
        else
            corpseTimer -= uiDiff;
    }
    void SetMagnetGuid(uint64 ghostMagnetGuid)
    {
        if (GameObject* gobj = m_creature->GetMap()->GetGameObject(ghostMagnetGuid))
        {
            float x,y,z;

            gobj->GetContactPoint(m_creature, x, y, z, 1);
            m_creature->SetHomePosition(x, y, z, 0);
            m_creature->GetMotionMaster()->MovePoint(2, x, y, z, MOVE_PATHFINDING);
            guidMagnet=ghostMagnetGuid;
        }
    }
};

CreatureAI* GetAI_npc_magrami_spetre(Creature* pCreature)
{
    return new npc_magrami_spetreAI(pCreature);
}
void DefineMagramiMagnet(Creature * crea, uint64 gobjGUID)
{
    if (npc_magrami_spetreAI* spectreAI = dynamic_cast<npc_magrami_spetreAI*>(crea->AI()))
    {
        spectreAI->SetMagnetGuid(gobjGUID);
    }
}

enum
{
    NPC_PORTAL_DEMON_DEMON  = 11937,
};

struct go_demon_portalAI: public GameObjectAI
{
    go_demon_portalAI(GameObject* pGo) : GameObjectAI(pGo)
    {
    }
    ObjectGuid _demonGuid;
    bool OnUse(Unit* caster)
    {
        Unit* demon;
        if (_demonGuid)
        {
            demon = me->GetMap()->GetCreature(_demonGuid);
            if (demon && demon->isAlive())
                return true;
        }
        demon = me->SummonCreature(NPC_PORTAL_DEMON_DEMON, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), caster ? me->GetAngle(caster) : 0.0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        if (demon)
        {
            _demonGuid = demon->GetObjectGuid();
            if (caster)
                demon->AI()->AttackStart(caster);
        }
        return true;
    }
};

GameObjectAI* GetAI_go_demon_portal(GameObject *go)
{
    return new go_demon_portalAI(go);
}


void AddSC_desolace()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_aged_dying_ancient_kodo";
    newscript->GetAI = &GetAI_npc_aged_dying_ancient_kodo;
    newscript->pEffectDummyCreature = &EffectDummyCreature_npc_aged_dying_ancient_kodo;
    newscript->pGossipHello = &GossipHello_npc_aged_dying_ancient_kodo;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_hand_of_iruxos_crystal";
    newscript->pGOHello = &GOHello_go_hand_of_iruxos_crystal;
    newscript->RegisterSelf();


    newscript = new Script;
    newscript->Name = "npc_melizza_brimbuzzle";
    newscript->GetAI = &GetAI_npc_melizza_brimbuzzle;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_melizza_brimbuzzle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dalinda_malem";
    newscript->GetAI = &GetAI_npc_dalinda_malem;
    newscript->pQuestAcceptNPC = &QuestAccept_npc_dalinda_malem;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_serpent_statue";
    newscript->GOGetAI = &GetAIgo_serpent_statue;
    newscript->pGOHello = &GOHello_go_serpent_statue;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_ghost_magnet";
    newscript->GOGetAI = &GetAIgo_ghost_magnet;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_magrami_spetre";
    newscript->GetAI = &GetAI_npc_magrami_spetre;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_demon_portal";
    newscript->GOGetAI = &GetAI_go_demon_portal;
    newscript->RegisterSelf();
}
