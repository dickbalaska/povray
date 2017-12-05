//******************************************************************************
///
/// @file vfe/websockets/wswinconsole.cpp
///
/// This file contains a basic proof-of-concept POV implementation using VFE.
///
/// @author Christopher J. Cason
/// @author Dick Balaska
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifdef _WINDOWS					// windows only main

#include "base/version_info.h"

#include "backend/povray.h"

// boost headers
#include <boost/shared_ptr.hpp>

// other library headers

#include "wshandler.h"
#include "wsoptions.h"
#include "wsserver.h"

// from directory "vfe"
//#include "vfeplatform.h"
#include "vfe.h"


#ifndef _CONSOLE
#error "You must define _CONSOLE in windows/povconfig/syspovconfig.h prior to building the websockets version, otherwise you will get link errors."
#endif

#define REGKEY		"POV-Ray"
#define	REGVERKEY	"v3.7"

enum ReturnValue
{
	RETURN_OK = 0,
	RETURN_ERROR,
	RETURN_USER_ABORT
};

using namespace vfe;
using namespace vfePlatform;

namespace pov_frontend
{
	////////////////////////////////
	// Called from the shellout code
	////////////////////////////////
	bool MinimizeShellouts(void) { return false; } // TODO
	bool ShelloutsPermitted(void) { return false; } // TODO
}


bool gCancelRender = false;
string	DocumentsPath;

bool getHome(void)
{
	HKEY        key;
	DWORD       len;
	char	DocumentsPath_[_MAX_PATH];
	char	BinariesPath_[_MAX_PATH];
	stringstream subkey;
	subkey << "Software\\" << REGKEY << "\\" << REGVERKEY << "\\Windows";
	DocumentsPath_[0] = '\0';
	if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey.str().c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
//		len = sizeof(LastInferredHome);
//		RegQueryValueEx(key, "LastInferredHome", 0, NULL, (BYTE *)LastInferredHome, &len);

		len = sizeof(DocumentsPath_);
		RegQueryValueEx(key, "DocPath", 0, NULL, (BYTE *)DocumentsPath_, &len);

		RegCloseKey(key);
		DocumentsPath = DocumentsPath_;

//		return (BinariesPath[0] != '\0');
	}
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\" REGKEY "\\" REGVERKEY "\\Windows", 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		len = sizeof(BinariesPath_);
		RegQueryValueEx(key, "Home", 0, NULL, (BYTE *)BinariesPath_, &len);
		RegCloseKey(key);
		return (BinariesPath_[0] != '\0');
	}
	return (false);
}

namespace povray { namespace websockets {

string GetBannerVersion()
{
#define PACKAGE_NAME	"povrayws"
	string v = "version ";
	char s[2048];
	snprintf(s, 2048,
		"%s %s\n\n"
		"%s\n%s\n%s\n"
		"%s\n%s\n%s\n\n",
		PACKAGE_NAME, POV_RAY_VERSION,
		DISTRIBUTION_MESSAGE_1, DISTRIBUTION_MESSAGE_2, DISTRIBUTION_MESSAGE_3,
		POV_RAY_COPYRIGHT, DISCLAIMER_MESSAGE_1, DISCLAIMER_MESSAGE_2
	);
	v += s;
//	snprintf(s, 2048,
//		"Built-in features:\n"
//		"  I/O restrictions:          %s\n"
//		"  X Window display:          %s\n"
//		"  Supported image formats:   %s\n"
//		"  Unsupported image formats: %s\n\n",
//		BUILTIN_IO_RESTRICTIONS, BUILTIN_XWIN_DISPLAY, BUILTIN_IMG_FORMATS, MISSING_IMG_FORMATS
//	);
//	v += s;
	snprintf(s, 2048,
		"Compilation settings:\n"
//		"  Build architecture:  %s\n"
//		"  Built/Optimized for: %s\n"
		"  Compiler vendor:     %s\n"
		"  Compiler version:    %s\n",
//		"  Compiler flags:      %s\n",
//		BUILD_ARCH, BUILT_FOR, COMPILER_VENDOR, COMPILER_VERSION, CXXFLAGS
		COMPILER_NAME, POV_COMPILER_VER
	);
	v += s;
	return(v);
}


}}	// namespace websockets, namespace povray

void ProcessSignal(void)
{

}

void PrintStatus(vfeSession *session)
{
	string str;
	vfeSession::MessageType type;
	static vfeSession::MessageType lastType = vfeSession::mUnclassified;

	while (session->GetNextCombinedMessage(type, str))
	{
		if (type != vfeSession::mGenericStatus)
		{
			if (lastType == vfeSession::mGenericStatus)
				fprintf(stderr, "\n");
			fprintf(stderr, "%s\n", str.c_str());
		}
		else
			fprintf(stderr, "%s\r", str.c_str());
		lastType = type;
	}
}

void ErrorExit(vfeSession *session)
{
	fprintf(stderr, "%s\n", session->GetErrorString());
	session->Shutdown();
	delete session;
	exit(1);
}

// this is an example of a minimal console version of POV-Ray using the VFE
// (virtual front-end) library. it is NOT INTENDED TO BE A FULLY-FEATURED
// CONSOLE IMPLEMENTATION OF POV-RAY and is not officially supported. see
// the unix version for a example of a more comprehensive console build.
int main(int argc, char **argv)
{
//	char              *s;
//	vfeWebsocketSession     *session = new vfeWebsocketSession();
//	vfeStatusFlags    flags;
//	vfeRenderOptions  opts;
//	boost::thread*    sigthread;

	fprintf(stderr,
		"This is the websockets edition of POV-Ray under Windows.\n"
		"It is intended to be called from qtpov.\n\n"
		"Persistence of Vision(tm) Ray Tracer Version " POV_RAY_VERSION_INFO ".\n"
		DISTRIBUTION_MESSAGE_1 "\n"
		DISTRIBUTION_MESSAGE_2 "\n"
		DISTRIBUTION_MESSAGE_3 "\n"
		POV_RAY_COPYRIGHT "\n"
		DISCLAIMER_MESSAGE_1 "\n"
		DISCLAIMER_MESSAGE_2 "\n\n");

	getHome();
	// create the signal handling thread
	//sigthread = new boost::thread(SignalHandler);
	::povray::websockets::WebsocketServer::init();
	int port = 4401;
	while (!::povray::websockets::WebsocketServer::listen(port)) {
		cerr << "failed to init socket port " << port << endl;
		ProcessSignal();
		if (gCancelRender)
			return(RETURN_USER_ABORT);
		Delay(1000);
	}
	cerr << "listening on port " << port << endl;
	::povray::websockets::WebsocketServer::setReceiveHandler(&::povray::websockets::WsHandler::staticReceiveHandler);
	boost::thread wsthread(::povray::websockets::WebsocketServer::run);

	::povray::websockets::waitForShutdown();
	::povray::websockets::WebsocketServer::stop();
	wsthread.join();

	//if (session->Initialize(NULL, NULL) != vfeNoError)
	//	ErrorExit(session);

	//if ((s = getenv("POVINC")) != NULL)
	//	opts.AddLibraryPath(s);
	//while (*++argv)
	//	opts.AddCommand(*argv);

	//if (session->SetOptions(opts) != vfeNoError)
	//	ErrorExit(session);
	//if (session->StartRender() != vfeNoError)
	//	ErrorExit(session);

	//bool pauseWhenDone = session->GetBoolOption("Pause_When_Done", false);
	//while (((flags = session->GetStatus(true, 1000)) & stRenderShutdown) == 0)
	//	PrintStatus(session);
	//session->Shutdown();
	//PrintStatus(session);
	//delete session;

	//if (pauseWhenDone)
	//{
	//	fprintf(stderr, "Press enter to continue ... ");
	//	fflush(stderr);
	//	getchar();
	//}

	return 0;
}

#endif		// _WINDOWS
