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
        XmlObject (int reserved_nodes=4);

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
                   const int          reserved_nodes=0);

        /**
         * Copy constructor.
         * @param xml_obj The XML object to be copied.
         */
        XmlObject (const XmlObject& xml_obj);

        /**
         * Move constructor.
         * Move the object attributes of the input object
         * to this object and set the input object to it's
         * default state.
         * @param xml_obj The XML object to be moved.
         */
        XmlObject (XmlObject&& xml_obj);

        /**
         * Destructor.
         */
        virtual ~XmlObject () = default;

        /**
         * Assignment operator.
         * @param xml_obj The XML object to be copied.
         * @return A reference to this object.
         */
        XmlObject& operator= (const XmlObject& xml_obj);

        /**
         * Move operator.
         * Move the object attributes of the input object
         * to this object and set the input object to it's
         * default state.
         * @param xml_obj The XML object to be moved.
         * @return A reference to this object.
         */
        XmlObject& operator= (XmlObject&& xml_obj);

        /**
         * Convert the XML object into a boolean which is false if the
         * name of the object is an empty string and true otherwise.
         * @return true if the object has a name, false if not.
         */
        operator bool () const {
            return !tag_name.empty ();
        }

        /**
         * Get the tag name of the XML object without the namespace prefix.
         * @return The tag name of the XML object.
         */
        std::string get_tag_name () const;

        /**
         * Set the tag name of the XML object.
         * This will set the tag name of the XML tag for this XML object.
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
        XmlObject& set_tag_name (const std::string& name);

        /**
         * Get the namespace of the XML object.
         * @return The namespace this XML object belongs to.
         */
        std::string get_namespace () const;

        /**
         * Set the namespace of the XML object.
         * @param xml_namespace The namespace of the XML oject.
         *                      <br/><em>Warning:</em> No check is made
         *                      for a valid XML name.
         * @return A reference to this object.
         */
        XmlObject& set_namespace (const std::string& xml_namespace);

        /**
         * Set the namespace of the XML object.
         * @param xml_namespace The namespace of the XML oject.
         *                      <br/><em>Warning:</em> No check is made
         *                      for a valid XML name.
         * @param is_default True if this should be the current default namespace.
         * @return A reference to this object.
         */
        XmlObject& set_namespace (const std::string& xml_namespace, bool is_default);

        /**
         * Check if the namespace is also the default namespace.
         * @return true if the namespace is also the default namespace.
         */
        bool is_namespace_default () const;

        /**
         * Specify if the namespace is also the current default namespace.
         * @param is_default True if this should be the current default namespace.
         * @return A reference to this object.
         */
        XmlObject& is_namespace_default (const bool is_default);

        /**
         * Get the alias attribute(s) for the XML object.
         * A namespace alias is defined by an atribute of the form xmlns:alias='namespace'.
         * This method will return a map of all aliases and their respective namespace.
         * @return A map with aliases and the namespaces they represent.
         */
        const std::unordered_map<std::string, std::string>& get_namespace_alias () const;

        /**
         * Get the namespace for a specific alias.
         * Given an alias name, find and return the namespace it represent.
         * If no such alias if found and empty string is returned.
         * @param alias The namespace alias name.
         * @return The namespace the alias represent,
         *         or an empty string if no such alias exist.
         */
        std::string get_namespace_alias (const std::string& alias) const;

        /**
         * Add a namespace alias attribute to the XML object. When converted to
         * a string this namespace alias will be printed as an attribute of the
         * form: <code>xmlns:alias='xml_namespace'</code>.
         * <br/><em>Note:</em> no check is made to see if the parameters
         * are valid XML names.
         * @param alias The alias name.
         * @param xml_namespace The namespace that the alias represent.
         * @return A reference to this object.
         */
        XmlObject& add_namespace_alias (const std::string& alias, const std::string& xml_namespace);

        /**
         * Remove a namespace alias attribute from the XML object.
         * @param alias The name of the alias to be removed from the XML object.
         * @return A reference to this object.
         */
        XmlObject& remove_namespace_alias (const std::string& alias);

        /**
         * Get the default namespace attribute of the XML object.
         * This will return the namespace that is defined by the xmlns attribute,
         * or an empty string of the attribute isn't present.
         * @return The value of the xmlns attribute,
         *         or an empty string if it isn't present.
         */
        std::string get_default_namespace_attr () const;

        /**
         * Get default namespace attribute of the XML object.
         * @return A reference to this object.
         */
        XmlObject& set_default_namespace_attr (const std::string& default_namespace);

        /**
         * Get the full name of the XML object including the namespace prefix.
         * This will return the complete name of the xml tag, including the namespace prefix.
         * It will not, however, translate namespace aliases into namespaces.
         * @return The complete XML tag name.
         */
        std::string get_full_name () const;

        /**
         * Return the value of an attribute.
         * @return The value of an attribute, or an empty
         *         string if the attribute doesn't exits.
         */
        const std::string get_attribute (const std::string& name) const;

        /**
         * Return a reference to the attribute map.
         * This will return the complete map that maps attribute names to
         * attribute values.
         * @return A map of attribute names and attribute values.
         */
        std::unordered_map<std::string, std::string>& get_attributes ();

        /**
         * Add or change an attribute of the XML object.
         * If the attribute is already present, this will change it's value,
         * if it is not present it will be added.
         * @param name The name of the attribute that will be set.
         * @param value The value of the attribute. This can be an empty string.
         * @return A reference to this object.
         */
        XmlObject& set_attribute (const std::string& name, const std::string& value="");

        /**
         * Remove a attribute of the XML object.
         * @param name The name of the attribute to remove.
         * @return A reference to this object.
         */
        XmlObject& remove_attribute (const std::string& name);

        /**
         * Add a child node.
         * This will add an XML object as a child element to this element.
         * @param xml_obj The XML object to be added.
         * @return A reference to this object.
         */
        XmlObject& add_node (const XmlObject& xml_obj);

        /**
         * Move a child node to this object.
         * This will add an XML object as a child element to this element.
         * @param xml_obj The XML object to be moved to as a child of this object.
         * @return A reference to this object.
         */
        XmlObject& add_node (XmlObject&& xml_obj);

        /**
         * Return all child nodes.
         * @return A reference to the list of all child elements.
         */
        std::vector<XmlObject>& get_nodes ();

        /**
         * Return the first child node with a given name.
         * If no such node exists an empty node will be returned.
         * @param name The name of the child node to find.
         * @param full_name If true, the <code>name</code> parameter
         *        is assumed to be the full name including the namespace.
         *        If false, only the name without the namespace will be compared.
         * @return An copy of the child XML object to be found, or an empty
         *         XML object. The resulting object can be tested in a
         *         boolean expression and if false, no child node was found.
         */
        XmlObject get_node (const std::string& name, bool full_name=false);

        /**
         * Return the first child node with a given name space.
         * If no such node exists an empty node will be returned.
         * @param name The namespace of the child node to find.
         * @return The first XML child element found with the
         *         given namespace. If no such element is found,
         *         an empty XML object will be returned.
         *         The result can be tested in a boolean expression
         *         and if false, no element was found.
         */
        XmlObject get_ns_node (const std::string& name_space);

        /**
         * Set the content of the XML object.
         * @return The content of the XML object.
         */
        const std::string& get_content () const;

        /**
         * Set the content of the XML object. If the XML object
         * already have content it will be replaced.
         * @param content The content to be set.
         * @return A reference to this object.
         */
        XmlObject& set_content (const std::string& content);

        /**
         * Set the content of the XML object. If the XML object
         * already have content it will be replaced.
         * @param content The content to be move to this object.
         * @return A reference to this object.
         */
        XmlObject& set_content (std::string&& content);

        /**
         * Set the part of the XML object that is to be written
         * when <code>to_string()</code> is called.
         * @param obj_part The specific part of the XML object
         *                 that will be written to a string whenever
         *                 <code>to_string()</code> is called.
         */
        XmlObject& set_part (const XmlObjPart obj_part);

        /**
         * Get the part of the XML object that is to be written when 'to_string()' is called.
         * @return The specific part of the XML object
         *         that will be written to a string whenever
         *         <code>to_string()</code> is called.
         */
        XmlObjPart get_part () const;


    protected:

        /**
         * The tag name of the XML object.
         */
        std::string tag_name;

        /**
         * The namespace of the XML object.
         */
        std::string xml_namespace;

        /**
         * True is the namespace in 'xml_namespace' is the current default namespace.
         * If it is, then the namespace can be omitted when printing the tag name.
         */
        bool namespace_is_default;

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
     *                    the parameter <code>pretty_print</code> is set to true.
     *                    Every line of output will be prepended with this string(normally
     *                    a number of space characters). And each level of child elements
     *                    will have an extra 'indentation' string prepended.
     */
    std::string to_string (const XmlObject& xml_obj, bool pretty=false, const std::string& indentation="");

}


#endif
