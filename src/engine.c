/**
 * @file engine.c
 * Engine, toot toot.
 */

#include "main.h"

static QState Engine_initial(Engine * const me, QEvt const * const e);
static QState Idle(Engine * const me, QEvt const * const e);

////////////////////////////////////

/**
 * Initializes global curses settings.
 */
static void configure_screen() {
	initscr();
	cbreak();
	noecho();
	set_escdelay(0);
}

/**
 * Cleans up curses settings.
 */
static void teardown_screen() {
	endwin();
}

/////////////////////////////////////////

static void post_CREATE_SECTION() {
	SectionCfgEvt* e = Q_NEW(SectionCfgEvt, CREATE_SECTION_SIG);
	if (e) {
		QACTIVE_POST(AO_RenderEngine, (QEvt*) e, NULL);
	}
}

static void publish_ENGINE_START() {
	QEvt* e = Q_NEW(QEvt, ENGINE_START_SIG);
	if (e) {
		QF_PUBLISH(e, AO_Engine);
	}
}

static void publish_ENGINE_END() {
	QEvt* e = Q_NEW(QEvt, ENGINE_END_SIG);
	if (e) {
		QF_PUBLISH(e, AO_Engine);
	}
}

//////////////////////////////////////////
///
/// @defgroup AOEngine Active Object - Engine
///		States for engine active object.
///
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static Engine l_engine;
/**Global Engine AO*/
QActive * const AO_Engine = &l_engine.super;

/**
 * Constructor.
 */
void Engine_ctor(void) {
	Engine *me = (Engine *)AO_Engine;
	QActive_ctor(&me->super, Q_STATE_CAST(&Engine_initial));

	QTimeEvt_ctorX(&me->timeEvt, (QActive *)me, TIMEOUT_SIG, 0U);
}

/**
 * Initial.
 */
static QState Engine_initial(Engine * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	QActive_subscribe((QActive *)me, ENGINE_END_SIG);

	QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, 0);

	return Q_TRAN(&Idle);
}

static QState Idle(Engine * const me, QEvt const * const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		configure_screen();
		publish_ENGINE_START();
		return Q_HANDLED();
	}
	case TIMEOUT_SIG: {
		publish_ENGINE_END();
		return Q_HANDLED();
	}
	case ENGINE_END_SIG: {
		teardown_screen();
		QF_stop();
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
