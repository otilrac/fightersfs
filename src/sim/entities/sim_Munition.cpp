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

#include <sim/entities/sim_Munition.h>

#include <sim/cgi/sim_Models.h>
#include <sim/entities/sim_Entities.h>

////////////////////////////////////////////////////////////////////////////////

using namespace sim;

////////////////////////////////////////////////////////////////////////////////

Munition::Munition( UInt16 dp, UInt32 shooterId, float life_span, Group *parent ) :
    Entity( parent, Active, life_span ),

    _dp ( dp ),
    _shooterId ( shooterId )
{}

////////////////////////////////////////////////////////////////////////////////

Munition::~Munition() {}

////////////////////////////////////////////////////////////////////////////////

void Munition::load()
{
    if ( _model.valid() )
    {
        _switch->removeChild( _model.get() );
    }

    _model = Models::get( _modelFile );

    if ( _model.valid() )
    {
        _switch->addChild( _model.get() );
    }
}

////////////////////////////////////////////////////////////////////////////////

void Munition::update( double timeStep )
{
    ///////////////////////////
    Entity::update( timeStep );
    ///////////////////////////

    if ( isActive() )
    {
        if ( isTopLevel() )
        {
            List::iterator it = Entities::instance()->getEntities()->begin();

            while ( it != Entities::instance()->getEntities()->end() )
            {
                Unit *target = dynamic_cast< Unit* >(*it);

                if ( target )
                {
                    if ( target->isActive() )
                    {
                        Vec3 v1 = _att * ( _vel * timeStep );
                        Vec3 v2 = target->getPos() - _pos;

                        float len1 = v1.length();

                        Vec3 v1_norm = v1 * (1.0/len1);

                        float proj2 = v2 * v1_norm;

                        if ( proj2 >= 0.0 && proj2 <= len1 )
                        {
                            float dist2 = ( v2 - v1_norm * proj2 ).length2();

                            if ( dist2 < target->getRadius2() && _shooterId != target->getId() )
                            {
                                hit( target );
                                setState( Inactive );
                                break;
                            }
                        }
                    }
                }

                ++it;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Munition::setModelFile( const std::string &modelFile )
{
    _modelFile = modelFile;
    load();
}

////////////////////////////////////////////////////////////////////////////////

void Munition::hit( Unit *target )
{
    target->hit( _dp, this );
    reportTargetHit( target );
}

////////////////////////////////////////////////////////////////////////////////

void Munition::reportTargetHit( Unit *target )
{
    Unit *shooter = dynamic_cast< Unit* >( Entities::instance()->getEntityById( _shooterId ) );

    if ( shooter )
    {
        shooter->reportTargetHit( target );
    }
}
