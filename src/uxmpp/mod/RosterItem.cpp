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
#include <uxmpp/XmlNames.hpp>
#include <uxmpp/mod/RosterItem.hpp>

#define THIS_FILE "RosterItem"


UXMPP_START_NAMESPACE2(uxmpp, mod)


using namespace std;
using namespace uxmpp;




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem ()
    : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem (const uxmpp::Jid& jid, const std::string& handle)
    : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
{
    std::string strjid = to_string (jid);
    if (!strjid.empty())
        set_attribute ("jid", strjid);
    if (!handle.empty())
        set_attribute ("name", handle);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem (const uxmpp::Jid& jid, const std::vector<std::string>& groups)
    : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
{
    std::string strjid = to_string (jid);
    if (!strjid.empty())
        set_attribute ("jid", strjid);
    for (auto group : groups)
        add_node (XmlObject("group", XmlIqRosterNs, false).set_content(group));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem (const uxmpp::Jid& jid, const std::string& handle, const std::vector<std::string>& groups)
    : uxmpp::XmlObject ("item", XmlIqRosterNs, false, true, 1)
{
    std::string strjid = to_string (jid);
    if (!strjid.empty())
        set_attribute ("jid", strjid);
    if (!handle.empty())
        set_attribute ("name", handle);
    for (auto group : groups)
        add_node (XmlObject("group", XmlIqRosterNs, false).set_content(group));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem (const RosterItem& item)
    : uxmpp::XmlObject (item)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem::RosterItem (RosterItem&& item)
    : uxmpp::XmlObject (item)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem& RosterItem::operator= (const RosterItem& item)
{
    if (this != &item)
        uxmpp::XmlObject::operator= (item);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RosterItem& RosterItem::operator= (RosterItem&& item)
{
    uxmpp::XmlObject::operator= (item);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uxmpp::Jid RosterItem::get_jid () const
{
    return uxmpp::Jid (get_attribute("jid"));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterItem::set_jid (const uxmpp::Jid& jid)
{
    set_attribute ("jid", to_string(jid));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string RosterItem::get_handle () const
{
    return get_attribute ("name");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterItem::set_handle (const std::string& handle)
{
    set_attribute ("name", handle);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool RosterItem::is_approved () const
{
    return get_attribute("approved") == "true";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void RosterItem::set_approved (bool approved)
{
    set_attribute("approved", approved ? "true" : "");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string RosterItem::get_ask () const
{
    return get_attribute ("ask");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string RosterItem::get_subscription () const
{
    return get_attribute ("subscription");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::string> RosterItem::get_groups ()
{
    std::vector<std::string> g;
    for (auto& node : get_nodes()) {
        if (node.get_full_name() == XmlRosterGroupTagFull) {
            string group_name = node.get_content ();
            if (group_name.length())
                g.push_back (group_name);
        }
    }
    return g;
}


UXMPP_END_NAMESPACE2
