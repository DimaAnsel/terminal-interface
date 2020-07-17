/**
 * @file engine.c
 * Engine, toot toot.
 */

#include <string.h>

#include "main.h"

static QState Engine_initial(Engine * const me, QEvt const * const e);
static QState Idle(Engine * const me, QEvt const * const e);

//////////////////////////////////////////
/// @ingroup Fwk
/// @defgroup AOEngine Active Object - Engine
///	States for engine active object.
/// @{
/////////////////////////////////////////

/**
 * Creates a new section in a layer.
 *
 * @ref CREATE_SECTION_SIG, @ref AO_RenderArtist
 */
static void post_CREATE_SECTION(char* key, int yAnchor, int xAnchor, int yDim, int xDim) {
	SectionCfgEvt* e = Q_NEW(SectionCfgEvt, CREATE_SECTION_SIG);
	if (e) {
		strncpy(e->section.key, key, PAINTER_KEY_LEN);
		e->section.yAnchor = yAnchor;
		e->section.xAnchor = xAnchor;
		e->section.yDim = yDim;
		e->section.xDim = xDim;
		QACTIVE_POST(AO_RenderArtist, (QEvt*) e, AO_Engine);
	}
}

/**
 * Notifies other objects that essential systems are initialized.
 *
 * @ref ENGINE_START_SIG
 */
static void publish_ENGINE_START() {
	QEvt* e = Q_NEW(QEvt, ENGINE_START_SIG);
	if (e) {
		QF_PUBLISH(e, AO_Engine);
	}
}

/**
 * Notifies other objects that system is going down.
 *
 * @ref ENGINE_END_SIG
 */
static void publish_ENGINE_END() {
	QEvt* e = Q_NEW(QEvt, ENGINE_END_SIG);
	if (e) {
		QF_PUBLISH(e, AO_Engine);
	}
}

/// @}
/////////////////////////////////////////

/**
 * Initializes global curses settings.
 */
static void configure_screen() {
	initscr();
	cbreak();
	noecho();
	set_escdelay(0); // don't pause on ESC
	curs_set(0); // hide cursor
}

/**
 * Cleans up curses on exit.
 */
static void teardown_screen() {
	endwin();
}

/**
 * Make some test sections.
 */
static void test_sections() {
	post_CREATE_SECTION("tallMid", 1, 11, 7, 6);
	post_CREATE_SECTION("topLeft", 1, 1, 4, 9);
	post_CREATE_SECTION("left2x2", 6, 1, 2, 4);
	post_CREATE_SECTION("top3x3", 1, 18, 3, 6);
	post_CREATE_SECTION("topRight", 1, 25, 3, 4);
	post_CREATE_SECTION("bot", 9, 1, 1, 23);
	post_CREATE_SECTION("right2x2", 6, 6, 2, 4);
	post_CREATE_SECTION("bot3x3", 5, 18, 3, 6);
	post_CREATE_SECTION("botRight", 5, 25, 10, 6);
}

//////////////////////////////////////////
/// @addtogroup AOEngine
/// @{
//////////////////////////////////////////

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

	QActive_subscribe((QActive *)me, ENGINE_START_SIG);
	QActive_subscribe((QActive *)me, ENGINE_END_SIG);

	QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, 0);

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(Engine * const me, QEvt const * const e) {
	switch (e->sig) {
	case Q_ENTRY_SIG: {
		configure_screen();
		publish_ENGINE_START();
		return Q_HANDLED();
	}
	case ENGINE_START_SIG: {
		test_sections();
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
