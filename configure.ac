#
# Copyright (C) 2013-2014 Ultramarin Design AB <dan@ultramarin.se>
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
AC_INIT([libuxmpp], [0.1.0], [dan@ultramarin.se])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2 subdir-objects])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AM_PROG_AR

LT_INIT()
LT_LANG([C++])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS(src/config.h src/uxmpp/uxmpp_config.hpp)
AM_SILENT_RULES([yes])

#
# Library interface version x:y:z
#                           | | |
#                           | | +- AGE - how many previous versions of CURRENT this interface is compatible with.
#                           | |          This is updated when CURRENT is updated.
#                           | |
#                           | +--- REVISION - Update this number when no interface change has been made.
#                           |                 Set this to zero when CURRENT is updated.
#                           |
#                           +----- CURRENT - Update this number when an interface change has been made.
#
# Library version (CURRENT:REVISION:AGE)
#
LIBRARY_VERSION=1:0:0
AC_SUBST([LIBRARY_VERSION])

#
# Libraries to be listed in uxmpp.pc
#
LIBS_IN_PC_FILE=

#
# Required packages to be listed in uxmpp.pc
#
REQUIRE_IN_PC_FILE=

#
# Defines in the doxygen config file
#
PREDEFINED_IN_DOXYGEN=

#
# Check for expat
#
PKG_CHECK_MODULES([expat],
	[expat >= 2.1.0],
	[REQUIRE_IN_PC_FILE+=expat],
	[AC_MSG_ERROR(Could not find expat)])

##
## Check for libgsasl
##
#PKG_CHECK_MODULES([libgsasl],
#	[libgsasl >= 1.8.0],
#	[REQUIRE_IN_PC_FILE+=" libgsasl"],
#	[AC_MSG_ERROR(Could not find libgsasl)])

#
# Check for openssl
#
PKG_CHECK_MODULES([openssl],
	[openssl >= 1.0.1],
	[REQUIRE_IN_PC_FILE+=" openssl"],
	[AC_MSG_ERROR(Could not find openssl)])

#
# Check for libevent
#
PKG_CHECK_MODULES([libevent],
	[libevent >= 2.0.0],
	[REQUIRE_IN_PC_FILE+=" libevent"],
	[AC_MSG_ERROR(Could not find libevent)])

#
# Check for libresolv
#
#LIBS+=-lresolv
#LIBS_IN_PC_FILE+=-lresolv
AC_SEARCH_LIBS([res_query], [resolv], [
			    AC_DEFINE([UXMPP_HAVE_BSD_RESOLVER],[1],[Define to 1 if res_query() is available])
			    ],
			    [AC_SEARCH_LIBS([__res_query], [resolv], [
			     AC_DEFINE([UXMPP_HAVE_BSD_RESOLVER],[1],[Define to 1 if res_query() is available])
			     ])
			    ])


#
# Give the user an option to not build test applications
#
AC_ARG_ENABLE([testapps],
	[AS_HELP_STRING([--disable-testapps],
	[don't build test applications])])
AM_CONDITIONAL([ENABLE_TESTAPPS_SET], [test "x$enable_testapps" != "xno"])



#
# All libraries are added
#
AC_SUBST([LIBS_IN_PC_FILE])
AC_SUBST([REQUIRE_IN_PC_FILE])
AC_SUBST([PREDEFINED_IN_DOXYGEN])


#
# Give the user an option to not build doxygen documentation
#
AC_ARG_ENABLE([doxygen],
	[AS_HELP_STRING([--disable-doxygen],
	[don't build doxygen documentation])])
AM_CONDITIONAL([ENABLE_DOXYGEN_SET], [test "x$enable_doxygen" != "xno"])


AM_COND_IF([ENABLE_DOXYGEN_SET],
	[
	#
	# Check for doxygen support
	#
	AC_CHECK_PROGS([DOXYGEN], [doxygen])
	if test -z "$DOXYGEN";
	   then AC_MSG_WARN([doxygen not found - continuing without doxygen support])
	fi
	AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
	AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxygen.cfg])])

	#
	# Check if doxygen can use dot
	#
	AC_CHECK_PROGS([DOT], [dot])
	if test -z "$DOT";
	   then AC_MSG_WARN([dot not found - continuing without doxygen dot support])
	fi
	AM_CONDITIONAL([HAVE_DOT], [test -n "$DOT"])
	AM_COND_IF([HAVE_DOT], [DOXYGEN_HAVE_DOT=YES], [DOXYGEN_HAVE_DOT=NO])
	AM_COND_IF([HAVE_DOT], [DOXYGEN_HAVE_CLASS_GRAPH=NO], [DOXYGEN_HAVE_CLASS_GRAPH=YES])
	AC_SUBST([DOXYGEN_HAVE_DOT])
	AC_SUBST([DOXYGEN_HAVE_CLASS_GRAPH])
	],
	[
	AM_CONDITIONAL([HAVE_DOXYGEN], false)
	AM_CONDITIONAL([HAVE_DOT], false)
	]
)


AC_OUTPUT([
	Makefile
	src/Makefile
	src/uxmpp.pc
	test/Makefile
	doc/Makefile
])
