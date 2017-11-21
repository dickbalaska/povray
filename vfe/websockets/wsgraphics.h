/*
 * wsgraphics.h
 *
 *  Created on: Nov 20, 2017
 *      Author: dick
 */

#ifndef _VFE_WEBSOCKETS_WSGRAPHICS_H_
#define _VFE_WEBSOCKETS_WSGRAPHICS_H_
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "vfe.h"

namespace povray {
namespace websockets {

enum
{
	WSG_INIT = 0,
	WSG_DRAW_PIXEL_BLOCK
};

using namespace vfe;

class WsGraphics : public vfeDisplay {
public:
	WsGraphics(unsigned int w, unsigned int h, GammaCurvePtr gamma, vfeSession *session, bool visible);
	virtual ~WsGraphics();

    virtual void DrawPixel(unsigned int x, unsigned int y, const RGBA8& colour);
    virtual void DrawRectangleFrame(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
    virtual void DrawFilledRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8& colour);
    virtual void DrawPixelBlock(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RGBA8 *colour);
    virtual void Clear();

	void	SendInit();

	websocketpp::connection_hdl m_hdl;
};

} /* namespace websockets */
} /* namespace povray */
#endif /* _VFE_WEBSOCKETS_WSGRAPHICS_H_ */
