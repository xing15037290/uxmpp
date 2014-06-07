/*
 *  Copyright (C) 2014 Ultramarin Design AB <dan@ultramarin.se>
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
#ifndef UXMPP_MOD_BUDDYITEM_HPP
#define UXMPP_MOD_BUDDYITEM_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/mod/RosterItem.hpp>
#include <vector>
#include <string>

namespace uxmpp { namespace mod {


    /**
     *
     */
    class Buddy {
    public:

        /**
         * Default constructor.
         */
        Buddy () = defult;

        /**
         * Constructor.
         */
        Buddy (const RosterItem& item) : roster_item{item} {
        }

        /**
         * Destructor.
         */
        virtual ~Buddy () = default;

        /**
         * Get the group(s) that the buddy belongs to.
         */
        std::vector<std::string> getGroups () {
            return roster_item.getGroups;
        }

        /**
         * Get the name of the buddy.
         */
        std::string getName () const {
            return roster_item.getHandle ();
        }

        /**
         * Get the bare JID of the buddy.
         */
        uxmpp::Jid getJid () const {
            return roster_item.getJid().bare ();
        }


    protected:
        RosterItem roster_item;
        std::vector<std::string> resources;


    private:
    };


}}


#endif
