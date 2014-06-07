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
#include <uxmpp/Logger.hpp>
#include <uxmpp/utils.hpp>
#include <uxmpp/PresenceStanza.hpp>

#define THIS_FILE "PresenceStanza.cpp"


START_NAMESPACE1(uxmpp)



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
SubscribeOp PresenceStanza::getSubscribeOp ()
{
    std::string type = getAttribute ("type");
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
PresenceStanza& PresenceStanza::setSubscribeOp (const SubscribeOp& type)
{
    return reinterpret_cast<PresenceStanza&> (setAttribute("type", to_string(type)));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PresenceStanza::getShow ()
{
    auto node = getNode ("show");
    return node ? node.getContent() : "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::setShow (const std::string& content)
{
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() == "show") {
            if (content == "")
                nodes.erase (i);
            else
                i->setContent (content);
            return *this;
        }
    }
    if (content != "")
        addNode (XmlObject("show", XmlJabberClientNs, false).setContent(content));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string PresenceStanza::getStatus (const std::string& lang)
{
    std::string configured_lang = getAttribute ("xml:lang");
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "status")
            continue;
        std::string node_lang = i->getAttribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang)
                return i->getContent ();
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang))
                return i->getContent ();
        }
    }
    return "";
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > PresenceStanza::getStatusList ()
{
    std::vector<std::pair<std::string, std::string> > status_list;
    for (auto node : getNodes()) {
        if (node.getName() == "status")
            status_list.push_back (std::pair<std::string, std::string>(node.getContent(),
                                                                       node.getAttribute("xml:lang")));
    }
    return status_list;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::setStatus (const std::string& status, const std::string& lang)
{
    std::string configured_lang = getAttribute ("xml:lang");
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "status")
            continue;
        std::string node_lang = i->getAttribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang) {
                if (status == "")
                    nodes.erase (i);
                else
                    i->setContent (status);
                return *this;
            }
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang)) {
                if (status == "")
                    nodes.erase (i);
                else
                    i->setContent (status);
                return *this;
            }
        }
    }
    if (status != "") {
        XmlObject status_node ("status", XmlJabberClientNs, false);
        if (lang!="" && lang!=configured_lang)
            status_node.setAttribute ("xml:lang", lang);
        status_node.setContent (status);
        addNode (status_node);
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int PresenceStanza::getPriority ()
{
    auto node = getNode ("priority");
    if (node)
        return atoi (node.getContent().c_str());
    else
        return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PresenceStanza& PresenceStanza::setPriority (int prio)
{
    if (prio > 127)
        prio = 127;
    else if (prio < -128)
        prio = -128;
    std::stringstream ss;
    ss << prio;

    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "priority")
            continue;
        if (prio == 0)
            nodes.erase (i);
        else
            i->setContent (ss.str());
        return *this;
    }

    if (prio != 0)
        addNode (XmlObject("priority", XmlJabberClientNs, false).setContent(ss.str()));
    return *this;
}



END_NAMESPACE1
