#
# Copyright (C) 2013 Ultramarin Design AB <dan@ultramarin.se>
#
# This file is part of uxmpp.
#
# uxmpp is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
if ENABLE_TESTAPPS_SET

AM_CXXFLAGS  = -Wall -pipe -std=c++11 -ggdb
AM_CXXFLAGS += -I$(srcdir)/../src -I../src $(expat_CFLAGS) $(libevent_CFLAGS) $(openssl_CFLAGS)
AM_LDFLAGS = -L../src -luxmpp $(expat_LIBS) $(libevent_LIBS) $(openssl_LIBS) -lrt -lpthread


noinst_bindir =
noinst_bin_PROGRAMS =

#	bindir =
bin_PROGRAMS =

#noinst_bin_PROGRAMS += test_prog
#test_prog_SOURCES    = test_prog.cpp

noinst_bin_PROGRAMS += test_Logger
test_Logger_SOURCES  = test_Logger.cpp

noinst_bin_PROGRAMS     += test_IpHostAddr
test_IpHostAddr_SOURCES  = test_IpHostAddr.cpp

noinst_bin_PROGRAMS   += test_Resolver
test_Resolver_SOURCES  = test_Resolver.cpp

noinst_bin_PROGRAMS    += test_XmlObject
test_XmlObject_SOURCES  = test_XmlObject.cpp

noinst_bin_PROGRAMS    += test_XmlStream
test_XmlStream_SOURCES  = test_XmlStream.cpp

noinst_bin_PROGRAMS += test_Jid
test_Jid_SOURCES     = test_Jid.cpp

noinst_bin_PROGRAMS  += test_Session
test_Session_SOURCES  = test_Session.cpp

noinst_bin_PROGRAMS     += test_ImpSession
test_ImpSession_SOURCES  = test_ImpSession.cpp

noinst_bin_PROGRAMS     += test_Timer
test_Timer_SOURCES  = test_Timer.cpp

noinst_bin_PROGRAMS     += test_cores
test_cores_SOURCES  = test_cores.cpp

noinst_bin_PROGRAMS     += uxmpp
uxmpp_SOURCES  = uxmpp.cpp

endif
