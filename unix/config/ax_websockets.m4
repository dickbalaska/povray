AC_DEFUN([AX_WEBSOCKETS],
[

	AC_ARG_WITH([websockets],
		AS_HELP_STRING([--websockets],
	                   [include the use of websockets ]),
	        [
	        if test "$withval" = "no"; then
				want_websockets="no"
	        elif test "$withval" = "yes"; then
	            want_websockets="yes"
	        else
			    want_websockets="yes"
				ax_websockets_dir="$withval"
			fi
	        ],
	        [want_websockets="yes"]
	)
	
	if test "x$want_websockets" = "xyes"; then
		AC_MSG_CHECKING([for websockets])
        #AC_REQUIRE([AC_PROG_CC])	# We already know we are c++11
		AC_LANG_PUSH(C++)
		AC_COMPILE_IFELSE([
            AC_LANG_PROGRAM(
                [[
@%:@include <websocketpp/config/asio_no_tls_client.hpp>
@%:@include <websocketpp/client.hpp>
			]],
			[[]]
			)],
            [
				websockets_header_found="yes"
				AC_MSG_RESULT([found])
			],
			[
				websockets_header_found="no"
				AC_MSG_RESULT([not found])
			]
		)
		AC_LANG_POP([C++])         
	fi
	
	if test "x$websockets_header_found" = "xyes"; then
		HAVE_WEBSOCKETS="yes"
		USE_WEBSOCKETS="yes"
	else	
		HAVE_WEBSOCKETS="no"
		USE_WEBSOCKETS="no"
	fi
	AC_DEFINE([HAVE_WEBSOCKETS])
	AC_DEFINE([USE_WEBSOCKETS])
])
