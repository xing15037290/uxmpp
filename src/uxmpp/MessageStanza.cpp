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


UXMPP_START_NAMESPACE1(uxmpp)

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
MessageStanza::MessageStanza (const std::string& to,
                              const std::string& from,
                              const std::string& body,
                              const MessageType  type,
                              const ChatState    chat_state,
                              const std::string& id,
                              const std::string& lang)
    : Stanza (to, from, id)
{
    set_tag_name ("message");
    set_message_type (type);
    set_chat_state (chat_state);
    set_body (body, lang);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza::MessageStanza (const Jid&         to,
                              const Jid&         from,
                              const std::string& body,
                              const MessageType  type,
                              const ChatState    chat_state,
                              const std::string& id,
                              const std::string& lang)
    : Stanza (to, from, id)
{
    set_tag_name ("message");
    set_message_type (type);
    set_chat_state (chat_state);
    set_body (body, lang);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza::MessageStanza (const MessageStanza& msg_stanza)
    : Stanza (msg_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza::MessageStanza (MessageStanza&& msg_stanza)
    : Stanza (msg_stanza)
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::operator= (const MessageStanza& msg_stanza)
{
    if (this != &msg_stanza)
        Stanza::operator= (msg_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::operator= (MessageStanza&& msg_stanza)
{
    Stanza::operator= (msg_stanza);
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageType MessageStanza::get_message_type ()
{
    std::string type = get_type ();

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
MessageStanza& MessageStanza::set_message_type (const MessageType type)
{
    set_type (to_string(type));
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::get_thread ()
{
    return find_node("thread").get_content ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::set_thread (const std::string& thread_id, const std::string& parent_thread_id)
{
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "thread")
            continue;
        if (thread_id == "") {
            nodes.erase (i);
        }else{
            i->set_content (thread_id);
            if (parent_thread_id == "")
                i->remove_attribute ("parent");
            else
                i->set_attribute ("parent", parent_thread_id);
        }
        return *this;
    }
    if (thread_id != "") {
        if (parent_thread_id == "")
            add_node (XmlObject("thread", XmlJabberClientNs, false).set_content(thread_id));
        else
            add_node (XmlObject("thread", XmlJabberClientNs, false).
                      set_content(thread_id).set_attribute("parent", parent_thread_id));
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::get_parent_thread ()
{
    return find_node("thread").get_attribute ("parent");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MessageStanza& MessageStanza::set_parent_thread (const std::string& parent_thread_id)
{
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "thread")
            continue;
        if (parent_thread_id=="")
            i->remove_attribute ("parent");
        else
            i->set_attribute ("parent", parent_thread_id);
        break;
    }
    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MessageStanza::get_body (std::string lang)
{
    string configured_lang = get_attribute ("xml:lang");
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "body")
            continue;
        string node_lang = i->get_attribute ("xml:lang");
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
MessageStanza& MessageStanza::set_body (const std::string& body, std::string lang)
{
    std::string configured_lang = get_attribute ("xml:lang");
    auto& nodes = get_nodes ();
    for (auto i=nodes.begin(); i!=nodes.end(); ++i) {
        if (i->get_tag_name() != "body")
            continue;
        std::string node_lang = i->get_attribute ("xml:lang");
        if (lang=="") {
            if (node_lang=="" || node_lang==configured_lang) {
                if (body == "")
                    nodes.erase (i);
                else
                    i->set_content (body);
                return *this;
            }
        }else{
            if (lang==node_lang || (node_lang=="" && lang==configured_lang)) {
                if (body == "")
                    nodes.erase (i);
                else
                    i->set_content (body);
                return *this;
            }
        }
    }
    if (body != "") {
        XmlObject body_node ("body", XmlJabberClientNs, false);
        if (lang!="" && lang!=configured_lang)
            body_node.set_attribute ("xml:lang", lang);
        body_node.set_content (body);
        add_node (body_node);
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
ChatState MessageStanza::get_chat_state ()
{
    string state = find_node_by_namespace(ChatStateNs).get_tag_name ();

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
MessageStanza& MessageStanza::set_chat_state (const ChatState state)
{
    for (auto& node : get_nodes()) {
        //
        // Find the first node with the correct name space and set the name of the object.
        //
        if (node.get_namespace() == ChatStateNs) {
            node.set_tag_name (to_string(state));
            return *this;
        }
    }
    //
    // No xml object found, add it.
    //
    add_node (XmlObject(to_string(state), ChatStateNs));
    return *this;
}


UXMPP_END_NAMESPACE1
