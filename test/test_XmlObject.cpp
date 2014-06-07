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

using namespace std;
using namespace uxmpp;


int main (int argc, char* argv[])
{
    //XmlObject top ("stream", "http://etherx.jabber.org/streams", "", "jabber:client");

    XmlObject top;

    top.setName ("stream");
    //top.setNamespace ("http://etherx.jabber.org/streams");
    //top.setNamespace ("http://etherx.jabber.org/streams");
    //top.setNamespaceAlias ("stream");
    top.setDefaultNamespaceAttr ("jabber:client");
    top.setNamespace ("trallallalal", true);
    top.addNamespaceAlias ("hej", "a_long_namespace_that_has_an_alias");

    top.setAttribute ("from", "dan@ultramarin.se");
    top.setAttribute ("to", "ultramarin.se");
    top.setAttribute ("version", "1.0");

    XmlObject node ("child");
    node.setAttribute ("hej", "true");
    node.setContent ("blablabla");

    top.addNode (node);


    top.addNode (XmlObject("rum", "hej").setAttribute("type", "kök").setContent("rymligt"));

//    top.setPart (XmlObjPart::start);
//    cout << to_string(top) << endl;

//    top.setPart (XmlObjPart::end);
//    cout << to_string(top) << endl;

    top.setPart (XmlObjPart::all);
    cout << to_string(top) << endl;

    XmlObject timer_obj (XmlUxmppTimerTag, XmlUxmppTimerNs);
    timer_obj.setAttribute ("id", "timer-id");
    cout << to_string(timer_obj) << endl;

    return 0;
}
