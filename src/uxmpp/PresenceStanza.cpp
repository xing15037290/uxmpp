/*
 *  Copyright (C) 2013-2015 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/Logger.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/PresenceStanza.hpp>
#include <uxmpp/xml/names.hpp>

#define THIS_FILE "PresenceStanza.cpp"


UXMPP_START_NAMESPACE1(uxmpp)



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const SubscribeOp& type)
{
    switch (type) {
    case SubscribeOp::subscribe :
        return "subscribe";
    case SubscribeOp::unsubscribe :
        return "unsubscribe";
    case SubscribeOp::subscribed :
        return "subscribed";
    case SubscribeOp::unsubscribed :
    default :
        return "unsubscribed";
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza::PresenceStanza (const std::string& to,
                                const std::string& from,
                                const std::string& id)
    : Stanza (to, from, id)
{
    set_tag_name ("presence");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza::PresenceStanza (const Jid& to, const Jid& from, const std::string& id)
    : Stanza (to, from, id)
{
    set_tag_name ("presence");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza::PresenceStanza (const PresenceStanza& msg_stanza)
    : Stanza (msg_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza::PresenceStanza (PresenceStanza&& msg_stanza)
    : Stanza (msg_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::operator= (const PresenceStanza& msg_stanza)
{
    if (this != &msg_stanza)
        Stanza::operator= (msg_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::operator= (PresenceStanza&& msg_stanza)
{
    Stanza::operator= (msg_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SubscribeOp PresenceStanza::get_subscribe_op ()
{
    std::string type = get_attribute ("type");
    if (type == "subscribe")
        return SubscribeOp::subscribe;
    else if (type == "unsubscribe")
        return SubscribeOp::unsubscribe;
    else if (type == "subscribed")
        return SubscribeOp::subscribed;
    else
        return SubscribeOp::unsubscribed;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::set_subscribe_op (const SubscribeOp& type)
{
    return reinterpret_cast<PresenceStanza&> (set_attribute("type", to_string(type)));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PresenceStanza::get_show ()
{
    auto node = find_node ("show");
    return node ? node.get_content() : "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::set_show (const std::string& content)
{
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() == "show") {
            if (content == "")
                i = nodes.erase (i);
            else
                i->set_content (content);
            return *this;
        }
    }
    if (content != "")
        add_node (XmlObject("show", xml::namespace_jabber_client, false).set_content(content));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PresenceStanza::get_status (const std::string& lang)
{
    std::string configured_lang = get_attribute ("xml:lang");
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "status")
            continue;
        std::string node_lang = i->get_attribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang)
                return i->get_content ();
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang))
                return i->get_content ();
        }
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > PresenceStanza::get_status_list ()
{
    std::vector<std::pair<std::string, std::string> > status_list;
    for (auto node : get_nodes()) {
        if (node.get_tag_name() == "status")
            status_list.push_back (std::pair<std::string, std::string>(node.get_content(),
                                                                       node.get_attribute("xml:lang")));
    }
    return status_list;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::set_status (const std::string& status, const std::string& lang)
{
    std::string configured_lang = get_attribute ("xml:lang");
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "status")
            continue;
        std::string node_lang = i->get_attribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang) {
                if (status == "")
                    i = nodes.erase (i);
                else
                    i->set_content (status);
                return *this;
            }
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang)) {
                if (status == "")
                    i = nodes.erase (i);
                else
                    i->set_content (status);
                return *this;
            }
        }
    }
    if (status != "") {
        XmlObject status_node ("status", xml::namespace_jabber_client, false);
        if (lang!="" && lang!=configured_lang)
            status_node.set_attribute ("xml:lang", lang);
        status_node.set_content (status);
        add_node (status_node);
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int PresenceStanza::get_priority ()
{
    auto node = find_node ("priority");
    try {
        return stoi (node.get_content());
    }
//    catch (invalid_argument ia) {
    catch (...) {
        uxmpp_log_debug (THIS_FILE, "Content of 'priority' element is not a number");
        return 0;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::set_priority (int prio)
{
    if (prio > 127)
        prio = 127;
    else if (prio < -128)
        prio = -128;
    std::stringstream ss;
    ss << prio;

    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "priority")
            continue;
        if (prio == 0)
            i = nodes.erase (i);
        else
            i->set_content (ss.str());
        return *this;
    }

    if (prio != 0)
        add_node (XmlObject("priority", xml::namespace_jabber_client, false).set_content(ss.str()));
    return *this;
}



UXMPP_END_NAMESPACE1
