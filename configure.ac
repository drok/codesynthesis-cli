# file      : configure.ac
# license   : GNU GPL v2; see accompanying LICENSE file

AC_PREREQ(2.60)
AC_INIT([cli], [1.2.0-b.9.1], [cli-users@codesynthesis.com])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([cli/cli/cli.cxx])

AC_DEFINE([CLI_VERSION], [1], [Major version number])
AC_DEFINE_UNQUOTED([CLI_VERSION_STR], ["$PACKAGE_VERSION"], [Full version string])
AC_DEFINE([CLI_VERSION_ID], [100002999995091], [Version ID, see definition in version.hxx])
AC_DEFINE([CLI_VERSION_MAJOR], [1], [Major])
AC_DEFINE([CLI_VERSION_MINOR], [2], [Minor])
AC_DEFINE([CLI_VERSION_PATCH], [0], [Patch])
AC_DEFINE([CLI_PRE_RELEASE], [9], [Pre-release])
AC_DEFINE([CLI_COPYRIGHT], ["2009-2023 Code Synthesis Tools CC"], [Copyright])
AC_DEFINE_UNQUOTED([CLI_VERSION_FULL], ["$PACKAGE_STRING"], [Full version string])


AM_INIT_AUTOMAKE([-Wall -Werror foreign nostdinc subdir-objects dist-bzip2 dist-zip tar-ustar])
# m4_equote()[m4_ifdef]m4_dquote()([AM_PROG_AR], [AM_PROG_AR]) # Required by automake 1.12.
AM_PROG_AR

# LT_INIT([win32-dll])

LT_INIT

# Create the libtool executable so that we can use it in further tests.
#
# LT_OUTPUT

AC_CANONICAL_HOST

# Check for C++ compiler and use it to compile the tests.
#
AC_PROG_CXX
AC_LANG(C++)

# Check for libcutl.
#
LIBCUTL([],[AC_MSG_ERROR([libcutl is not found; consider using --with-libcutl=DIR])])

# Output.
#
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    cli/cli/Makefile
    ])
AC_CONFIG_SUBDIRS([libcutl])
AC_OUTPUT
