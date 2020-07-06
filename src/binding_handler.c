/**
 * @file render_engine.c
 * BindingHandler, toot toot.
 */

#include "main.h"

static QState BindingHandler_initial(BindingHandler * const me, QEvt const * const e);
static QState Idle(BindingHandler * const me, QEvt const * const e);

////////////////////////////////////

/////////////////////////////////////////

static void publish_KEY_DETECT(int key) {
	KeyEvt* e = Q_NEW(KeyEvt, KEY_DETECT_SIG);
	if (e) {
		e->key = key;
		QF_PUBLISH((QEvt *)e, AO_BindingHandler);
	}
}

//////////////////////////////////////////
///
/// @defgroup AOBindingHandler Active Object - BindingHandler
///		States for binding handler active object.
///
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static BindingHandler l_bindingHandler;
/**Global BindingHandler AO*/
QActive * const AO_BindingHandler = &l_bindingHandler.super;

/**
 * Constructor.
 */
void BindingHandler_ctor(void) {
	BindingHandler *me = (BindingHandler *)AO_BindingHandler;
	QActive_ctor(&me->super, Q_STATE_CAST(&BindingHandler_initial));
}

/**
 * Initial.
 */
static QState BindingHandler_initial(BindingHandler * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(BindingHandler * const me, QEvt const * const e) {
	switch (e->sig) {
	case KEY_DETECT_SIG: {
		int key = ((KeyEvt *)e)->key;
		publish_KEY_DETECT(key);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
