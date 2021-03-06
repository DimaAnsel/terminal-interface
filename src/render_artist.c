/**
 * @file render_artist.c
 * RenderArtist, toot toot.
 */

#include <string.h>

#include "main.h"

static QState RenderArtist_initial(RenderArtist * const me, QEvt const * const e);
static QState Idle(RenderArtist * const me, QEvt const * const e);

//////////////////////////////////////////
/// @ingroup Fwk
/// @defgroup AORenderArtist Active Object - RenderArtist
/// States for render artist active object.
/// @{
////////////////////////////////////

/**
 * Paints a single line to the screen.
 *
 * @ref PAINT_LINE_SIG, @ref AOScreenPainter
 *
 * @param[in] yAnchor Vertical anchor (from top)
 * @param[in] xAnchor Horizontal anchor (from left)
 * @param[in] artwork String to draw
 */
static void post_PAINT_LINE(uint16_t yAnchor, uint16_t xAnchor, char* artwork) {
	PaintEvt* e = Q_NEW(PaintEvt, PAINT_LINE_SIG);
	if (e) {
		e->yAnchor = yAnchor;
		e->xAnchor = xAnchor;
		strncpy(e->canvas, artwork, MAX_SCREEN_WIDTH);
		QACTIVE_POST(AO_ScreenPainter, (QEvt *)e, AO_RenderArtist);
	}
}

/**
 * Refreshes the screen.
 *
 * @ref REFRESH_SCREEN_SIG, @ref AOScreenPainter
 */
static void post_REFRESH_SCREEN() {
	QEvt* e = Q_NEW(QEvt, REFRESH_SCREEN_SIG);
	if (e) {
		QACTIVE_POST(AO_ScreenPainter, e, AO_RenderArtist);
	}
}

/// @}
/////////////////////////////////////////

/**
 * Initialize a single section.
 *
 * @param[out] section Section to be initialized
 */
static void init_section(RenderSection* section) {
	section->key[0] = '\0';
}

/**
 * Initialize a single layer.
 *
 * @param[out] layer Layer to be initialized
 */
static void init_layer(RenderLayer* layer) {
	memset(layer->artwork, '\0', MAX_SCREEN_HEIGHT * MAX_SCREEN_WIDTH * sizeof(layer->artwork[0][0]));
	memset(layer->leftEdge, -1, MAX_SCREEN_HEIGHT * sizeof(layer->leftEdge[0]));
	for (int i = 0; i < SECTIONS_PER_LAYER; i++) {
		init_section(&layer->sections[i]);
	}
}

/**
 * Draws a border if the location isn't already a corner for another section.
 *
 * @param[in,out] loc	 Location to put border
 * @param[in]	  border Character to use for the border
 */
static inline void coalesce_outline(char* loc, char border) {
	if (*loc != '+') {
		*loc = border;
	}
}

/**
 * Draws a blank section.
 *
 * @param[in,out] layer		Layer where section is drawn
 * @param[in]	  leftEdge	Leftmost column (lowest x)
 * @param[in]	  topEdge	Topmost row (lowest y)
 * @param[in]	  rightEdge	Rightmost column (highest x)
 * @param[in]	  botEdge	Bottom-most row (highest y)
 */
static void draw_blank_section(RenderLayer* layer, int leftEdge, int topEdge, int rightEdge, int botEdge) {
	layer->artwork[topEdge][leftEdge] = '+';
	layer->artwork[topEdge][rightEdge] = '+';
	layer->artwork[botEdge][leftEdge] = '+';
	layer->artwork[botEdge][rightEdge] = '+';

	for (int row = topEdge + 1; row < botEdge; row++) {
		coalesce_outline(&layer->artwork[row][leftEdge], '|');
		coalesce_outline(&layer->artwork[row][rightEdge], '|');
		memset(&layer->artwork[row][leftEdge + 1], ' ', rightEdge - leftEdge - 1);
	}
	for (int col = leftEdge + 1; col < rightEdge; col++) {
		coalesce_outline(&layer->artwork[topEdge][col], '-');
		coalesce_outline(&layer->artwork[botEdge][col], '-');
	}
}

/**
 * Initializes a section and draws on the screen.
 *
 * @param[in,out] layer   Layer that should contain the new section
 * @param[in]	  section Section to be added
 */
static void create_section(RenderLayer* layer, RenderSection* section) {
	int idx;
	int leftEdge = section->xAnchor - 1;
	int topEdge = section->yAnchor - 1;
	int rightEdge = section->xAnchor + section->xDim;
	int botEdge = section->yAnchor + section->yDim;
	if (leftEdge < 0 || topEdge < 0 || rightEdge >= MAX_SCREEN_WIDTH || botEdge >= MAX_SCREEN_HEIGHT) {
		return;
	}

	for (idx = 0; idx < SECTIONS_PER_LAYER; idx++) {
		if (layer->sections[idx].key[0] == '\0') {
			break;
		}
	}
	if (idx == SECTIONS_PER_LAYER) {
		return; // no free sections
	}

	memcpy(&layer->sections[idx], section, sizeof(RenderSection));

	for (int row = topEdge; row <= botEdge; row++) {
		if (leftEdge < layer->leftEdge[row]) {
			layer->leftEdge[row] = leftEdge;
		}
	}

	draw_blank_section(layer, leftEdge, topEdge, rightEdge, botEdge);

	for (int row = topEdge; row <= botEdge; row++) {
		post_PAINT_LINE(row, leftEdge, &layer->artwork[row][leftEdge]);
	}
	post_REFRESH_SCREEN();
}

/**
 * Looks up a section by its key.
 *
 * @param[in] layer		 Layer to search
 * @param[in] sectionKey Section key to find
 *
 * @returns Pointer to section, or NULL on failure
 */
static RenderSection* get_section(RenderLayer* layer, char* sectionKey) {
	RenderSection* ret = NULL;
	for (int i = 0; i < SECTIONS_PER_LAYER; i++) {
		if (layer->sections[i].key[0] == '\0') { break; }
		if (!strncmp(layer->sections[i].key, sectionKey, PAINTER_KEY_LEN)) {
			ret = &layer->sections[i];
			break;
		}
	}
	return ret;
}

/**
 * Draws a single line in a section.
 *
 * @param[in,out] layer Layer to be updated
 * @param[in]	  e		Paint event
 */
static void draw_section_line(RenderLayer* layer, PaintEvt* e) {
	RenderSection* section = get_section(layer, e->sectionKey);
	if (section == NULL) { return; }

	int yAnchor = section->yAnchor + e->yAnchor;
	int xAnchor = section->xAnchor + e->xAnchor;
	int size = strnlen(e->canvas, section->xDim);
	memcpy(&layer->artwork[yAnchor][xAnchor], e->canvas, size * sizeof(char));

	post_PAINT_LINE(yAnchor, xAnchor, &layer->artwork[yAnchor][xAnchor]);
	post_REFRESH_SCREEN();
}

//////////////////////////////////////////
/// @addtogroup AORenderArtist
/// @{

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

	return Q_TRAN(&Idle);
}

/**
 * Idle state.
 */
static QState Idle(RenderArtist * const me, QEvt const * const e) {
	switch (e->sig) {
	/// - @ref CREATE_SECTION_SIG
	case CREATE_SECTION_SIG: {
		create_section(&me->layers[0], &((SectionCfgEvt *)e)->section);
		return Q_HANDLED();
	}
	/// - @ref PAINT_LINE_SIG
	case PAINT_LINE_SIG: {
		draw_section_line(&me->layers[0], ((PaintEvt *)e));
		return Q_HANDLED();
	}
	}
	return Q_SUPER(&QHsm_top);
}

/// @}
//////////////////////////////////////////
