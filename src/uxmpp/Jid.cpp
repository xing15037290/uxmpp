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
#include <uxmpp/Jid.hpp>
#include <uxmpp/Logger.hpp>

#define THIS_FILE "Jid"


START_NAMESPACE1(uxmpp)


using namespace std;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid::Jid (const std::string& jid)
    :
    local    {""},
    domain   {""},
    resource {""}
{
    //
    // TODO: Use regex in C++11 when G++ implements it !!!
    //

    if (jid.length() == 0)
        return;

    // Set local part
    //
    auto pos = jid.find ('@');
    if (pos == std::string::npos) {
        setLocal (jid);
        return;
    }
    if (pos==0) {
        local = "";
    }else{
        setLocal (jid.substr(0, pos));
    }

    if (++pos >= jid.length()-1)
        return;

    // Set domain and resource part
    //
    auto domain_pos = pos;
    pos = jid.find ('/', pos);
    if (pos == std::string::npos) {
        // set domain
        setDomain (jid.substr(domain_pos));
    }
    else if (pos == domain_pos) {
        // Set resource
        if (pos < jid.length()-1) {
            setResource (jid.substr(pos+1));
        }
    }
    else {
        // Set domain and resource
        setDomain (jid.substr(domain_pos, pos-domain_pos));
        setResource (jid.substr(pos+1));
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Jid::Jid (const std::string& localpart, const std::string& domainpart, const std::string& resourcepart)
{
    setLocal    (localpart);
    setDomain   (domainpart);
    setResource (resourcepart);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Jid::setLocal (const std::string& localpart)
{
    if (localpart.length() > 1023) {
        uxmppLogInfo (THIS_FILE, "localpart longer than 1023 bytes, truncate it");
        local = std::string(localpart, 1023);
    }else{
        local = localpart;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Jid::setDomain (const std::string& domainpart)
{
    if (domainpart.length() > 1023) {
        uxmppLogInfo (THIS_FILE, "domainpart longer than 1023 bytes, truncate it");
        domain = std::string(domainpart, 1023);
    }else{
        domain = domainpart;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Jid::setResource (const std::string& resourcepart)
{
    if (resourcepart.length() > 1023) {
        uxmppLogInfo (THIS_FILE, "resourcepart longer than 1023 bytes, truncate it");
        resource = std::string(resourcepart, 1023);
    }else{
        resource = resourcepart;
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const Jid& jid)
{
    string result {""};

    if (jid.getLocal().length())
        result += jid.getLocal();

    if (jid.getDomain().length()) {
        result += string("@") + jid.getDomain ();
    }

    if (jid.getResource().length()) {
        if (!jid.getDomain().length())
            result += string("@");
        result += string("/") + jid.getResource();
    }

    return result;
}


END_NAMESPACE1
