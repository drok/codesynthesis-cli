dnl file      : m4/libcutl.m4
dnl license   : MIT; see accompanying LICENSE file
dnl
dnl LIBCUTL([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
dnl
dnl
AC_DEFUN([LIBCUTL], [
libcutl_found=no

AC_ARG_WITH(
  [libcutl],
  [AC_HELP_STRING([--with-libcutl=DIR],[location of libcutl build directory])],
  [libcutl_dir=${withval}],
  [libcutl_dir=libcutl])

AC_MSG_CHECKING([for libcutl])

# If libcutl_dir was given, add the necessary preprocessor and linker flags.
#
if test x"$libcutl_dir" != x; then

  # AS_SET_CATFILE([abs_libcutl_dir], [$ac_pwd], [$libcutl_dir])

  CUTL_CFLAGS='-I$(top_srcdir)/'$libcutl_dir
  CUTL_LIBS='-L$(top_srcdir)/'$libcutl_dir'/libcutl -lcutl'
  AC_SUBST(CUTL_LIBS)
  AC_SUBST(CUTL_CFLAGS)
fi

])dnl
