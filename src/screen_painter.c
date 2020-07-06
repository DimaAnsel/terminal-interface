/**
 * @file screen_painter.c
 * ScreenPainter, toot toot.
 */

#include "main.h"
#include <string.h>

static QState ScreenPainter_initial(ScreenPainter * const me, QEvt const * const e);
static QState Setup(ScreenPainter * const me, QEvt const * const e);
static QState Idle(ScreenPainter * const me, QEvt const * const e);

////////////////////////////////////


static void post_REFRESH_SCREEN() {
	QEvt* e = Q_NEW(QEvt, REFRESH_SCREEN_SIG);
	if (e) {
		QACTIVE_POST(AO_ScreenPainter, e, AO_ScreenPainter);
	}
}

/////////////////////////////////////////

/**
 * Outlines a window.
 */
static void outline(int height, int width) {
	int dim = (width > height) ? width : height;
	char tempStr[dim + 1];

	for (int i = 0; i < dim; i++) {
		tempStr[i] = '-';
	}
	tempStr[dim] = '\0';

	mvaddstr(0, 0, tempStr); // top row
	mvaddstr(height - 1, 0, tempStr); // bottom row

	for (int i = 1; i < height - 1; i++) {
		mvaddch(i, 0, '|');
		mvaddch(i, width - 1, '|');
	}
}

//////////////////////////////////////////
///
/// @defgroup AOScreenPainter Active Object - ScreenPainter
///		States for screen painter active object.
///
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
		outline(MAX_SCREEN_HEIGHT, MAX_SCREEN_WIDTH);
		post_REFRESH_SCREEN();
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
