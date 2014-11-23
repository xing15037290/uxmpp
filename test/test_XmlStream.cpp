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
using namespace uxmpp::net;


class XmlHandler : public XmlStreamListener {
public:
    virtual void on_rx_xml_obj (XmlStream& stream, XmlObject& xml_obj) override {
        cout << endl << "Got RX: ";
        if (xml_obj.get_part() == XmlObjPart::start) {
            cout << "(start-tag) ";
        }
        else if (xml_obj.get_part() == XmlObjPart::end) {
            cout << "(end-tag) ";
        }

        cout << xml_obj.get_tag_name() << endl;


        if (xml_obj.get_tag_name() == "error") {
            cout << "Got error, close stream" << endl;
            stream.stop ();
        }

        if (xml_obj.get_tag_name()=="stream" && xml_obj.get_part() == XmlObjPart::end) {
            cout << "Got EOS, close stream" << endl;
            stream.stop ();
        }

        if (xml_obj.get_tag_name() == "features") {
            cout << "Got features, close stream" << endl;
            stream.stop ();
        }

    }

    virtual void on_open (XmlStream& xs) override {
        cout << "Stream opened" << endl;
        //StreamXmlObj stream_start ("ultramarin.se", "dan@ultramarin.se");
/*
        XmlObject stream_start ("stream:stream");
        stream_start.setAttribute ("from", "dan@ultramarin.se");
        stream_start.setAttribute ("to", "ultramarin.se");
        stream_start.setAttribute ("version", "1.0");
        stream_start.setAttribute ("xml:lang", "en");
        //stream_start.setAttribute ("xmlns", "'jabber:client'");
        //stream_start.setAttribute ("xmlns:stream", "'http://etherx.jabber.org/streams'");
        stream_start.setPart (XmlObjPart::start);
*/
        //xs.write (stream_start);

        //xs.stop ();
        std::thread ([&xs](){
                cerr << "Stop the stream from a thread" << endl;
                xs.stop ();
            }).detach ();
    }

    virtual void on_close (XmlStream& stream) override {
        cout << "Stream closed" << endl;
/*
        BsdResolver resolver;
        auto addr_list = resolver.lookup_srv ("ultramarin.se", AddrProto::tcp, "xmpp-client");
        bool connected = false;
        for (auto addr : addr_list) {
            cout << "Open it again on " << to_string(addr) << endl;
            connected = stream.start (addr);
            if (connected) {
                cout << "Failed to open it again!" << endl;
                break;
            }
        }
*/
    }

    virtual void on_rx_xml_error (XmlStream& stream) override {
    }
};


int main (int argc, char* argv[])
{
    uxmpp_set_log_level (LogLevel::trace);

    if (argc < 2) {
        cerr << "Usage: test_Resolver <domain>" << endl;
        return 1;
    }

    BsdResolver resolver;
    auto addr_list = resolver.lookup_srv (argv[1], AddrProto::tcp, "xmpp-client");
    //auto addr_list = resolver.lookup_host (argv[1]);

    XmlStream xs (XmlObject("stream", "http://etherx.jabber.org/streams", false, false));
    XmlHandler xh;

    xs.add_listener (xh);

    for (int i=0; i<128; i++) {
        bool connected = false;
        for (auto addr : addr_list) {
            connected = xs.start (addr);
            if (!connected) {
                cerr << "Failed to connect to " << to_string(addr) << endl;
                break;
            }
        }
        //cout << "#" << i << endl;
        cerr << "#" << i << endl;
    }

/*
    XmlObject stream_start ("stream:stream");
    stream_start.setAttribute ("from", "dan@ultramarin.se");
    stream_start.setAttribute ("to", "ultramarin.se");
    stream_start.setAttribute ("version", "1.0");
    stream_start.setAttribute ("xml:lang", "en");
    //stream_start.setAttribute ("xmlns", "'jabber:client'");
    //stream_start.setAttribute ("xmlns:stream", "'http://etherx.jabber.org/streams'");
    stream_start.setPart (XmlObjPart::start);

    xs.write (stream_start);

    sleep (10);
*/
    //sleep (10);

    return 0;
}
