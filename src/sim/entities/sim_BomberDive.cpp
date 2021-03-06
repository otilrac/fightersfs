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

#include <sim/entities/sim_BomberDive.h>

#include <sim/utils/sim_Convert.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

const std::string BomberDive::_tagName = "bomber_dive";

const float BomberDive::_dropAltMin = 300.0f;
const float BomberDive::_dropAltMax = 400.0f;

////////////////////////////////////////////////////////////////////////////////

BomberDive::BomberDive( Affiliation affiliation ) :
    Bomber( affiliation ),

    _attack ( false )
{}

////////////////////////////////////////////////////////////////////////////////

BomberDive::~BomberDive() {}

////////////////////////////////////////////////////////////////////////////////

int BomberDive::read( const XmlNode &node )
{
    //////////////////////////////////
    int result = Bomber::read( node );
    //////////////////////////////////

    if ( result == SIM_SUCCESS )
    {
        XmlNode nodeData = node.getFirstChildElement( "bomber_dive" );
        readOrdnance( nodeData );

        return SIM_SUCCESS;
    }

    return SIM_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::limitTht( float &tht )
{
    if ( _attack )
    {
        if ( tht < -_target_tht ) tht = -_target_tht;
        if ( tht >  _target_tht ) tht =  _target_tht;
    }
    else
    {
        ////////////////////////
        Bomber::limitTht( tht );
        ////////////////////////
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::updateTarget()
{
    ///////////////////////
    Bomber::updateTarget();
    ///////////////////////

    _attack = false;

    if ( _engaged )
    {
        if ( _target_alt > _dropAltMin )
        {
            float max_range = ( _airspeed / SIM_GRAVITY_ACC ) * sqrt( 2.0f*SIM_GRAVITY_ACC*_target_alt );

            if ( _target_dist < 0.8f*max_range )
            {
                _attack = true;

                float v_2 = _airspeed * _airspeed;
                float num = v_2 - sqrt( v_2*v_2 - SIM_GRAVITY_ACC*(SIM_GRAVITY_ACC*_target_dist*_target_dist - 2.0f*_target_alt*v_2) );
                float den = SIM_GRAVITY_ACC * _target_dist;

                _target_tht = atan2( num, den );


                _destination.first  = _target_pos;
                _destination.second = _speed_min;
            }
        }
        else
        {
            _engaged = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void BomberDive::updateWeapons()
{
    ////////////////////////
    Bomber::updateWeapons();
    ////////////////////////

    if ( _engaged )
    {
        if ( _target_alt < _dropAltMax  )
        {
            _trigger = true;
        }
    }
    else
    {
        _trigger = false;
    }

    if ( _trigger && _ordnanceIndex < _ordnance.size() )
    {
        if ( _time_drop > 0.5f )
        {
            releaseWeapon();
        }
    }
}
