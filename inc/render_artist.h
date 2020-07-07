/**
 * @file render_artist.h
 */

#ifndef __RENDER_ARTIST_H
#define __RENDER_ARTIST_H

#include <stdint.h>

#include "screen_painter.h"

#define PAINTER_KEY_LEN 16		///< Size of alphanumeric key used to identify sections/layers
#define SECTIONS_PER_LAYER 16	///< Maximum number of sections per layer
#define NUM_LAYERS 4			///< Maximum number of layers


/**
 * @struct RenderSection
 * Logical unit of a layer.
 * Each section is outlined and can be painted individually.
 */
typedef struct {
	/**Alphanumeric key used to identify section.*/
	char	 key[PAINTER_KEY_LEN];

	/**Horizontal anchor (from left)*/
	uint16_t xAnchor;
	/**Vertical anchor (from top)*/
	uint16_t yAnchor;
	/**Horizontal size*/
	uint16_t xDim;
	/**Vertical size*/
	uint16_t yDim;
} RenderSection;

/**
 * @struct RenderLayer
 * Flat image that can take up part or all of a screen.
 * Screen manipulation is done by sections, and the layer stores the
 * compiled image.
 */
typedef struct {
	/**Alphanumeric key used to identify section.*/
	char	key[PAINTER_KEY_LEN];

	/**Left-most edge of each row, used to minimize paint instructions.*/
	int16_t	leftEdge[MAX_SCREEN_HEIGHT];
	/**Compiled screen artwork.*/
	char	artwork[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH];
	/**Sections contained in the layer.*/
	RenderSection sections[SECTIONS_PER_LAYER];
} RenderLayer;


#endif // __RENDER_ARTIST_H
