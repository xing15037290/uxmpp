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
            setErrorName (xml_obj.getNodes().size() ? xml_obj.getNodes()[0].getTagName() : "");
            for (auto& node : xml_obj.getNodes()) {
                if (node.getTagName() == "text") {
                    setText (node.getContent());
                    break;
                }
            }
        }

        /**
         * Constructor.
         */
        StreamError (const std::string& error) : XmlObject ("error", XmlStreamNsAlias, false, false, 2) {
            setErrorName (error);
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
            setErrorName (obj.getNodes().size() ? obj.getNodes()[0].getTagName() : "");
            for (auto& node : obj.getNodes()) {
                if (node.getTagName() == "text") {
                    setText (node.getContent());
                    break;
                }
            }
            return *this;
        }

        /**
         * Is there an error message available.
         */
        const bool haveError () {
            return getNodes().size() != 0;
        }

        /**
         * Set the error condition.
         */
        void setErrorName (const std::string& error) {
            getNodes().clear ();
            if (error.length())
                addNode (XmlObject(error, XmlStreamErrortypeNs));
        }

        /**
         * Return the error condition.
         */
        std::string getErrorName () {
            return getNodes().size() ? getNodes()[0].getTagName() : "";
        }

        /**
         * Set the application specific condition.
         */
        void setAppError (const std::string& app_error, const std::string& text="") {
            setErrorName ("undefined-condition");
            addNode (XmlObject(app_error, XmlUxmppErrorNs));
            setText (text);
        }

        /**
         * Return the application specific error condition, if any.
         */
        std::string getAppError () {
            auto& nodes = getNodes ();
            for (unsigned i=1; i<nodes.size(); i++) {
                if (nodes[i].getTagName() != "text")
                    return nodes[i].getTagName ();
            }
            return "";
        }

        /**
         * Return a descriptive error text (if any).
         */
        std::string getText () {
            for (auto& node : getNodes()) {
                if (node.getTagName() == "text")
                    return node.getContent ();
            }
            return "";
        }


        /**
         * Set a descriptive error text.
         */
        void setText (const std::string& text) {
            for (auto& node : getNodes()) {
                if (node.getTagName() == "text") {
                    node.setContent (text);
                    return;
                }
            }
            XmlObject text_node ("text", XmlStreamErrortypeNs);
            text_node.setContent (text);
            addNode (text_node);
        }

    };


}


#endif
