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
#include <XmlInputStream.hpp>
#include <cstring>
#include <stack>
#include <unistd.h>
#include <expat.h>
#include <forward_list>
#include <map>

#define THIS_FILE "XmlInputStream"

// Uncomment this for debug trace
//
//#define DEBUG_TRACE

#ifdef DEBUG_TRACE
#include <uxmpp/Logger.hpp>
#define TRACE(prefix, msg, ...) uxmppLogTrace(prefix, msg, ## __VA_ARGS__)
#else
#define TRACE(prefix, msg, ...)
#endif


START_NAMESPACE1(uxmpp)


using namespace std;


/**
 *
 */
class XmlStreamParseElement {
public:
    XmlStreamParseElement (int reserved_nodes) : xml_obj(reserved_nodes) { }

    XmlObject xml_obj;
    string default_namespace;
    map<string, string> namespace_aliases;
};

/**
 *
 */
class XmlInputStream::XmlParseData {
public:
    XML_Parser xml_parser;
    forward_list<XmlStreamParseElement*> element_stack;
    bool error;
    bool top_element_found;
    XmlInputStream* stream;
    string default_namespace;
    map<string, string> namespace_aliases;

    static void parse_xml_tag_name (const XML_Char* name, string& tag_name, string& xml_namespace);

    static void parse_xml_attributes (const XML_Char** attributes,
                                      XmlObject& xml_obj,
                                      string& default_namespace,
                                      map<string, string>& namespace_aliases);

    static void normalize_namespace (XmlInputStream::XmlParseData& pd, XmlObject& xml_obj);

    static void start_stream_element (void* user_data,
                                      const XML_Char* name,
                                      const XML_Char** attributes);
    static void start_xml_node (void* user_data,
                                const XML_Char* name,
                                const XML_Char** attributes);
    static void end_xml_node (void* user_data,
                              const XML_Char* name);
    static void xml_character_data (void* userData,
                                    const XML_Char* s,
                                    int len);
};




constexpr char namespace_delim {':'};



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::parse_xml_tag_name (const XML_Char* name,
                                                       string& tag_name,
                                                       string& xml_namespace)
{
    char* pos = strrchr ((char*)name, namespace_delim);
    if (!pos) {
        tag_name = name;
        xml_namespace = "";
    }else{
        tag_name      = string (pos+1, name+strlen(name)-(pos+1));
        xml_namespace = string (name, pos-name);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::parse_xml_attributes (const XML_Char** attributes,
                                                         XmlObject& xml_obj,
                                                         string& default_namespace,
                                                         map<string, string>& namespace_aliases)
{
    string xml_namespace = xml_obj.getNamespace ();
    string name;
    string value;

    default_namespace = "";
    namespace_aliases.clear ();

    for (const XML_Char** i=attributes; *i!=NULL; i++) {
        name  = string (*i);
        if (++i != NULL)
            value = string (*i);
        else
            value = "";

        // Handle default namespace attribute
        //
        if (name == "xmlns") {
            if (value.length() == 0)
                continue;

            TRACE (THIS_FILE, "parse_xml_attributes - set default namespace: ", value);

            default_namespace = value;
            xml_obj.setDefaultNamespaceAttr (default_namespace);
            if (xml_namespace.length() == 0) {
                xml_namespace = default_namespace;
                xml_obj.setNamespace (xml_namespace);
            }
            if (default_namespace == xml_namespace)
                xml_obj.isNamespaceDefault (true);
        }
        //
        // Handle namespace alias attribute
        //
        else if (name.find("xmlns:") == 0) {
            if (value.length() == 0)
                continue;
            string alias = name.substr (6);
            if (alias.length()) {

                TRACE (THIS_FILE, "parse_xml_attributes - add namespace alias: ", alias, "=", value);

                namespace_aliases[alias] = value;
/*
                if (alias == xml_obj.getNamespace()) {
                    uxmppLogDebug (THIS_FILE, "Set namespace to: ->", value, "<-");
                    xml_obj.setNamespace (value);
                    if (xml_obj.getDefaultNamespaceAttr() == xml_obj.getNamespace())
                        xml_obj.isNamespaceDefault (true);
                }
*/
            }
        }
        //
        // Handle 'normal' attribute
        //
        else{
            xml_obj.setAttribute (name, value);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::normalize_namespace (XmlInputStream::XmlParseData& pd,
                                                        XmlObject& xml_obj)
{
    string xml_namespace = xml_obj.getNamespace ();

    if (xml_namespace.length() == 0) {
        //
        // Find the default namespace
        //
        TRACE (THIS_FILE, "normalize_namespace - find the default namespace");
        xml_namespace = pd.default_namespace; // Fallback to top-level default namespace
        TRACE (THIS_FILE, "normalize_namespace - top level default namespace is: ", xml_namespace);
        for (auto element : pd.element_stack) {
            TRACE (THIS_FILE, "normalize_namespace - peek namespace in stack: ", element->default_namespace);
            if (element->default_namespace.length() > 0) {
                xml_namespace = element->default_namespace;
                break;
            }
        }
        if (xml_namespace.length() != 0) {
            TRACE (THIS_FILE, "normalize_namespace - found default namespace: ", xml_namespace);
            xml_obj.setNamespace (xml_namespace);
            xml_obj.isNamespaceDefault (true);
        }
    }else{
        //
        // Find a namespace alias
        //
        TRACE (THIS_FILE, "normalize_namespace - find a namespace alias");
        string alias_value = "";
        for (auto element : pd.element_stack) {
            alias_value = element->namespace_aliases[xml_namespace];
            if (alias_value.length() > 0)
                break;
        }
        if (alias_value.length() == 0)
            alias_value = pd.namespace_aliases[xml_namespace];

        if (alias_value.length()) {
            xml_obj.setNamespace (alias_value);
            // See if this is also the default namespace
            string default_namespace = pd.default_namespace;
            for (auto element : pd.element_stack) {
                if (element->default_namespace.length() > 0) {
                    default_namespace = element->default_namespace;
                    break;
                }
            }
            if (default_namespace.length() && default_namespace == xml_obj.getDefaultNamespaceAttr())
                xml_obj.isNamespaceDefault (true);
        }
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlInputStream::XmlInputStream (const XmlObject& top_element)
    :
    parse_data {nullptr},
    top_node   {top_element}
{
    reset ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlInputStream::~XmlInputStream ()
{
    freeResources ();
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::setXmlHandler (std::function<void (XmlInputStream&, XmlObject&)> xml_handler)
{
    std::lock_guard<std::mutex> lock (mutex);
    rx_func = xml_handler;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::setErrorHandler (std::function<void (XmlInputStream&)> err_handler)
{
    std::lock_guard<std::mutex> lock (mutex);
    err_func = err_handler;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::end_xml_node (void* user_data,
                                                 const XML_Char* name)
{
    XmlParseData* pd = reinterpret_cast<XmlParseData*> (user_data);
    XmlInputStream& stream = *pd->stream;

    // Check for end of the top-level xml tag
    //
    if (pd->element_stack.empty()) {
        if (pd->top_element_found) {
            pd->top_element_found = false;
            XmlObject xml_obj (stream.top_node);//XmlInputStreamTag, XmlInputStreamNs, false);
            xml_obj.setPart (XmlObjPart::end);

            XML_SetElementHandler (pd->xml_parser,
                                   XmlInputStream::XmlParseData::start_stream_element,
                                   NULL);
            if (stream.rx_func) {
                stream.mutex.unlock ();
                stream.rx_func (stream, xml_obj);
                stream.mutex.lock ();
            }
        }
        return;
    }

    // Pop a parsed element from the stack
    //
    XmlStreamParseElement* element = pd->element_stack.front ();
    pd->element_stack.pop_front ();
    TRACE (THIS_FILE, "end_xml_node - popping element from stack (", element->xml_obj.getName(), ")");

    if (pd->element_stack.empty()) {
        TRACE (THIS_FILE, "end_xml_node - complete XML object fround (", element->xml_obj.getName(), ")");

        if (stream.rx_func) {
            stream.mutex.unlock ();
            stream.rx_func (stream, element->xml_obj);
            stream.mutex.lock ();
        }
    }else{
        XmlStreamParseElement* parent_element = pd->element_stack.front ();
        parent_element->xml_obj.addNode (std::move(element->xml_obj));
    }
    delete element;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::start_xml_node (void* user_data,
                                                   const XML_Char* name,
                                                   const XML_Char** attributes)
{
    XmlParseData* pd = reinterpret_cast<XmlParseData*> (user_data);
    string tag_name;
    string xml_namespace;
    XmlStreamParseElement* parent_element = pd->element_stack.empty() ? nullptr : pd->element_stack.front();
    XmlStreamParseElement* element = new XmlStreamParseElement (parent_element ? 0 : 4);
    XmlObject& xml_obj = element->xml_obj;

    // Split the full tag name into name and namespace.
    //
    parse_xml_tag_name (name, tag_name, xml_namespace);
    xml_obj.setName (tag_name);
    if (xml_namespace.length())
        xml_obj.setNamespace (xml_namespace);

    // Parse xml attributes
    //
    TRACE (THIS_FILE, "start_xml_node - before parsing attributes: (", xml_obj.getNamespace(), ")", to_string(xml_obj));
    parse_xml_attributes (attributes, xml_obj, element->default_namespace, element->namespace_aliases);
    TRACE (THIS_FILE, "start_xml_node - after parsing attributes: (", xml_obj.getNamespace(), ")", to_string(xml_obj));

    // Normalize namespace
    //
    normalize_namespace (*pd, xml_obj);
    TRACE (THIS_FILE, "start_xml_node - after normalizing namespace: (", xml_obj.getFullName(), ")", to_string(xml_obj));

    // Push the current XML object on the stack
    //
    pd->element_stack.push_front (element);
    TRACE (THIS_FILE, "start_xml_node - pushing element to stack (", element->xml_obj.getName(), ")");
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::xml_character_data (void* user_data,
                                                       const XML_Char* data,
                                                       int len)
{
    XmlParseData* pd = reinterpret_cast<XmlParseData*> (user_data);
    XmlStreamParseElement* element = pd->element_stack.empty() ? nullptr : pd->element_stack.front();

    if (element == nullptr)
        return;

    element->xml_obj.setContent (element->xml_obj.getContent() + string(data, len));
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::XmlParseData::start_stream_element (void* user_data,
                                                         const XML_Char* name,
                                                         const XML_Char** attributes)
{
    XmlParseData* pd = reinterpret_cast<XmlParseData*> (user_data);
    XmlInputStream& stream = *pd->stream;
    string tag_name;
    string xml_namespace;
    string default_namespace = "";
    string full_name = "";
    XmlObject xml_obj (0);

    // Split the name into namespace and name
    //
    parse_xml_tag_name (name, tag_name, xml_namespace);
    xml_obj.setName (tag_name);
    if (xml_namespace.length())
        xml_obj.setNamespace (xml_namespace);

    // Parse xml attributes
    //
    TRACE (THIS_FILE, "start_stream_element - before parsing attributes: ", to_string(xml_obj));
    parse_xml_attributes (attributes, xml_obj, pd->default_namespace, pd->namespace_aliases);
    TRACE (THIS_FILE, "start_stream_element - after parsing attributes: ", to_string(xml_obj));

    // Normalize namespace
    //
    normalize_namespace (*pd, xml_obj);
    xml_namespace = xml_obj.getNamespace ();
    TRACE (THIS_FILE, "start_stream_element - after normalizing namespace: ", to_string(xml_obj));

    // Find default namespace if needed
    //
    if (!xml_namespace.length() && pd->default_namespace.length()) {
        // Find default namespace
        xml_namespace = pd->default_namespace;
        xml_obj.setNamespace (xml_namespace);
        xml_obj.isNamespaceDefault (true);
    }

    // Get the canonical tag name
    //
    if (xml_namespace.length()) {
        /*
        full_name = xml_obj.getNamespaceAlias (xml_namespace);
        if (full_name.length()) {
            full_name += string(":") + tag_name;
        }else{
        */
            full_name = xml_namespace + string(":") + tag_name;
            //}
    }else if (default_namespace.length() > 0) {
        full_name = default_namespace + string(":") + tag_name;
    }else{
        full_name = tag_name;
    }

    // Check if this is the XMPP start stream tag.
    //
    if (stream.top_node.getFullName() == full_name) {
        //uxmppLogDebug (THIS_FILE, "Got start stream element");
        XML_SetElementHandler (pd->xml_parser,
                               XmlInputStream::XmlParseData::start_xml_node,
                               XmlInputStream::XmlParseData::end_xml_node);
        XML_SetCharacterDataHandler (pd->xml_parser, XmlInputStream::XmlParseData::xml_character_data);

        xml_obj.setPart (XmlObjPart::start);

        pd->top_element_found = true;

        if (stream.rx_func) {
            stream.mutex.unlock ();
            stream.rx_func (stream, xml_obj);
            stream.mutex.lock ();
        }
    }else{
        //uxmppLogDebug (THIS_FILE, "Ignoring XML element: ", full_name);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlInputStream::reset ()
{
    std::lock_guard<std::mutex> lock (mutex);
    freeResources ();

    parse_data = new XmlParseData;
    //parse_data->xml_parser = XML_ParserCreateNS (NULL, namespace_delim);
    parse_data->xml_parser = XML_ParserCreate (NULL);
    parse_data->error      = false;
    parse_data->stream     = this;
    parse_data->top_element_found = false;

    parse_data->default_namespace = "";
    parse_data->namespace_aliases.clear ();

    XML_SetUserData (parse_data->xml_parser, parse_data);
    XML_SetElementHandler (parse_data->xml_parser,
                           XmlInputStream::XmlParseData::start_stream_element,
                           NULL);
}


//------------------------------------------------------------------------------
// mutex assumed to be locked
//------------------------------------------------------------------------------
void XmlInputStream::freeResources ()
{
    if (parse_data) {
        while (!parse_data->element_stack.empty()) {
            delete parse_data->element_stack.front ();
            parse_data->element_stack.pop_front ();
        }
        if (parse_data->xml_parser != nullptr) {
            XML_ParserFree (parse_data->xml_parser);
            parse_data->xml_parser = nullptr;
        }
        delete parse_data;
        parse_data = nullptr;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlInputStream& XmlInputStream::operator<< (const char ch)
{
    std::lock_guard<std::mutex> lock (mutex);
    //std::lock_guard<std::mutex> lock (mutex);
    //uxmppLogTrace (THIS_FILE, "RX:\n", input, "\n");

    // Ignore incoming data on error.
    //
    if (parse_data->error) {
        //uxmppLogDebug (THIS_FILE, "Ignore XML input when a parse error has occurred");
        return *this;
    }

    // Parse the incoming data
    //
    if (!XML_Parse(parse_data->xml_parser, &ch, 1, 0)) {
        parse_data->error = true;
        //uxmppLogError (THIS_FILE, "RX XML parse error");
        while (parse_data && !parse_data->element_stack.empty()) {
            delete parse_data->element_stack.front ();
            parse_data->element_stack.pop_front ();
        }

        if (err_func) {
            mutex.unlock ();
            err_func (*this);
            mutex.lock ();
        }
    }

    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlInputStream& XmlInputStream::operator<< (const std::string& input)
{
    std::lock_guard<std::mutex> lock (mutex);
    //std::lock_guard<std::mutex> lock (mutex);
    //uxmppLogTrace (THIS_FILE, "RX:\n", input, "\n");

    // Ignore incoming data on error.
    //
    if (parse_data->error) {
        //uxmppLogDebug (THIS_FILE, "Ignore XML input when a parse error has occurred");
        return *this;
    }

    // Parse the incoming data
    //
    if (!XML_Parse(parse_data->xml_parser, input.c_str(), input.length(), 0)) {
        parse_data->error = true;
        //uxmppLogError (THIS_FILE, "RX XML parse error");
        while (parse_data && !parse_data->element_stack.empty()) {
            delete parse_data->element_stack.front ();
            parse_data->element_stack.pop_front ();
        }
        if (err_func) {
            mutex.unlock ();
            err_func (*this);
            mutex.lock ();
        }
    }

    return *this;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
XmlInputStream& XmlInputStream::operator<< (const XmlObject& xml_obj)
{
    std::lock_guard<std::mutex> lock (mutex);

    if (!xml_obj)
        return *this;

    XmlObject xo (xml_obj);

    if (rx_func) {
        mutex.unlock ();
        rx_func (*this, xo);
        mutex.lock ();
    }

    return *this;
}


END_NAMESPACE1
