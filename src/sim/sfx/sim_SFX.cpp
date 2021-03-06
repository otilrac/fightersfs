/****************************************************************************//*
 * Copyright (C) 2020 Marek M. Cel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 ******************************************************************************/

#include <sim/sfx/sim_SFX.h>

#ifdef SIM_SFX_OPENSLES
#   include <sim/sfx/sim_EngineOpenSLES.h>
#else
#   include <sim/sfx/sim_EngineOpenAL.h>
#endif

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

SFX::SFX() :
    _inited ( false ),
    _paused ( false ),
    _rpm ( 0 ),
    _destroyed ( false ),
    _ownship_hits ( 0 ),
    _hit_time ( 0.0f )
{
    _rpm = new Inertia< float >( 0.5f, 0.85f );
}

////////////////////////////////////////////////////////////////////////////////

SFX::~SFX()
{
    stop();

    DELPTR( _rpm );
}

////////////////////////////////////////////////////////////////////////////////

void SFX::init()
{
    if ( !_inited )
    {
#       ifdef SIM_SFX_OPENSLES
        EngineOpenSLES::instance()->init();
#       else
        EngineOpenAL::instance()->init();
#       endif

        _inited = true;
        _paused = true;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SFX::pause()
{
    _paused = true;

#   ifdef SIM_SFX_OPENSLES
    EngineOpenSLES::instance()->setPlayerCrash     ( false );
    EngineOpenSLES::instance()->setPlayerEngine    ( false );
    EngineOpenSLES::instance()->setPlayerGunfire   ( false );
    EngineOpenSLES::instance()->setPlayerHeartbeat ( false );
    EngineOpenSLES::instance()->setPlayerHit       ( false );
    EngineOpenSLES::instance()->setPlayerWaypoint  ( false );
#   else
    EngineOpenAL::instance()->setCrash     ( false );
    EngineOpenAL::instance()->setEngine    ( false );
    EngineOpenAL::instance()->setGunfire   ( false );
    EngineOpenAL::instance()->setHeartbeat ( false );
    EngineOpenAL::instance()->setHit       ( false );
    EngineOpenAL::instance()->setWaypoint  ( false );
#   endif
}

////////////////////////////////////////////////////////////////////////////////

void SFX::unpause()
{
    _paused = false;
}

////////////////////////////////////////////////////////////////////////////////

void SFX::stop()
{
    if ( _inited )
    {
#       ifdef SIM_SFX_OPENSLES
        EngineOpenSLES::instance()->stop();
#       else
        EngineOpenAL::instance()->stop();
#       endif

        _inited = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void SFX::update()
{
    _rpm->update( 0.016f, 0.7f + 0.3f * Data::get()->controls.throttle );

#   ifdef SIM_SFX_OPENSLES
    if ( !_paused )
    {
        if ( !Data::get()->ownship.destroyed )
        {
            // engine
            EngineOpenSLES::instance()->setPlayerEngine( true );
            EngineOpenSLES::instance()->setPlayerEngineRPM( _rpm->getValue() );

            // gunfire
            EngineOpenSLES::instance()->setPlayerGunfire( Data::get()->ownship.gunfire );

            // hearbeat
            EngineOpenSLES::instance()->setPlayerHeartbeat( Data::get()->ownship.hit_points < 25 );

            // bombs
            EngineOpenSLES::instance()->setPlayerBombs( Data::get()->ownship.bombs_drop < 5.0f );

            // hit
            EngineOpenSLES::instance()->setPlayerHit( Data::get()->ownship.ownship_hit < 0.5f );

            // waypoint
            EngineOpenSLES::instance()->setPlayerWaypoint( Data::get()->ownship.waypoint_time < 1.0 );
        }
        else
        {
            // crash
            if ( !_destroyed )
            {
                EngineOpenSLES::instance()->setPlayerCrash( true );
            }

            EngineOpenSLES::instance()->setPlayerEngine    ( false );
            EngineOpenSLES::instance()->setPlayerGunfire   ( false );
            EngineOpenSLES::instance()->setPlayerHeartbeat ( false );
            EngineOpenSLES::instance()->setPlayerHit       ( false );
            EngineOpenSLES::instance()->setPlayerWaypoint  ( false );
        }
    }
#   else
    if ( !_paused )
    {
        if ( !Data::get()->ownship.destroyed )
        {
            // engine
            EngineOpenAL::instance()->setEngine( true );
            EngineOpenAL::instance()->setEngineRPM( _rpm->getValue() );

            // gunfire
            EngineOpenAL::instance()->setGunfire( Data::get()->ownship.gunfire );

            // hearbeat
            EngineOpenAL::instance()->setHeartbeat( Data::get()->ownship.hit_points < 25 );

            // bombs
            EngineOpenAL::instance()->setBombs( Data::get()->ownship.bombs_drop < 5.0f );

            // hit
            EngineOpenAL::instance()->setHit( Data::get()->ownship.ownship_hit < 0.5f );

            // waypoint
            EngineOpenAL::instance()->setWaypoint( Data::get()->ownship.waypoint_time < 1.0 );
        }
        else
        {
            // crash
            if ( !_destroyed )
            {
                EngineOpenAL::instance()->setCrash( true );
            }

            EngineOpenAL::instance()->setEngine( false );
            EngineOpenAL::instance()->setGunfire( false );
            EngineOpenAL::instance()->setHeartbeat( false );
            EngineOpenAL::instance()->setHit( false );
            EngineOpenAL::instance()->setWaypoint( false );
        }
    }
#   endif

    _destroyed = Data::get()->ownship.destroyed;
}
