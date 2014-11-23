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
    cout << "set_default_namespace_attr(\"nsattr\")" << endl;
    tc7.set_default_namespace_attr ("def_ns");
    cout << "to_string(): " << to_string(tc7) << endl;
    cout << "is_namespace_default(false)" << endl;
    tc7.is_namespace_default (false);
    cout << "to_string(): " << to_string(tc7) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"a\", false, false)" << endl;
    cout << "add_namespace_alias (\"a\", \"namespace\")" << endl;
    XmlObject tc8 ("name", "a", false, false);
    tc8.add_namespace_alias ("a", "namespace");
    cout << "to_string(): " << to_string(tc8) << endl;

    cout << endl;
    cout << "Constructor: XmlObject(\"name\", \"namespace\")" << endl;
    XmlObject tc9 ("name", "namespace");
    cout << "set_attribute(\"attrname\", \"attrvalue\")" << endl;
    tc9.set_attribute ("attrname", "attrvalue");
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "add_node(XmlObject(\"<element>\", \"namespace\", false).set_attribute(\"some_attr\", \"some_value\"))" << endl;
    tc9.add_node (XmlObject("<element>", "namespace", false).set_attribute("some_attr", "some_value"));
    cout << "to_string(): " << to_string(tc9) << endl;
    cout << "to_string(true): " << to_string(tc9, true) << endl;

    cout << "add_node(XmlObject(\"child\", \"namespace\", false).set_attribute(\"num\", \"two\").add_node(XmlObject(\"body\", \"namespace\", false).set_attribute(\"att\", \"val\").set_content(\"A message\")))" << endl;

    tc9.add_node (XmlObject("child", "namespace", false).set_attribute("num", "two")
                  .add_node(XmlObject("body", "namespace", false)
                            .set_attribute("att", "val").set_content("A message")));
    cout << "to_string(): " << to_string(tc9) << endl;
    cout << "to_string(true): " << to_string(tc9, true) << endl;

    cout << "set_part(XmlObjPart::start)" << endl;
    tc9.set_part (XmlObjPart::start);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "set_part(XmlObjPart::body)" << endl;
    tc9.set_part (XmlObjPart::body);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << "set_part(XmlObjPart::end)" << endl;
    tc9.set_part (XmlObjPart::end);
    cout << "to_string(): " << to_string(tc9) << endl;

    cout << endl;
    cout << "Constructor: XmlObject xobj;" << endl;
    XmlObject xobj;
    cout << "xobj == true : " << (xobj==true) << endl;
    cout << "xobj == false: " << (xobj==false) << endl;
    cout << "xobj.set_tag_name(\"name\") " << endl;
    xobj.set_tag_name ("name");
    cout << "xobj == true : " << (xobj==true) << endl;
    cout << "xobj == false: " << (xobj==false) << endl;

    return 0;
}
