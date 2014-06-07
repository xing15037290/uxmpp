/*
 *  Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
 *
 *  This file is part of uxmpp.
 *
 *  uxmpp is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef UXMPP_MOD_ROSTER_HPP
#define UXMPP_MOD_ROSTER_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/XmlNames.hpp>
#include <uxmpp/mod/RosterItem.hpp>
#include <vector>
#include <set>


namespace uxmpp { namespace mod {


    /**
     *
     */
    class Roster : public uxmpp::XmlObject {
    public:

        /**
         * Constructor.
         */
        Roster (const std::string& version="")
            : uxmpp::XmlObject ("query", XmlIqRosterNs, true, true, 1)
        {
            if (version.length())
                setAttribute ("ver", version);
        }

        /**
         * Copy constructor.
         */
        Roster (const Roster& roster) : uxmpp::XmlObject (roster) {
        }

        /**
         * Move constructor.
         */
        Roster (Roster&& roster) : uxmpp::XmlObject (roster) {
        }

        /**
         * Destructor.
         */
        virtual ~Roster () = default;

        /**
         * Assignment operator.
         */
        Roster& operator= (const Roster& roster) {
            if (this != &roster) {
                uxmpp::XmlObject::operator= (roster);
            }
            return *this;
        }

        /**
         * Assignment operator.
         */
        Roster& operator= (const uxmpp::XmlObject& roster) {
            if (this != &roster) {
                uxmpp::XmlObject::operator= (roster);
            }
            return *this;
        }

        /**
         * Move operator.
         */
        Roster& operator= (Roster&& roster) {
            uxmpp::XmlObject::operator= (roster);
            return *this;
        }

        /**
         * Return the roster items.
         */
        std::vector<RosterItem>& getItems () {
            return reinterpret_cast<std::vector<RosterItem>&> (getNodes());
        }
    };


}}


#endif
