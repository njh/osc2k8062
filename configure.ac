# Process this file with autoconf to produce a configure script.
AC_INIT(osc2k8062.c)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(osc2k8062, 0.1)

# Check for C compiler
AC_PROG_CC

# Check for libusb
PKG_CHECK_MODULES(LIBUSB, libusb >= 0.1)
AC_SUBST(LIBUSB_CFLAGS)
AC_SUBST(LIBUSB_LIBS)

# Check for liblo
PKG_CHECK_MODULES(LIBLO, liblo >= 0.23)
AC_SUBST(LIBLO_CFLAGS)
AC_SUBST(LIBLO_LIBS)

AC_OUTPUT([Makefile])
