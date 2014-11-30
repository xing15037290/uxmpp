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
#ifndef UXMPP_UTILS_HPP
#define UXMPP_UTILS_HPP

#include <uxmpp/types.hpp>
#include <string>


namespace uxmpp {


    /**
     *
     */
    std::string base64_encode (const std::string& bindata);


    /**
     *
     */
    std::string base64_decode (const std::string& ascdata);


    /**
     *
     */
    bool block_signal (int signal_number);


    /**
     *
     */
    bool unblock_signal (int signal_number);


    /**
     *
     */
    unsigned long get_thread_id ();

}


#endif
