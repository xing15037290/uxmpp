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
    cout << "Default constructor" << endl;
    XmlObject tc1;
    cout << "to_string(): " << to_string(tc1) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\")" << endl;
    XmlObject tc2 ("name");
    cout << "to_string(): " << to_string(tc2) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\")" << endl;
    XmlObject tc3 ("name", "namespace");
    cout << "to_string(): " << to_string(tc3) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\", false)" << endl;
    XmlObject tc4 ("name", "namespace", false);
    cout << "to_string(): " << to_string(tc4) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\", false, false)" << endl;
    XmlObject tc5 ("name", "namespace", false, false);
    cout << "to_string(): " << to_string(tc5) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\", true, false)" << endl;
    XmlObject tc6 ("name", "namespace", true, false);
    cout << "to_string(): " << to_string(tc6) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\")" << endl;
    XmlObject tc7 ("name", "namespace");
    cout << "to_string(): " << to_string(tc7) << endl;
    cout << "setDefaultNamespaceAttr(\"nsattr\")" << endl;
    tc7.setDefaultNamespaceAttr ("def_ns");
    cout << "to_string(): " << to_string(tc7) << endl;
    cout << "isNamespaceDefault(false)" << endl;
    tc7.isNamespaceDefault (false);
    cout << "to_string(): " << to_string(tc7) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"a\", false, false)" << endl;
    cout << "addNamespaceAlias (\"a\", \"namespace\")" << endl;
    XmlObject tc8 ("name", "a", false, false);
    tc8.addNamespaceAlias ("a", "namespace");
    cout << "to_string(): " << to_string(tc8) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\")" << endl;
    XmlObject tc9 ("name", "namespace");
    cout << "setAttribute(\"attrname\", \"attrvalue\")" << endl;
    tc9.setAttribute ("attrname", "attrvalue");
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "addNode(XmlObject(\"<element>\", \"namespace\", false).setAttribute(\"some_attr\", \"some_value\"))" << endl;
    tc9.addNode (XmlObject("<element>", "namespace", false).setAttribute("some_attr", "some_value"));
    cout << "to_string(): " << to_string(tc9) << endl;
    cout << "to_string(true): " << to_string(tc9, true) << endl;

    cout << "addNode(XmlObject(\"child\", \"namespace\", false).setAttribute(\"num\", \"two\").addNode(XmlObject(\"body\", \"namespace\", false).setAttribute(\"att\", \"val\").setContent(\"A message\")))" << endl;

    tc9.addNode (XmlObject("child", "namespace", false).setAttribute("num", "two")
                 .addNode(XmlObject("body", "namespace", false)
                          .setAttribute("att", "val").setContent("A message")));
    cout << "to_string(): " << to_string(tc9) << endl;
    cout << "to_string(true): " << to_string(tc9, true) << endl;

    cout << "setPart(XmlObjPart::start)" << endl;
    tc9.setPart (XmlObjPart::start);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "setPart(XmlObjPart::body)" << endl;
    tc9.setPart (XmlObjPart::body);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "setPart(XmlObjPart::end)" << endl;
    tc9.setPart (XmlObjPart::end);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << endl;
    cout << "Constructor: XmlObject xobj;" << endl;
    XmlObject xobj;
    cout << "xobj == true : " << (xobj==true) << endl;
    cout << "xobj == false: " << (xobj==false) << endl;
    cout << "xobj.setName(\"name\") " << endl;
    xobj.setTagName ("name");
    cout << "xobj == true : " << (xobj==true) << endl;
    cout << "xobj == false: " << (xobj==false) << endl;

    return 0;
}
