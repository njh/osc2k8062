dnl Require autoconf version >= 2.59
AC_PREREQ(2.59)


dnl ############# Initialization

AC_INIT([osc2k8062], [0.1], [njh@aelius.com])
AC_CONFIG_SRCDIR([osc2k8062.c])
AC_CONFIG_AUX_DIR([build-scripts])
AC_CONFIG_MACRO_DIR([build-scripts])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])


dnl ############# Compiler and tools Checks

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S


dnl ############## Library Checks

# Check for libusb
PKG_CHECK_MODULES(LIBUSB, libusb >= 0.1)
AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)

# Check for liblo
PKG_CHECK_MODULES(LIBLO, liblo >= 0.23)
AC_SUBST(LIBLO_CFLAGS)
AC_SUBST(LIBLO_LIBS)


dnl ############## Output files

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
