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

enum
{
	WSG_INIT = 0,
	WSG_DRAW_PIXEL_BLOCK
};
WsGraphics::WsGraphics(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible)
    : vfeDisplay(w, h, gamma, session, visible)
{
	std::cerr << "WsGraphics::WsGraphics: w=" << w << " h=" << h << std::endl;
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

}
void WsGraphics::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{

}
void WsGraphics::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
	std::cerr << "WsGraphics::DrawPixelBlock: x1=" << x1 << " y1=" << y1 << std::endl;

}
void WsGraphics::Clear()
{

}

} /* namespace websockets */
} /* namespace povray */
