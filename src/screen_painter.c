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

/**
 * Outlines a window.
 */
static void outline(WINDOW* win, int height, int width) {
	int dim = (width > height) ? width : height;
	char tempStr[dim + 1];

	for (int i = 0; i < dim; i++) {
		tempStr[i] = '-';
	}
	tempStr[dim] = '\0';

	mvwaddstr(win, 0, 0, tempStr); // top row
	mvwaddstr(win, height - 1, 0, tempStr); // bottom row

	for (int i = 1; i < height - 1; i++) {
		mvwaddch(win, i, 0, '|');
		mvwaddch(win, i, width - 1, '|');
	}
}

/////////////////////////////////////////

static void post_REFRESH_SCREEN(WINDOW* win) {
	ScreenEvt* e = Q_NEW(ScreenEvt, REFRESH_SCREEN_SIG);
	if (e) {
		e->win = win;
		QACTIVE_POST(AO_ScreenPainter, (QEvt*) e, NULL);
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
		me->win = newwin(MAX_SCREEN_HEIGHT, MAX_SCREEN_WIDTH, 0, 0);
		outline(me->win, MAX_SCREEN_HEIGHT, MAX_SCREEN_WIDTH);
		post_REFRESH_SCREEN(me->win);
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
		mvwaddstr(me->win, paintEvt->yAnchor, paintEvt->xAnchor, paintEvt->canvas);
		post_REFRESH_SCREEN(me->win);
		return Q_HANDLED();
	}
	case REFRESH_SCREEN_SIG: {
		wrefresh(((ScreenEvt*)e)->win);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
