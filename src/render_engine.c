/**
 * @file render_engine.c
 * RenderEngine, toot toot.
 */

#include "main.h"
#include <string.h>

static QState RenderEngine_initial(RenderEngine * const me, QEvt const * const e);
static QState Idle(RenderEngine * const me, QEvt const * const e);

////////////////////////////////////

static void post_CREATE_SECTION(RenderSection* section) {
	SectionCfgEvt* e = Q_NEW(SectionCfgEvt, CREATE_SECTION_SIG);
	if (e) {
		memcpy(&e->section, section, sizeof(RenderSection));
		QACTIVE_POST(AO_ScreenPainter, (QEvt *)e, AO_RenderEngine);
	}
}

static void post_PAINT_DIRECT(uint16_t y, uint16_t x, char canvas[MAX_SCREEN_WIDTH]) {
	PaintEvt* e = Q_NEW(PaintEvt, PAINT_DIRECT_SIG);
	if (e) {
		e->yAnchor = y;
		e->xAnchor = x;
		memcpy(e->canvas, canvas, MAX_SCREEN_WIDTH * sizeof(char));
		QACTIVE_POST(AO_ScreenPainter, (QEvt *)e, AO_RenderEngine);
	}
}

/////////////////////////////////////////

static void create_section(RenderEngine* me, RenderSection* section) {
	int idx;
	for (idx = 0; idx < NUM_SECTIONS; idx++) {
		if (me->sections[idx].key[0] == '\0') {
			break;
		}
	}
	if (idx == NUM_SECTIONS) {
		return; // no free sections
	}

	memcpy(&me->sections[idx], section, sizeof(RenderSection));
	post_CREATE_SECTION(section);
}

static void delete_section(RenderEngine* me, char* key) {
	int idx;
	for (idx = 0; idx < NUM_SECTIONS; idx++) {
		if (!strncmp(key, me->sections[idx].key, SECTION_KEY_LEN)) {
			break;
		}
	}
	if (idx == NUM_SECTIONS) {
		return; // section not found
	}

}

//////////////////////////////////////////
///
/// @defgroup AORenderEngine Active Object - RenderEngine
///		States for render engine active object.
///
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static RenderEngine l_renderEngine;
/**Global RenderEngine AO*/
QActive * const AO_RenderEngine = &l_renderEngine.super;

/**
 * Constructor.
 */
void RenderEngine_ctor(void) {
	RenderEngine *me = (RenderEngine *)AO_RenderEngine;
	QActive_ctor(&me->super, Q_STATE_CAST(&RenderEngine_initial));
}

/**
 * Initial.
 */
static QState RenderEngine_initial(RenderEngine * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	QActive_subscribe((QActive*) me, KEY_DETECT_SIG);

	for (int i = 0; i < NUM_SECTIONS; i++) {
		me->sections[i].key[0] = '\0';
	}

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(RenderEngine * const me, QEvt const * const e) {
	switch (e->sig) {
	case CREATE_SECTION_SIG: {
		create_section(me, &((SectionCfgEvt *)e)->section);
		return Q_HANDLED();
	}
	case KEY_DETECT_SIG: {
		int key = ((KeyEvt *)e)->key;
		char canvas[MAX_SCREEN_WIDTH];
		snprintf(canvas, MAX_SCREEN_WIDTH, "%d", key);
		post_PAINT_DIRECT(1, 1, canvas);
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
