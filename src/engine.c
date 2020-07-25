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
 *
 * @param[in] key	  Section key
 * @param[in] yAnchor Vertical anchor (from top)
 * @param[in] xAnchor Horizontal anchor (from left)
 * @param[in] yDim	  Vertical size
 * @param[in] xDim	  Horizontal size
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
 * Paints a single line for a section.
 *
 * @ref PAINT_LINE_SIG, @ref AORenderArtist
 *
 * @param[in] section Section key
 * @param[in] yAnchor Vertical anchor (from top)
 * @param[in] xAnchor Horizontal anchor (from left)
 * @param[in] artwork String to draw
 */
static void post_PAINT_LINE(char* section, uint16_t yAnchor, uint16_t xAnchor, char* artwork) {
	PaintEvt* e = Q_NEW(PaintEvt, PAINT_LINE_SIG);
	if (e) {
		strncpy(e->sectionKey, section, PAINTER_KEY_LEN);
		e->yAnchor = yAnchor;
		e->xAnchor = xAnchor;
		strncpy(e->canvas, artwork, MAX_SCREEN_WIDTH);
		QACTIVE_POST(AO_RenderArtist, (QEvt *)e, AO_Engine);
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

/**
 * Rotates through test sections.
 * @returns next section key
 */
static const char* next_sec() {
	static int sec = 0;
	switch (sec++) {
	case 0:
		return "tallMid";
	case 1:
		return "topLeft";
	case 2:
		return "left2x2";
	case 3:
		return "top3x3";
	case 4:
		return "topRight";
	case 5:
		return "bot";
	case 6:
		return "right2x2";
	case 7:
		return "bot3x3";
	case 8:
		return "botRight";
	default:
		sec = 0;
		return "tallMid";
	}
}

//////////////////////////////////////////
/// @addtogroup AOEngine
/// @{

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
	QActive_subscribe((QActive *)me, KEY_DETECT_SIG);

	QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC * 5, 0);

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(Engine * const me, QEvt const * const e) {
	switch (e->sig) {
	/// - Q_ENTRY_SIG
	case Q_ENTRY_SIG: {
		configure_screen();
		publish_ENGINE_START();
		return Q_HANDLED();
	}
	/// - @ref ENGINE_START_SIG
	case ENGINE_START_SIG: {
		test_sections();
		return Q_HANDLED();
	}
	/// - @ref TIMEOUT_SIG
	case TIMEOUT_SIG: {
		publish_ENGINE_END();
		return Q_HANDLED();
	}
	/// - @ref ENGINE_END_SIG
	case ENGINE_END_SIG: {
		teardown_screen();
		QF_stop();
		return Q_HANDLED();
	}
	/// - @ref KEY_DETECT_SIG
	case KEY_DETECT_SIG: {
		int key = ((KeyEvt *)e)->key;
		char canvas[MAX_SCREEN_WIDTH];
		snprintf(canvas, MAX_SCREEN_WIDTH, "%d", key);
		post_PAINT_LINE(next_sec(), 0, 0, canvas);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
//////////////////////////////////////////
