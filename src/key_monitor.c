/**
 * @file render_engine.c
 * KeyMonitor, toot toot.
 */

#include "main.h"

static QState KeyMonitor_initial(KeyMonitor * const me, QEvt const * const e);
static QState Idle(KeyMonitor * const me, QEvt const * const e);

////////////////////////////////////

/**
 * Initializes input window.
 */
static WINDOW* configure() {
	WINDOW* win = newwin(1,1,0,0); // top left
	keypad(win, TRUE);
	nodelay(win, TRUE);

	wrefresh(win); // puts this below other windows
	return win;
}

/////////////////////////////////////////

static void post_KEY_DETECT_SIG(int key) {
	KeyEvt* e = Q_NEW(KeyEvt, KEY_DETECT_SIG);
	if (e) {
		e->key = key;
		QACTIVE_POST(AO_BindingHandler, (QEvt *)e, AO_KeyMonitor);
	}
}

//////////////////////////////////////////
///
/// @defgroup AOKeyMonitor Active Object - KeyMonitor
///		States for key monitor active object.
///
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static KeyMonitor l_keyMonitor;
/**Global KeyMonitor AO*/
QActive * const AO_KeyMonitor = &l_keyMonitor.super;

/**
 * Constructor.
 */
void KeyMonitor_ctor(void) {
	KeyMonitor *me = (KeyMonitor *)AO_KeyMonitor;
	QActive_ctor(&me->super, Q_STATE_CAST(&KeyMonitor_initial));

	QTimeEvt_ctorX(&me->keyScanEvt, (QActive *)me, KEY_SCAN_SIG, 0U);
}

/**
 * Initial.
 */
static QState KeyMonitor_initial(KeyMonitor * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	QActive_subscribe((QActive*) me, ENGINE_START_SIG);

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(KeyMonitor * const me, QEvt const * const e) {
	switch (e->sig) {
	case ENGINE_START_SIG: {
		me->win = configure();
		QTimeEvt_armX(&me->keyScanEvt, 1, 1);
		return Q_HANDLED();
	}
	case KEY_SCAN_SIG: {
		int key = wgetch(me->win);
		if (key != ERR) {
			post_KEY_DETECT_SIG(key);
		}
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
