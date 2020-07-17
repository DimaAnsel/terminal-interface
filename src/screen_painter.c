/**
 * @file screen_painter.c
 * ScreenPainter, toot toot.
 */

#include "main.h"
#include <string.h>

static QState ScreenPainter_initial(ScreenPainter * const me, QEvt const * const e);
static QState Setup(ScreenPainter * const me, QEvt const * const e);
static QState Idle(ScreenPainter * const me, QEvt const * const e);

//////////////////////////////////////////
/// @ingroup Fwk
/// @defgroup AOScreenPainter Active Object - ScreenPainter
///	States for screen painter active object.
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static ScreenPainter l_screenPainter;
/**Global ScreenPainter AO*/
QActive * const AO_ScreenPainter = &l_screenPainter.super;

/**
 * Constructor.
 */
void ScreenPainter_ctor(void) {
	ScreenPainter *me = (ScreenPainter *)AO_ScreenPainter;
	QActive_ctor(&me->super, Q_STATE_CAST(&ScreenPainter_initial));
}

/**
 * Initial.
 */
static QState ScreenPainter_initial(ScreenPainter * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	QActive_subscribe((QActive *)me, ENGINE_START_SIG);

	return Q_TRAN(&Setup);
}

/**
 * Setup state.
 */
static QState Setup(ScreenPainter * const me, QEvt const * const e) {
	switch (e->sig) {
	case ENGINE_START_SIG: {
		return Q_TRAN(&Idle);
	}
	}
	return Q_SUPER(&QHsm_top);
}

/**
 * Idle state.
 */
static QState Idle(ScreenPainter * const me, QEvt const * const e) {
	switch (e->sig) {
	case PAINT_DIRECT_SIG: {
		PaintEvt* paintEvt = (PaintEvt *)e;
		mvaddstr(paintEvt->yAnchor, paintEvt->xAnchor, paintEvt->canvas);
		return Q_HANDLED();
	}
	case REFRESH_SCREEN_SIG: {
		refresh();
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
