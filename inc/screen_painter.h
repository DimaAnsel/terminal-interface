/**
 * @file screen_painter.h
 */

#ifndef __SCREEN_PAINTER_H
#define __SCREEN_PAINTER_H

#include <stdint.h>


#define SECTION_KEY_LEN 16
#define NUM_SECTIONS 16

#define MAX_SCREEN_HEIGHT 24
#define MAX_SCREEN_WIDTH 80

typedef struct {
	char	 key[SECTION_KEY_LEN]; ///< Key
	uint16_t xAnchor; ///< Horizontal anchor (from left)
	uint16_t yAnchor; ///< Vertical anchor (from top)
	uint16_t xDim;	  ///< Horizontal size
	uint16_t yDim;	  ///< Vertical size
} RenderSection;

typedef enum {
	SECTION_CREATE,
	SECTION_DESTROY,
	SECTION_CONFIG
} SectionAction;

#endif // __SCREEN_PAINTER_H
