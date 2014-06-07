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
#include <uxmpp/MessageStanza.hpp>

#define THIS_FILE "MessageStanza.cpp"


START_NAMESPACE1(uxmpp)

using namespace std;


const string ChatStateNs = string ("http://jabber.org/protocol/chatstates");


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const MessageType& type)
{
    switch (type) {
    case MessageType::chat:
        return "chat";

    case MessageType::error:
        return "error";

    case MessageType::groupchat:
        return "groupchat";

    case MessageType::headline:
        return "headline";

    case MessageType::normal:
    default:
        return "normal";
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string to_string (const ChatState& state)
{
    switch (state) {
    case ChatState::active:
        return "active";

    case ChatState::composing:
        return "composing";

    case ChatState::paused:
        return "paused";

    case ChatState::inactive:
        return "inactive";

    case ChatState::gone:
        return "gone";

    case ChatState::none:
    default:
        return "";
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageType MessageStanza::getMessageType ()
{
    std::string type = getType ();

    if (type == "chat")
        return MessageType::chat;
    else if (type == "error")
        return MessageType::error;
    else if (type == "groupchat")
        return MessageType::groupchat;
    else if (type == "headline")
        return MessageType::headline;
    else
        return MessageType::normal;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::setMessageType (const MessageType type)
{
    setType (to_string(type));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::getThread ()
{
    return getNode("thread").getContent ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::setThread (const std::string& thread_id, const std::string& parent_thread_id)
{
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "thread")
            continue;
        if (thread_id == "") {
            nodes.erase (i);
        }else{
            i->setContent (thread_id);
            if (parent_thread_id == "")
                i->removeAttribute ("parent");
            else
                i->setAttribute ("parent", parent_thread_id);
        }
        return *this;
    }
    if (thread_id != "") {
        if (parent_thread_id == "")
            addNode (XmlObject("thread", XmlJabberClientNs, false).setContent(thread_id));
        else
            addNode (XmlObject("thread", XmlJabberClientNs, false).
                     setContent(thread_id).setAttribute("parent", parent_thread_id));
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::getParentThread ()
{
    return getNode("thread").getAttribute ("parent");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::setParentThread (const std::string& parent_thread_id)
{
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "thread")
            continue;
        if (parent_thread_id=="")
            i->removeAttribute ("parent");
        else
            i->setAttribute ("parent", parent_thread_id);
        break;
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::getBody (std::string lang)
{
    string configured_lang = getAttribute ("xml:lang");
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "body")
            continue;
        string node_lang = i->getAttribute ("xml:lang");
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
MessageStanza& MessageStanza::setBody (const std::string& body, std::string lang)
{
    std::string configured_lang = getAttribute ("xml:lang");
    auto& nodes = getNodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); i++) {
        if (i->getName() != "body")
            continue;
        std::string node_lang = i->getAttribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang) {
                if (body == "")
                    nodes.erase (i);
                else
                    i->setContent (body);
                return *this;
            }
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang)) {
                if (body == "")
                    nodes.erase (i);
                else
                    i->setContent (body);
                return *this;
            }
        }
    }
    if (body != "") {
        XmlObject body_node ("body", XmlJabberClientNs, false);
        if (lang!="" && lang!=configured_lang)
            body_node.setAttribute ("xml:lang", lang);
        body_node.setContent (body);
        addNode (body_node);
    }
    return *this;
/*
    for (auto& node : getNodes()) {
        if (node.getName() == "body") {
            node.setContent (body);
            return *this;
        }
    }
    addNode (XmlObject("body", XmlJabberClientNs, false).setContent(body));
    return *this;
*/
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
ChatState MessageStanza::getChatState ()
{
    string state = getNsNode(ChatStateNs).getName ();

    if (state == "active")
        return ChatState::active;
    else if (state == "composing")
        return ChatState::composing;
    else if (state == "paused")
        return ChatState::paused;
    else if (state == "inactive")
        return ChatState::inactive;
    else if (state == "gone")
        return ChatState::gone;
    else
        return ChatState::none; // No valid chat state found.
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::setChatState (const ChatState state)
{
    for (auto& node : getNodes()) {
        //
        // Find the first node with the correct name space and set the name of the object.
        //
        if (node.getNamespace() == ChatStateNs) {
            node.setName (to_string(state));
            return *this;
        }
    }
    //
    // No xml object found, add it.
    //
    addNode (XmlObject(to_string(state), ChatStateNs));
    return *this;
}


END_NAMESPACE1
