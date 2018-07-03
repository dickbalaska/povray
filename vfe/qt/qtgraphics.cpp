/*
 * wsgraphics.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: dick
 */

#include <QDebug>
#include "qtgraphics.h"
#include "qtvfe.h"

#ifdef _DEBUG
static bool wsDebug = false;
#endif


namespace povray {
namespace qtpov {

QtGraphics::QtGraphics(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible)
	: vfeDisplay(w, h, gamma, session, visible)
{
#ifdef _DEBUG
	std::cerr << "WsGraphics::WsGraphics: w=" << w << " h=" << h << std::endl;
#endif
}

QtGraphics::~QtGraphics() {
	// TODO Auto-generated destructor stub
}

void QtGraphics::SendInit()
{
	uchar buff[12];
	unsigned int* bi = (unsigned int*)&buff;
	*bi++ = (quint32)(WSG_INIT);
	*bi++ = (quint32)(GetWidth());
	*bi++ = (quint32)(GetHeight());

	//WebsocketServer::sendBinary(m_hdl, buff, 12);
	m_qtVfe->sendPovrayGraphicsMessage(buff, 12);

}
void QtGraphics::DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour)
{
	//std::cerr << "QtGraphics::DrawPixel: x=" << x << " y=" << y << std::endl;
	qWarning() << "QtGraphics::DrawPixel: x=" << x << " y=" << y;

}
void QtGraphics::DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
	//std::cerr << "QtGraphics::DrawRectangleFrame: x1=" << x1 << " y1=" << y1 << std::endl;
	qWarning() << "QtGraphics::DrawRectangleFrame: x1=" << x1 << " y1=" << y1;

}
void QtGraphics::DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour)
{
	//std::cerr << "QtGraphics::DrawFilledRectangle: x1=" << x1 << " y1=" << y1 << std::endl;
	qWarning() << "QtGraphics::DrawFilledRectangle: x1=" << x1 << " y1=" << y1;

}
void QtGraphics::DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour)
{
#ifdef _DEBUG
	if (wsDebug)
		std::cerr << "QtGraphics::DrawPixelBlock: x1=" << x1 << " y1=" << y1 << std::endl;
#endif
	int size = std::abs((int)(x2-x1+1)) * std::abs((int)(y2-y1+1));
	int fullsize = (size+5)*4;		// add opcode plus 4 coords
	//char buff[fullsize];
	//QList<uint>	qui;
	uchar* buff = new uchar[fullsize];
	unsigned int* bi = (unsigned int*)buff;
	*bi++ = (quint32)(WSG_DRAW_PIXEL_BLOCK);
	*bi++ = (quint32)(x1);
	*bi++ = (quint32)(y1);
	*bi++ = (quint32)(x2);
	*bi++ = (quint32)(y2);
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
	//WebsocketServer::sendBinary(m_hdl, buff, fullsize);
	m_qtVfe->sendPovrayGraphicsMessage(buff, fullsize);
	delete buff;
}

void QtGraphics::Clear()
{

}

} /* namespace qtpov */
} /* namespace povray */
