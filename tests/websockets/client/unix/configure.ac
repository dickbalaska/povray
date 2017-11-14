#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
#m4_include([../../../../unix/config/ax_with_prog.m4])
#m4_include([m4/ax_lib_xerces.m4])
m4_include([../../../../unix/config/ax_boost_base.m4])
#m4_include([../../../../unix/config/ax_boost_regex.m4])
#m4_include([../../../../unix/config/ax_boost_system.m4])
m4_include([../../../../unix/config/ax_boost_thread.m4])
m4_include([../../../../unix/config/ax_boost_program_options.m4])
m4_include([../../../../unix/config/ax_compile_stdcxx_11.m4])
m4_include([../../../../unix/config/ax_websockets.m4])

#AC_CONFIG_SRCDIR([berzerk/Config.cpp])
#AC_CONFIG_HEADER([config.h])


# Checks for programs.
#AC_PROG_CC
AC_PROG_RANLIB



DEBUGFLAG=""
DEBUGNOTE="off"
PICFLAG=0
CFLAGS="${CFLAGS:=}"
CXXFLAGS="${CXXFLAGS:=}"

if test "$RANLIB" != ":"; then
  LIBCOMMAND="ar cr"
else
  LIBCOMMAND="ar crs"
fi
AC_SUBST(LIBCOMMAND)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

uname=`uname`
uversion=`uname -r | sed -e 's/-.*$//g;s/[\.]//g'`
if test "$uname" = "IRIX64"; then
    uname="IRIX"
fi

# Checks for library functions.

# XERCES detection doesn't work.
#AX_LIB_XERCES

##########################################################################################
#
# Silent building (at least quieter)
#
SILENTBUILD=".SILENT:"

AC_ARG_ENABLE(silent, [  --disable-silent        turn off silent build [default=no]],
	[if eval "test x$enable_silent = xyes"; then
		echo "Silent builds"
	else
		SILENTBUILD=""
		echo "Enabling non-silent builds"
	fi])

AC_SUBST(SILENTBUILD)


##########################################################################################
#
# Enable Debugging
#
AC_ARG_ENABLE(debug, [  --enable-debug          turn on debugging [default=no]],
	[if eval "test x$enable_debug = xyes"; then
		DEBUGFLAG="-g -D_DEBUG"
		DEBUGNOTE=$DEBUGFLAG
		echo "Enabling debug builds"
	fi])

CFLAGS="$DEBUGFLAG $CFLAGS"
CXXFLAGS="$DEBUGFLAG $CXXFLAGS"
# echo "5 CXXFLAGS=$CXXFLAGS"

##########################################################################################
#
# configure C++ flags
#

echo "2 CXXFLAGS=$CXXFLAGS"
SAVECXXFLAGS=$CXXFLAGS
AC_PROG_CXX
CXXFLAGS=$SAVECXXFLAGS

echo "3 CXXFLAGS=$CXXFLAGS"

# DIK - Lifted this section out of the fltk configure.in
dnl Add warnings to compiler switches:
dnl do this last so messing with switches does not break tests
MAKEDEPEND="\$(CXX) -M $OSFLAG"

if test -n "$GXX"; then
    CFLAGS="-Wall $CFLAGS"
	#DIKXXX    CXXFLAGS="-Wall $CXXFLAGS"
    if test -z "$DEBUGFLAG"; then
		#
		# Note: Can't use -fomit-frame-pointer - prevents tools like
		#       libsafe from working!
		#
		#       Don't use -fforce-mem, -fforce-addr, or -fcaller-saves.
		#       They all seem to make either no difference or enlarge
		#       the code by a few hundred bytes.
		#
        CFLAGS="-O2 $CFLAGS"
        CXXFLAGS="-O2 $CXXFLAGS"
    fi
    if test $PICFLAG = 1; then
    	CFLAGS="-fPIC $CFLAGS"
    	CXXFLAGS="-fPIC $CXXFLAGS"
    fi

#    # See if GCC supports -fno-exceptions...
#    AC_MSG_CHECKING(Testing if GCC supports -fno-exceptions...)
#    OLDCFLAGS="$CFLAGS"
#    CFLAGS="$CFLAGS -fno-exceptions"
#    AC_TRY_COMPILE(,,
#		CXXFLAGS="$CXXFLAGS -fno-exceptions"
#		AC_MSG_RESULT(yes; disabling them for FLTK...),
#		CFLAGS="$OLDCFLAGS"
#		AC_MSG_RESULT(no))

    # See if we are running Solaris; if so, try the -fpermissive option...
    if test $uname = SunOS; then
		AC_MSG_CHECKING(Testing if GCC supports -fpermissive...)
		
		OLDCFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -fpermissive"
		AC_TRY_COMPILE(,,
		    CXXFLAGS="$CXXFLAGS -fpermissive"
		    AC_MSG_RESULT(yes),
		    CFLAGS="$OLDCFLAGS"
		    AC_MSG_RESULT(no))
    fi
else
    case `(uname) 2>/dev/null` in
#	IRIX*)
#	    # Running some flavor of IRIX; see which version and
#	    # set things up according...
#	    if test "$uversion" -ge 62; then
#	        # We are running IRIX 6.2 or higher; uncomment the following
#		# lines if you don't have IDO 7.2 or higher:
#		#
#		#     CXX="CC -n32 -mips3"
#		#     CC="cc -n32 -mips3"
#		#     LD="ld -n32 -mips3"
#		#     MAKEDEPEND="CC -M"
#
#		if test "x`grep abi=n32 /etc/compiler.defaults`" = x; then
#			AC_MSG_WARN(FOR BEST RESULTS BEFORE COMPILING: setenv SGI_ABI \"-n32 -mips3\")
#		fi
#
#        	CFLAGS="-fullwarn $CFLAGS"
#        	CXXFLAGS="-fullwarn $CXXFLAGS"
#	    else
#        	CXXFLAGS="+w +pp $CXXFLAGS"
#	    fi
#	    if test -z "$DEBUGFLAG"; then
#        	CFLAGS="-O2 $CFLAGS"
#        	CXXFLAGS="-O2 $CXXFLAGS"
#	    fi
#	    ;;
	HP-UX*)
	    # Running HP-UX; these options should work for the HP compilers.
	    if test -z "$DEBUGFLAG"; then
		CFLAGS="+O2 $CFLAGS"
		CXXFLAGS="+O2 +W336,501,736,740,749,829 $CXXFLAGS"
	    fi
	    CFLAGS="+DAportable $CFLAGS"
	    CXXFLAGS="+DAportable $CXXFLAGS"
	    ;;
	SunOS*)
	    # Solaris
	    if test -z "$DEBUGFLAG"; then
		CFLAGS="-xO3"
		CXXFLAGS="-xO3"
	    fi
	    if test $PICFLAG = 1; then
		CFLAGS="-KPIC $CFLAGS"
		CXXFLAGS="-PIC $CXXFLAGS"
	    fi
	    ;;
	AIX*)
	    if test -z "$DEBUGFLAG"; then
        	CFLAGS="-O2 $CFLAGS"
        	CXXFLAGS="-O2 $CXXFLAGS"
	    fi
	    echo "WARNING: The AIX C and C++ compilers are known not to correctly"
	    echo "         compile the FLTK library.  We recommend using GCC 2.95.2"
	    echo "         under AIX."
	    ;;
	*)
	    # Running some other operating system; inform the user they
	    # should contribute the necessary options to fltk-bugs@fltk.org...
	    echo "Building berzerk with default compiler optimizations; contact"
	    echo "dick@buckosoft.com with uname and compiler options needed"
	    echo "for your platform, or set the CFLAGS and CXXFLAGS options"
	    echo "before running configure."
	    ;;
    esac
fi

echo "4 CXXFLAGS=$CXXFLAGS"

#AC_SUBST(LINKFLTK)
AC_SUBST(MAKEDEPEND)
#AC_SUBST(X_EXTRA_LIBS)


##########################################################################################
#
# boost support
#

#echo "Boost Base:"
AX_BOOST_BASE
#echo "Boost Program_Options:"
#AX_BOOST_PROGRAM_OPTIONS
#echo "Boost Regex:"
#AX_BOOST_REGEX
#echo "Boost Thread:"
#AX_BOOST_THREAD
#echo "Boost System:"
#AX_BOOST_SYSTEM

AC_COMPILE_STDCXX_11
#if defined HAVE_STDCXX_11
	CXXFLAGS="$CXXFLAGS -std=c++11"
	echo "Have c++11"
#endif
AX_WEBSOCKETS

echo "5 CXXFLAGS=$CXXFLAGS"
##########################################################################################
#
# Make the Makefiles
#
#AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([makeinclude
                 Makefile
                 ../Makefile
                 ])


# these directories (which have actual source code) need to have at least an empty .depend file
echo $ECHO_N >>.depend

#echo sysconfdir=${sysconfdir}
##########################################################################################
#
# generate config.status and launch it.
#

AC_OUTPUT

# weirdness modeled after openssh
A=`eval echo ${prefix}` ; A=`eval echo ${A}`
B=`eval echo ${sbindir}` ; B=`eval echo ${B}`
G=`eval echo ${VARRUND}` ; G=`eval echo ${G}`
H=`eval echo ${sysconfdir}` ; H=`eval echo ${H}`


echo ""
echo "========================================================"
echo "testpovws has been configured with the following options:"
echo ""
echo "                     Compiler : ${CXX}"
echo "               Compiler flags : ${CXXFLAGS}"
echo "                        Debug : $DEBUGNOTE"


