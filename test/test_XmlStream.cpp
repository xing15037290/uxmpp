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
#include <uxmpp.hpp>
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace std;
using namespace uxmpp;
using namespace uxmpp::io;


static void rx_xml (XmlStream& stream, XmlObject& xml_obj)
{
    if (xml_obj.get_namespace() == XmlUxmppInternalErrorNs) {
        if (xml_obj.get_tag_name() == "parse-error") {
            uxmpp_log_error ("main", "XML parse error: ", xml_obj.get_content());
        }
        else if (xml_obj.get_tag_name() == "rx-error") {
            uxmpp_log_error ("main", "RX faliure: ", xml_obj.get_attribute("errnum"));
            //stream.stop ();
        }
        else {
            uxmpp_log_error ("main", "RX faliure: ", xml_obj.get_tag_name());
        }
        //stream.stop ();
    }
    else if (xml_obj.get_namespace() == XmlUxmppInternalTimerNs) {
        if (xml_obj.get_tag_name() == "timeout") {
            uxmpp_log_info ("main", "Got timeout: ", xml_obj.get_attribute("name"));
        }
        stream.write (xml_obj);
        stream.stop ();
    }else{
        //stream.set_timeout ("stop", 500);
        //uxmpp_log_info ("main", "Receivec xml: ", to_string(xml_obj));
        stream.write (xml_obj);
    }
}


int main (int argc, char* argv[])
{

    uxmpp_set_log_level (LogLevel::debug);

    if (argc < 4) {
        cerr << "Usage: test_XmlStream <xml_input_file> <xml_output_file> <top_node_name> [top_node_namespace]" << endl;
        return 1;
    }

    FileConnection in_file (argv[1], O_RDONLY);
    FileConnection out_file (argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG);
    XmlStream xs (XmlObject(argv[3], argc>4 ? argv[4] : ""));

    xs.set_rx_cb (rx_xml);
    xs.set_timeout ("stop", 800);
    xs.run (in_file, out_file);

    cerr << "done" << endl;

    return 0;
}
