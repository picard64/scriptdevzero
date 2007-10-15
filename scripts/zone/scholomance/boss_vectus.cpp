/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "../../sc_defines.h"

#define SPELL_FIRESHIELD                19626           
#define SPELL_BLASTWAVE            13021
#define SPELL_FRENZY                28371



struct MANGOS_DLL_DECL boss_vectusAI : public ScriptedAI
{
    boss_vectusAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 FireShield_Timer;
    uint32 BlastWave_Timer;
    uint32 Frenzy_Timer;
    bool InCombat;

    void EnterEvadeMode()
    {       
        FireShield_Timer = 2000;
        BlastWave_Timer = 14000;
        Frenzy_Timer = 0;
        InCombat = false;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
            DoStartMeleeAttack(who);
            InCombat = true;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && m_creature->IsWithinLOSInMap(who))
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                DoStartMeleeAttack(who);
                InCombat = true;

            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //FireShield_Timer
        if (FireShield_Timer < diff)
        {
            //Cast
            DoCast(m_creature, SPELL_FIRESHIELD);

            //90 seconds
            FireShield_Timer = 90000;
        }else FireShield_Timer -= diff;

        //BlastWave_Timer
        if (BlastWave_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_BLASTWAVE);

            //12 seconds until we should cast this agian
            BlastWave_Timer = 12000;
        }else BlastWave_Timer -= diff;

        //Frenzy_Timer
        if ( m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 25 )
        {
            if (Frenzy_Timer < diff)
            {
                DoCast(m_creature,SPELL_FRENZY);
                DoTextEmote("goes into a killing frenzy!",NULL);

                //24 seconds
                Frenzy_Timer = 24000;
            }else Frenzy_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
}; 
CreatureAI* GetAI_boss_vectus(Creature *_Creature)
{
    return new boss_vectusAI (_Creature);
}


void AddSC_boss_vectus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_vectus";
    newscript->GetAI = GetAI_boss_vectus;
    m_scripts[nrscripts++] = newscript;
}