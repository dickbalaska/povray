/*
 * wsgraphics.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dick
 */

#include "wsgraphics.h"
#include "wsserver.h"

namespace povray {
namespace websockets {

WsGraphics::WsGraphics(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible)
    : vfeDisplay(w, h, gamma, session, visible)
{
#ifdef _DEBUG
	std::cerr << "WsGraphics::WsGraphics: w=" << w << " h=" << h << std::endl;
#endif
}

WsGraphics::~WsGraphics() {
	// TODO Auto-generated destructor stub
}

void WsGraphics::SendInit()
{
	char buff[12];
	unsigned int* bi = (unsigned int*)&buff;
	*bi++ = htonl(WSG_INIT);
	*bi++ = htonl(GetWidth());
	*bi++ = htonl(GetHeight());

	WebsocketServer::sendBinary(m_hdl, buff, 12);

}
void WsGraphics::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
{
	std::cerr << "WsGraphics::DrawPixel: x=" << x << " y=" << y << std::endl;

}
void WsGraphics::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
	std::cerr << "WsGraphics::DrawRectangleFrame: x1=" << x1 << " y1=" << y1 << std::endl;

}
void WsGraphics::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
	std::cerr << "WsGraphics::DrawFilledRectangle: x1=" << x1 << " y1=" << y1 << std::endl;

}
void WsGraphics::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
#ifdef _DEBUG
	std::cerr << "WsGraphics::DrawPixelBlock: x1=" << x1 << " y1=" << y1 << std::endl;
#endif
	int size = std::abs(x2-x1+1) * std::abs(y2-y1+1);
	int fullsize = (size+5)*4;		// add opcode plus 4 coords
	char buff[fullsize];
	unsigned int* bi = (unsigned int*)&buff;
	*bi++ = htonl(WSG_DRAW_PIXEL_BLOCK);
	*bi++ = htonl(x1);
	*bi++ = htonl(y1);
	*bi++ = htonl(x2);
	*bi++ = htonl(y2);
	unsigned char* bc = (unsigned char*)bi;
	for (unsigned int y = y1, i = 0; y <= y2; y++) {
		for(unsigned int x = x1; x <= x2; x++, i++) {
        	*bc++ = colour->red;
        	*bc++ = colour->green;
        	*bc++ = colour->blue;
        	*bc++ = colour->alpha;
        	colour++;
        }
	}
	WebsocketServer::sendBinary(m_hdl, buff, fullsize);
}
void WsGraphics::Clear()
{

}

} /* namespace websockets */
} /* namespace povray */
