/*
 * wsgraphics.h
 *
 *  Created on: Nov 20, 2017
 *      Author: dick
 */

#ifndef _VFE_WEBSOCKETS_WSGRAPHICS_H_
#define _VFE_WEBSOCKETS_WSGRAPHICS_H_
//#include "websocketpp/config/asio_no_tls.hpp"
//#include "websocketpp/server.hpp"

#include "vfe.h"

namespace vfe {
	class QtVfe;
}
namespace povray {
namespace qtpov {

enum
{
	WSG_INIT = 0,
	WSG_DRAW_PIXEL_BLOCK,
	WSG_DRAW_FILLED_RECT,
	WSG_DRAW_PIXEL
};

using namespace vfe;

class QtGraphics : public vfeDisplay {
public:
	QtGraphics(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible);
	virtual ~QtGraphics();

    virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
    virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
    virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
    virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);
    virtual void Clear();

	void	setQtVfe(QtVfe* qtVfe) { m_qtVfe = qtVfe; }
	void	SendInit();
private:
	QtVfe*	m_qtVfe;

};

} /* namespace qtpov */
} /* namespace povray */
#endif /* _VFE_WEBSOCKETS_WSGRAPHICS_H_ */
