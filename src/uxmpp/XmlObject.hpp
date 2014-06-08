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
#ifndef UXMPP_XMLOBJECT_HPP
#define UXMPP_XMLOBJECT_HPP

#include <uxmpp/types.hpp>
#include <string>
#include <vector>
//#include <map>
#include <unordered_map>


namespace uxmpp {


    /**
     * This defines a specific part of an XML object.
     */
    enum class XmlObjPart {
        start, /**< The start tag with all attributes. */
        body,  /**< The body of the XML object. */
        end,   /**< The end tag of the XML object. */
        all    /**< The whole XML object. */
    };


    /**
     * Return a string representation of an XmlObjPart value.
     * @param part The XmlObjPart value.
     * @return A string representation of an XmlObjPart value.
     */
    std::string to_string (const XmlObjPart& part);


    /**
     * This is a representation of an XML object including attributes and child elements.
     */
    class XmlObject {
    public:

        /**
         * Constructor.
         * This will create an empty nameless XML object that will be
         * treated as a false value in boolean expressions and as an
         * empty string when represented as a string.
         * @param reserved_nodes The number of reserved child nodes for the XML object.
         *                       If it is known at creation time how may child nodes
         *                       that will be added to the XML object this can be used
         *                       to optimize memory allocation a bit.
         */
        XmlObject (int reserved_nodes=4) : is_namespace_default{false}, part{XmlObjPart::all} {
            nodes.reserve (reserved_nodes);
        }

        /**
         * Constructor.
         * This will create an XML object with specified tag name and namespace.
         * <br/><em>Warning:</em> No check is made for valid XML names.
         * @param the_name The tag name of the XML element.
         *                 If the name of the XML element is an empty string, the
         *                 XML object will be treated as an invalid object and will
         *                 have a value of false in boolean expressions and an empty
         *                 string when represented as a string (even if it has valid
         *                 child elements).
         *                 <br/><em>Note:</em> Do not include the namespace in the
         *                 name, use parameter <code>the_namespace</code> instead.
         * @param the_namespace The namespace of the XML element.
         *                      This defines the namespace the object belongs to.
         * @param set_namespace_attr Set the namespace as an 'xmlns' attribute.
         *                           If true, the attribute 'xmlns' will be set with the
         *                           namespace as value. If false, the attribute 'xmlns'
         *                           will not be set.
         * @param namespace_is_default True if the_namespace is the default namespace.
         *                             This will affect how the XML object is represented
         *                             as a string. If the namespace is the default namespace
         *                             it won't be printed as part of the XML tag. But if it
         *                             isn't, it will be printed like: <namespace:name ...
         * @param reserved_nodes The number of reserved child nodes for the XML object.
         *                       If it is known at creation time how may child nodes
         *                       that will be added to the XML object this can be used
         *                       to optimize memory allocation a bit.
         */
        XmlObject (const std::string& the_name,
                   const std::string& the_namespace="",
                   const bool         set_namespace_attr=true,
                   const bool         namespace_is_default=true,
                   const int          reserved_nodes=0)
            :
            name                 {the_name},
            xml_namespace        {the_namespace},
            is_namespace_default {namespace_is_default},
            part                 {XmlObjPart::all}
        {
            nodes.reserve (reserved_nodes);
            if (set_namespace_attr)
                setDefaultNamespaceAttr (the_namespace);
        }

        /**
         * Copy constructor.
         * @param xml_obj The XML object to be copied.
         */
        XmlObject (const XmlObject& xml_obj) {
            name                 = xml_obj.name;
            xml_namespace        = xml_obj.xml_namespace;
            is_namespace_default = xml_obj.is_namespace_default;
            namespace_alias      = xml_obj.namespace_alias;
            default_namespace    = xml_obj.default_namespace;
            attributes           = xml_obj.attributes;
            nodes                = xml_obj.nodes;
            content              = xml_obj.content;
            part                 = xml_obj.part;
        }

        /**
         * Move constructor.
         * Move the object attributes of the input object
         * to this object and set the input object to it's
         * default state.
         * @param xml_obj The XML object to be moved.
         */
        XmlObject (XmlObject&& xml_obj) {
            name                 = std::move (xml_obj.name);
            xml_namespace        = std::move (xml_obj.xml_namespace);
            is_namespace_default = xml_obj.is_namespace_default;
            namespace_alias      = std::move (xml_obj.namespace_alias);
            default_namespace    = std::move (xml_obj.default_namespace);
            attributes           = std::move (xml_obj.attributes);
            nodes                = std::move (xml_obj.nodes);
            content              = std::move (xml_obj.content);
            part                 = xml_obj.part;
            xml_obj.is_namespace_default = false;
            xml_obj.part                 = XmlObjPart::all;
        }

        /**
         * Destructor.
         */
        virtual ~XmlObject () = default;

        /**
         * Assignment operator.
         * @param xml_obj The XML object to be copied.
         * @return A reference to this object.
         */
        XmlObject& operator= (const XmlObject& xml_obj) {
            if (&xml_obj != this) {
                name                 = xml_obj.name;
                xml_namespace        = xml_obj.xml_namespace;
                is_namespace_default = xml_obj.is_namespace_default;
                namespace_alias      = xml_obj.namespace_alias;
                default_namespace    = xml_obj.default_namespace;
                attributes           = xml_obj.attributes;
                nodes                = xml_obj.nodes;
                content              = xml_obj.content;
                part                 = xml_obj.part;
            }
            return *this;
        }

        /**
         * Move operator.
         * Move the object attributes of the input object
         * to this object and set the input object to it's
         * default state.
         * @param xml_obj The XML object to be moved.
         * @return A reference to this object.
         */
        XmlObject& operator= (XmlObject&& xml_obj) {
            name                 = std::move (xml_obj.name);
            xml_namespace        = std::move (xml_obj.xml_namespace);
            is_namespace_default = xml_obj.is_namespace_default;
            namespace_alias      = std::move (xml_obj.namespace_alias);
            default_namespace    = std::move (xml_obj.default_namespace);
            attributes           = std::move (xml_obj.attributes);
            nodes                = std::move (xml_obj.nodes);
            content              = std::move (xml_obj.content);
            part                 = xml_obj.part;
            xml_obj.is_namespace_default = false;
            xml_obj.part                 = XmlObjPart::all;
            return *this;
        }

        /**
         * Convert the XML object into a boolean which is false if the
         * name of the object is an empty string and true otherwise.
         * @return true if the object has a name, false if not.
         */
        operator bool () const {
            return name.length() != 0;
        }

        /**
         * Get the name of the XML object without the namespace prefix.
         * @return The name of the XML object.
         */
        std::string getName () const {
            return name;
        }

        /**
         * Set the name of the XML object.
         * This will set the name of the XML tag for this XML object.
         * @param name The new name of the XML object.
         *             If the name of the XML element is an empty string, the
         *             XML object will be treated as an invalid object and will
         *             have a value of false in boolean expressions and an empty
         *             string when represented as a string (even if it has valid
         *             child elements).
         *             <br/><em>Note:</em> Do not include the namespace in the
         *             name, use method <code>setNamespace</code> instead.
         *             <br/><em>Warning:</em> No check is made for a valid
         *             XML tag name.
         * @return A reference to this object.
         */
        XmlObject& setName (const std::string& name) {
            this->name = name;
            return *this;
        }

        /**
         * Get the namespace of the XML object.
         * @return The namespace this XML object belongs to.
         */
        std::string getNamespace () const {
            return xml_namespace;
        }

        /**
         * Set the namespace of the XML object.
         * @return A reference to this object.
         */
        XmlObject& setNamespace (const std::string& xml_namespace) {
            this->xml_namespace = xml_namespace;
            return *this;
        }

        /**
         * Set the namespace of the XML object.
         * @param xml_namespace The namespace of the XML oject.
         * @param is_default True if this is the current default namespace.
         * @return A reference to this object.
         */
        XmlObject& setNamespace (const std::string& xml_namespace, bool is_default) {
            this->xml_namespace = xml_namespace;
            isNamespaceDefault (is_default);
            return *this;
        }

        /**
         * Return true if this is the default namespace.
         */
        bool isNamespaceDefault () const {
            return is_namespace_default;
        }

        /**
         * Specify if the namespace is also the current default namespace.
         */
        XmlObject& isNamespaceDefault (const bool is_default) {
            is_namespace_default = is_default;
            return *this;
        }

        /**
         * Get the alias attribute(s) for the XML object.
         */
        const std::unordered_map<std::string, std::string>& getNamespaceAlias () const {
            return namespace_alias;
        }

        /**
         * Get the namespace for a specific alias.
         */
        std::string getNamespaceAlias (const std::string& alias) const {
            auto element = namespace_alias.find (alias);
            return element==namespace_alias.end() ? "" : (*element).second;
        }

        /**
         * Add a namespace alias attribute to the XML object.
         * @return A reference to this object.
         */
        XmlObject& addNamespaceAlias (const std::string& alias, const std::string& xml_namespace) {
            namespace_alias[alias] = xml_namespace;
            return *this;
        }

        /**
         * Remove a namespace alias attribute from the XML object.
         * @return A reference to this object.
         */
        XmlObject& removeNamespaceAlias (const std::string& alias) {
            namespace_alias.erase (alias);
            return *this;
        }

        /**
         * Get default namespace attribute of the XML object.
         */
        std::string getDefaultNamespaceAttr () const {
            return default_namespace;
        }

        /**
         * Get default namespace attribute of the XML object.
         * @return A reference to this object.
         */
        XmlObject& setDefaultNamespaceAttr (const std::string& default_namespace) {
            this->default_namespace = default_namespace;
            return *this;
        }

        /**
         * Get the full name of the XML object including the namespace prefix.
         */
        std::string getFullName () const {
            if (xml_namespace.length())
                return xml_namespace + std::string(":") + name;
            /*
            else if (namespace_alias.length())
                return namespace_alias + std::string(":") + name;
            */
            else if (default_namespace.length())
                return default_namespace + std::string(":") + name;
            else
                return name;
        }

        /**
         * Return the value of an attribute.
         * @return The value of an attribute, or an empty
         *         string if the attribute doesn't exits.
         */
        const std::string getAttribute (const std::string& name) const {
            auto value = attributes.find (name);
            return value == attributes.end() ? "" : (*value).second;
        }

        /**
         * Return a reference to the attribute map.
         */
        std::unordered_map<std::string, std::string>& getAttributes () {
            return attributes;
        }

        /**
         * Add or change an attribute of the XML object.
         * @return A reference to this object.
         */
        XmlObject& setAttribute (const std::string& name, const std::string& value="") {
            attributes[name] = value;
            return *this;
        }

        /**
         * Remove a attribute of the XML object.
         * @return A reference to this object.
         */
        XmlObject& removeAttribute (const std::string& name) {
            attributes.erase (name);
            return *this;
        }

        /**
         * Add a child node.
         * @return A reference to this object.
         */
        XmlObject& addNode (const XmlObject& xml_obj) {
            nodes.push_back (xml_obj);
            return *this;
        }

        /**
         * Move a child node to this object.
         * @return A reference to this object.
         */
        XmlObject& addNode (XmlObject&& xml_obj) {
            nodes.push_back (std::move(xml_obj));
            return *this;
        }

        /**
         * Return all child nodes.
         */
        std::vector<XmlObject>& getNodes () {
            return nodes;
        }

        /**
         * Return the first child node with a given name.
         * If no such node exists an empty node will be returned.
         * @param name The name of the child node to find.
         * @param full_name If true, the name includes the full namespace.
         */
        XmlObject getNode (const std::string& name, bool full_name=false) {
            for (auto& node : getNodes()) {
                std::string node_name = full_name ? node.getFullName() : node.getName();
                if (node_name == name)
                    return node;
            }
            return XmlObject (0); // Return an empty object
        }

        /**
         * Return the first child node with a given name space.
         * If no such node exists an empty node will be returned.
         * @param name The name space of the child node to find.
         */
        XmlObject getNsNode (const std::string& name_space) {
            for (auto& node : getNodes()) {
                if (name_space == xml_namespace)
                    return node;
            }
            return XmlObject (0); // Return an empty object
        }

        /**
         * Set the content of the XML object.
         */
        const std::string& getContent () const {
            return content;
        }

        /**
         * Set the content of the XML object.
         * @return A reference to this object.
         */
        XmlObject& setContent (const std::string& content) {
            this->content = content;
            return *this;
        }

        /**
         * Set the content of the XML object.
         * @return A reference to this object.
         */
        XmlObject& setContent (std::string&& content) {
            this->content = std::move (content);
            return *this;
        }

        /**
         * Get the part of the XML object that is to be written when 'to_strin()' is called.
         */
        XmlObjPart getPart () const {
            return part;
        }

        /**
         * Set the part of the XML object that is to be written when 'to_strin()' is called.
         */
        XmlObject& setPart (const XmlObjPart obj_part) {
            part = obj_part;
            return *this;
        }


    protected:

        /**
         * The name of the XML object.
         */
        std::string name;

        /**
         * The namespace of the XML object.
         */
        std::string xml_namespace;

        /**
         * True is the namespace in 'xml_namespace' is the current default namespace.
         * If it is, then the namespace can be omitted when printing the tag name.
         */
        bool is_namespace_default;

        /**
         * Namespace alias(es).
         */
        std::unordered_map <std::string, std::string> namespace_alias;

        /**
         * The default namespace for the XML object and it's children.
         */
        std::string default_namespace;

        /**
         * The attributes of the XML object.
         */
        std::unordered_map<std::string, std::string> attributes;

        /**
         * A list of child XML objects.
         */
        std::vector<XmlObject> nodes;

        /**
         * The content of the XML object.
         */
        std::string content;

        /**
         * This is used to indicate that the XML object may consist of only the start or end tag.
         */
        XmlObjPart part;


        friend std::string to_string (const XmlObject& xml_obj, bool pretty, const std::string& indentation);
    };


    /**
     * Return a string representation of the XML object.
     * @param xml_obj The XML object to be presented as a string.
     * @param pretty_print Print the object in a more human readable form.
     *                     This will introduce new lines and whitespace indentation
     *                     to make the XML object more readable.
     *                     <br/><em>Note:</em> This will introduce data to the XML body.
     *                     Use this only when you want to present the XML object to
     *                     a user in a readable format.
     * @param indentation String used to indent the output. This is only used when
     *                    the parameter pretty is set to true. Every line of output
     *                    will be prepended with this string(normally a number of
     *                    space characters). And each level of child elements will
     *                    have an extra 'indentation' string prepended.
     */
    std::string to_string (const XmlObject& xml_obj, bool pretty=false, const std::string& indentation="");

}


#endif
