#
# Copyright (C) 2013,2014 Ultramarin Design AB <dan@ultramarin.se>
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
lib_LTLIBRARIES = libuxmpp.la

libuxmpp_la_CPPFLAGS = -I$(srcdir)/include -I./include $(expat_CFLAGS) $(libevent_CFLAGS) $(openssl_CFLAGS)
libuxmpp_la_LDFLAGS = $(expat_LIBS) $(libevent_LIBS) $(openssl_LIBS)

libuxmpp_la_CXXFLAGS = -Wall -pipe -std=c++11 -ggdb


# CURRENT : REVISION : AGE
libuxmpp_la_LDFLAGS += -version-info @LIBRARY_VERSION@


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = uxmpp.pc


# Source files
libuxmpp_la_SOURCES  =
#libuxmpp_la_SOURCES += libsource.cpp
libuxmpp_la_SOURCES += uxmpp/Semaphore.cpp
libuxmpp_la_SOURCES += uxmpp/Logger.cpp
libuxmpp_la_SOURCES += uxmpp/UxmppException.cpp
libuxmpp_la_SOURCES += uxmpp/xml/names.cpp
libuxmpp_la_SOURCES += uxmpp/io/Timer.cpp
libuxmpp_la_SOURCES += uxmpp/io/Connection.cpp
libuxmpp_la_SOURCES += uxmpp/io/FileConnection.cpp
libuxmpp_la_SOURCES += uxmpp/io/SocketConnection.cpp
libuxmpp_la_SOURCES += uxmpp/io/ConnectionManager.cpp
libuxmpp_la_SOURCES += uxmpp/io/BsdResolver.cpp
libuxmpp_la_SOURCES += uxmpp/io/IpHostAddr.cpp
libuxmpp_la_SOURCES += uxmpp/utils.cpp
libuxmpp_la_SOURCES += uxmpp/Jid.cpp
libuxmpp_la_SOURCES += uxmpp/XmlObject.cpp
libuxmpp_la_SOURCES += uxmpp/StreamXmlObj.cpp
libuxmpp_la_SOURCES += uxmpp/XmlInputStream.cpp
libuxmpp_la_SOURCES += uxmpp/XmlStream.cpp
libuxmpp_la_SOURCES += uxmpp/StanzaError.cpp
libuxmpp_la_SOURCES += uxmpp/Stanza.cpp
libuxmpp_la_SOURCES += uxmpp/IqStanza.cpp
libuxmpp_la_SOURCES += uxmpp/PresenceStanza.cpp
libuxmpp_la_SOURCES += uxmpp/StreamError.cpp
libuxmpp_la_SOURCES += uxmpp/MessageStanza.cpp
libuxmpp_la_SOURCES += uxmpp/Session.cpp
libuxmpp_la_SOURCES += uxmpp/SessionConfig.cpp
libuxmpp_la_SOURCES += uxmpp/mod/RosterItem.cpp
libuxmpp_la_SOURCES += uxmpp/mod/Roster.cpp
libuxmpp_la_SOURCES += uxmpp/mod/TlsModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/AuthModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/KeepAliveModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/DiscoIdentity.cpp
libuxmpp_la_SOURCES += uxmpp/mod/DiscoInfo.cpp
libuxmpp_la_SOURCES += uxmpp/mod/DiscoModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/RosterModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/PresenceModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/MessageModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/SessionModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/PingModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/PrivateDataModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/RegisterModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/SearchModule.cpp
libuxmpp_la_SOURCES += uxmpp/mod/VcardModule.cpp

# Header files
#libuxmppdir = $(includedir)/uxmpp
libuxmppdir = $(includedir)
nobase_libuxmpp_HEADERS  =
nobase_libuxmpp_HEADERS += uxmpp.hpp
nobase_libuxmpp_HEADERS += uxmpp/uxmpp_config.hpp
nobase_libuxmpp_HEADERS += uxmpp/types.hpp
nobase_libuxmpp_HEADERS += uxmpp/Semaphore.hpp
nobase_libuxmpp_HEADERS += uxmpp/Logger.hpp
nobase_libuxmpp_HEADERS += uxmpp/UxmppException.hpp
nobase_libuxmpp_HEADERS += uxmpp/xml.hpp
nobase_libuxmpp_HEADERS += uxmpp/xml/names.hpp
nobase_libuxmpp_HEADERS += uxmpp/io.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/IoException.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/TimerException.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/Timer.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/Connection.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/FileConnection.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/SocketConnection.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/ConnectionManager.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/io_operation.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/Resolver.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/BsdResolver.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/TlsConfig.hpp
nobase_libuxmpp_HEADERS += uxmpp/io/IpHostAddr.hpp
nobase_libuxmpp_HEADERS += uxmpp/Jid.hpp
nobase_libuxmpp_HEADERS += uxmpp/XmlObject.hpp
nobase_libuxmpp_HEADERS += uxmpp/StreamXmlObj.hpp
nobase_libuxmpp_HEADERS += uxmpp/XmlInputStream.hpp
nobase_libuxmpp_HEADERS += uxmpp/XmlStream.hpp
#nobase_libuxmpp_HEADERS += uxmpp/XmlStreamListener.hpp
nobase_libuxmpp_HEADERS += uxmpp/XmppModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/StanzaError.hpp
nobase_libuxmpp_HEADERS += uxmpp/Stanza.hpp
nobase_libuxmpp_HEADERS += uxmpp/IqStanza.hpp
nobase_libuxmpp_HEADERS += uxmpp/MessageStanza.hpp
nobase_libuxmpp_HEADERS += uxmpp/PresenceStanza.hpp
nobase_libuxmpp_HEADERS += uxmpp/StreamError.hpp
nobase_libuxmpp_HEADERS += uxmpp/SessionState.hpp
nobase_libuxmpp_HEADERS += uxmpp/SessionConfig.hpp
nobase_libuxmpp_HEADERS += uxmpp/SessionListener.hpp
nobase_libuxmpp_HEADERS += uxmpp/Session.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/RosterItem.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/Roster.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/TlsModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/AuthModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/KeepAliveModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/DiscoIdentity.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/DiscoInfo.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/DiscoModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/RosterModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/PresenceModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/MessageModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/SessionModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/PingModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/PrivateDataModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/RegistrationInfo.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/RegisterModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/SearchModule.hpp
nobase_libuxmpp_HEADERS += uxmpp/mod/VcardModule.hpp


# Header files that is not to be installed
noinst_HEADERS  = config.h
noinst_HEADERS += uxmpp/utils.hpp
noinst_HEADERS += uxmpp/io/android_resolver_glue.h
