/**
 * @file render_artist.c
 * RenderArtist, toot toot.
 */

#include <string.h>

#include "main.h"

static QState RenderArtist_initial(RenderArtist * const me, QEvt const * const e);
static QState Idle(RenderArtist * const me, QEvt const * const e);

////////////////////////////////////

/**
 * Initialize a single section.
 */
static void init_section(RenderSection* section) {
	section->key[0] = '\0';
}

/**
 * Initialize a single layer.
 */
static void init_layer(RenderLayer* layer) {
	memset(layer->artwork, 0, MAX_SCREEN_HEIGHT * MAX_SCREEN_WIDTH * sizeof(layer->artwork[0][0]));
	memset(layer->leftEdge, -1, MAX_SCREEN_HEIGHT * sizeof(layer->leftEdge[0]));
	for (int i = 0; i < SECTIONS_PER_LAYER; i++) {
		init_section(&layer->sections[i]);
	}
}

////////////////////////////////////

/**
 * Paints a single line to the screen.
 */
static void post_PAINT_DIRECT(uint16_t y, uint16_t x, char* artwork) {
	PaintEvt* e = Q_NEW(PaintEvt, PAINT_DIRECT_SIG);
	if (e) {
		e->yAnchor = y;
		e->xAnchor = x;
		memcpy(e->canvas, artwork, MAX_SCREEN_WIDTH * sizeof(char));
		QACTIVE_POST(AO_ScreenPainter, (QEvt *)e, AO_RenderArtist);
	}
}

/**
 * Refreshes the screen.
 */
static void post_REFRESH_SCREEN() {
	QEvt* e = Q_NEW(QEvt, REFRESH_SCREEN_SIG);
	if (e) {
		QACTIVE_POST(AO_ScreenPainter, e, AO_RenderArtist);
	}
}

/////////////////////////////////////////

/**
 * Initializes a section.
 */
static void create_section(RenderLayer* layer, RenderSection* section) {
	int idx;
	for (idx = 0; idx < SECTIONS_PER_LAYER; idx++) {
		if (layer->sections[idx].key[0] == '\0') {
			break;
		}
	}
	if (idx == SECTIONS_PER_LAYER) {
		return; // no free sections
	}

	memcpy(&layer->sections[idx], section, sizeof(RenderSection));
}

//////////////////////////////////////////
/// @ingroup Fwk
/// @defgroup AORenderArtist Active Object - RenderArtist
/// States for render engine active object.
/// @{
/////////////////////////////////////////

/**
 * Local reference.
 */
static RenderArtist l_renderEngine;
/**Global RenderArtist AO*/
QActive * const AO_RenderArtist = &l_renderEngine.super;

/**
 * Constructor.
 */
void RenderArtist_ctor(void) {
	RenderArtist *me = (RenderArtist *)AO_RenderArtist;
	QActive_ctor(&me->super, Q_STATE_CAST(&RenderArtist_initial));

	for (int i = 0; i < NUM_LAYERS; i++) {
		init_layer(&me->layers[i]);
	}
}

/**
 * Initial.
 */
static QState RenderArtist_initial(RenderArtist * const me, QEvt const * const e) {
	(void)e; /* unused parameter */

	QActive_subscribe((QActive*) me, KEY_DETECT_SIG);

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(RenderArtist * const me, QEvt const * const e) {
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
		post_REFRESH_SCREEN();
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
