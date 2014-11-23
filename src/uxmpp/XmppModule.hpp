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
#ifndef UXMPP_XMPPMODULE_HPP
#define UXMPP_XMPPMODULE_HPP

#include <string>
#include <vector>
#include <uxmpp/types.hpp>


namespace uxmpp {


    // Forward declarations.
    class Session;
    class XmlObject;


    /**
     * An XMPP module.
     */
    class XmppModule {
    public:

        /**
         * Constructor.
         */
        XmppModule (const std::string& name) {
            this->name = name.length() ? name : "<anonymous>";
        }

        /**
         * Destructor.
         */
        virtual ~XmppModule () = default;

        /**
         * Return the name of the XMPP module.
         */
        const std::string& get_name () const {
            return name;
        }

        /**
         * Called when the module is registered to a session.
         */
        virtual void module_registered (Session& session) {}

        /**
         * Called when the module is unregistered from a session.
         */
        virtual void module_unregistered (Session& session) {}

        /**
         * Called whan an XML object is received.
         * @return Return true if this XML object was processed and no further work should be done.
         */
        virtual bool proccess_xml_object (Session& session, XmlObject& xml_obj) {
            return false;
        }

        /**
         * Return a list of service discovery information features supported
         * by the module;
         */
        virtual std::vector<std::string> get_disco_features () {
            return std::vector<std::string> ();
        }

    protected:

        /**
         * The name of the XMPP module.
         */
        std::string name;
    };


}


#endif
