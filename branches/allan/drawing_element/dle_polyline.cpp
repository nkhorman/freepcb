#include "stdafx.h"
#include "dle_polyline.h"

// globals
unsigned short dith_1[] = { 
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
unsigned short dith_2[] = { 
	0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
	0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa };
unsigned short dith_4[] = { 
	0xaa,0x00,0xaa,0x00,0xaa,0x00,0xaa,0x00,
	0x55,0x00,0x55,0x00,0x55,0x00,0x55,0x00,
	0x00,0xaa,0x00,0xaa,0x00,0xaa,0x00,0xaa,
	0x00,0x55,0x00,0x55,0x00,0x55,0x00,0x55 };
unsigned short dith_8[] = { 
	0x88,0x00,0x88,0x00,0x88,0x00,0x88,0x00,
	0x00,0x88,0x00,0x88,0x00,0x88,0x00,0x88,
	0x22,0x00,0x22,0x00,0x22,0x00,0x22,0x00,
	0x00,0x22,0x00,0x22,0x00,0x22,0x00,0x22,
	0x44,0x00,0x44,0x00,0x44,0x00,0x44,0x00,
	0x00,0x44,0x00,0x44,0x00,0x44,0x00,0x44,
	0x11,0x00,0x11,0x00,0x11,0x00,0x11,0x00,
	0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x11 };
unsigned short dith_16[] = { 
	0x88,0x00,0x00,0x00,0x88,0x00,0x00,0x00,
	0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,
	0x22,0x00,0x00,0x00,0x22,0x00,0x00,0x00,
	0x11,0x00,0x00,0x00,0x11,0x00,0x00,0x00,
	0x00,0x88,0x00,0x00,0x00,0x88,0x00,0x00,
	0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,
	0x00,0x22,0x00,0x00,0x00,0x22,0x00,0x00,
	0x00,0x11,0x00,0x00,0x00,0x11,0x00,0x00,
	0x00,0x00,0x88,0x00,0x00,0x00,0x88,0x00,
	0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,
	0x00,0x00,0x22,0x00,0x00,0x00,0x22,0x00,
	0x00,0x00,0x11,0x00,0x00,0x00,0x11,0x00,
	0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x88,
	0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,
	0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x22,
	0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x11 };


// draw a closed CPolyLine
void CDLE_POLYLINE::_Draw(CDrawInfo const &di) const
{
	CPolyLine * poly = (CPolyLine*)ptr;
	int layer = poly->GetLayer();
	if( onScreen() && poly->GetClosed() )
	{
		// set up brush with dither pattern
//		int dith_factor = poly->GetDitherFactor();
//		int dith_offset = poly->GetDitherOffset();
		int dith_factor = 1;
		int dith_offset = 0;
#if 0
		// get left and top pixel coords to correct for pixel shift of brush
		CRect br = poly->GetCornerBounds( 0 );
		CPoint topleft = br.TopLeft();
		topleft = dlist->PCBToScreen( topleft );
		if( dith_factor == 4 )
		{
			int x_shift = topleft.x%2;
			int y_shift = topleft.y%2;
			if( x_shift )
			{
				if( dith_offset%2)
					dith_offset--;
				else
					dith_offset++;
			}
			if( y_shift )
				dith_offset = (dith_offset + 2)%4;
		}
		if( dith_offset >= dith_factor )
			ASSERT(0);
#endif
		unsigned short * lpDith;
		switch( dith_factor ) {
		case 1: lpDith = dith_1; break;
		case 2: lpDith = dith_2; break;
		case 4: lpDith = dith_4; break;
		case 8: lpDith = dith_8; break;
		case 16: lpDith = dith_16; break;
		default: ASSERT(0); break;
		}
		lpDith += 8*dith_offset;
		CBitmap patternBmap;
		patternBmap.CreateBitmap( 8, 8, 1, 1, lpDith );
		CBrush patternBrush( &patternBmap );
		CBrush * old_brush = di.DC->SelectObject( &patternBrush );

		// start by drawing the first contour (no cutouts)
		int n = poly->GetContourEnd(0) + 1;
		POINT *lpPoints = new POINT[n];
		for( int i=0; i<n; i++ )
		{
			lpPoints[i].x = poly->GetX(i)/dlist->Get_pcbu_per_wu();
			lpPoints[i].y = poly->GetY(i)/dlist->Get_pcbu_per_wu();
		}
		di.DC->SetPolyFillMode( WINDING );
		BOOL bOK = di.DC->Polygon( lpPoints, n );
		di.DC->SelectObject( old_brush );
	}
}

int CDLE_POLYLINE::onScreen(void) const
{
	CPolyLine * poly = (CPolyLine*)ptr;
	CRect br = poly->GetCornerBounds( 0 );
	br = br.MulDiv(1, dlist->Get_pcbu_per_wu() );
	if( br.left < dlist->m_max_x && br.right > dlist->m_org_x)
		return 1;
	if( br.bottom < dlist->m_max_y && br.top > dlist->m_org_y)
		return 1;
	return 0;
}
