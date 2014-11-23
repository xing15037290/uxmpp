/*
 *  Copyright (C) 2014 Ultramarin Design AB <dan@ultramarin.se>
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
#include <uxmpp/Logger.hpp>
#include <uxmpp/Stanza.hpp>
#include <uxmpp/utils.hpp>
#include <random>


#define THIS_FILE "Stanza"


UXMPP_START_NAMESPACE1(uxmpp)


using namespace std;

static std::string default_make_id ();


std::function<std::string (void)> Stanza::make_id = default_make_id;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static std::string default_make_id ()
{
    static random_device rd;
    static default_random_engine re (rd());
    //static uniform_int_distribution<int> dlen (12, 16);  // Random between 12 .. 16
    static uniform_int_distribution<int> d0 (0, 2);  // Random between 0 .. 2
    static uniform_int_distribution<int> d1 (0, 9);  // Random between 0 .. 9
    static uniform_int_distribution<int> d2 (0, 25); // Random between 0 .. 25

    string id = "";
    char ch;
    int len = 16;//dlen (re);

    for (auto i=0; i<len; ++i) {
        switch (d0(re)) {
        case 0:
            ch = '0' + d1(re);
            break;
        case 1:
            ch = 'a' + d2(re);
            break;
        case 2:
            ch = 'A' + d2(re);
            break;
        }
        id += ch;
    }
    return id;
}



UXMPP_END_NAMESPACE1
