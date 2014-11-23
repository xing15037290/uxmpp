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
#ifndef UXMPP_STREAMERROR_HPP
#define UXMPP_STREAMERROR_HPP

#include <uxmpp/types.hpp>
#include <uxmpp/XmlObject.hpp>
#include <uxmpp/XmlNames.hpp>
#include <string>


namespace uxmpp {


    /**
     * Stream error object.
     */
    class StreamError : public XmlObject {
    public:

        /**
         * Default Constructor.
         */
        StreamError () : XmlObject ("error", XmlStreamNsAlias, false, false, 2) { }

        /**
         * Construct a stream error from an XmlObject.
         */
        StreamError (XmlObject& xml_obj) : XmlObject ("error", XmlStreamNsAlias, false, false, 2) {
            set_error_name (xml_obj.get_nodes().empty() ? "" : xml_obj.get_nodes().begin()->get_tag_name());
            for (auto& node : xml_obj.get_nodes()) {
                if (node.get_tag_name() == "text") {
                    set_text (node.get_content());
                    break;
                }
            }
        }

        /**
         * Constructor.
         */
        StreamError (const std::string& error) : XmlObject ("error", XmlStreamNsAlias, false, false, 2) {
            set_error_name (error);
        }

        /**
         * Destructor.
         */
        virtual ~StreamError () = default;

        /**
         * Assignment operator.
         */
        StreamError& operator= (const XmlObject& xml_obj) {
            if (&xml_obj == this)
                return *this;

            XmlObject& obj = const_cast<XmlObject&> (xml_obj);
            set_error_name (obj.get_nodes().empty() ? "": obj.get_nodes().begin()->get_tag_name());
            for (auto& node : obj.get_nodes()) {
                if (node.get_tag_name() == "text") {
                    set_text (node.get_content());
                    break;
                }
            }
            return *this;
        }

        /**
         * Is there an error message available.
         */
        const bool have_error () {
            return get_nodes().size() != 0;
        }

        /**
         * Set the error condition.
         */
        void set_error_name (const std::string& error) {
            get_nodes().clear ();
            if (error.length())
                add_node (XmlObject(error, XmlStreamErrortypeNs));
        }

        /**
         * Return the error condition.
         */
        std::string get_error_name () {
            return get_nodes().empty() ? "": get_nodes().begin()->get_tag_name();
        }

        /**
         * Set the application specific condition.
         */
        void set_app_error (const std::string& app_error, const std::string& text="") {
            set_error_name ("undefined-condition");
            add_node (XmlObject(app_error, XmlUxmppErrorNs));
            set_text (text);
        }

        /**
         * Return the application specific error condition, if any.
         */
        std::string get_app_error () {
            auto& nodes = get_nodes ();
            for (unsigned i=1; i<nodes.size(); ++i) {
                if (nodes[i].get_tag_name() != "text")
                    return nodes[i].get_tag_name ();
            }
            return "";
        }

        /**
         * Return a descriptive error text (if any).
         */
        std::string get_text () {
            for (auto& node : get_nodes()) {
                if (node.get_tag_name() == "text")
                    return node.get_content ();
            }
            return "";
        }


        /**
         * Set a descriptive error text.
         */
        void set_text (const std::string& text) {
            for (auto& node : get_nodes()) {
                if (node.get_tag_name() == "text") {
                    node.set_content (text);
                    return;
                }
            }
            XmlObject text_node ("text", XmlStreamErrortypeNs);
            text_node.set_content (text);
            add_node (text_node);
        }

    };


}


#endif
