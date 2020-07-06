/**
 * @file main.h
 */

#ifndef __MAIN_H
#define __MAIN_H

#include "qpc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* for exit() */
#include <curses.h>

#include "screen_painter.h"
#include "utilities.h"

#define AO_DEF(NAME) \
		void NAME##_ctor(void);\
		QActive * const AO_##NAME

//////////////////////////////
///
/// @defgroup Fwk Framework
/// 	Active objects and other globally known structures for QP framework.
///
/// @{
///////////////////////////////

/**
 * QP ticks per second.
 */
#define BSP_TICKS_PER_SEC (100)

/**
 * @enum AoPrio
 * Active object priorities in increasing priority.
 */
typedef enum {
	AO_ENGINE = 1,		///< @see Engine
	AO_RENDER_ENGINE,	///< @see RenderEngine
	AO_SCREEN_PAINTER,	///< @see ScreenPainter
	AO_BINDING_HANDLER,	///< @see BindingHandler
	AO_KEY_MONITOR,		///< @see KeyMonitor
	AO_SAVE_GENERATOR,	///< @see SaveGenerator
	AO_FILE_FRAMER,		///< @see FileFramer
	AO_FILE_PARSER,		///< @see FileParser
	AO_FILE_SYSTEM,		///< @see FileSystem
	MAX_AO				///< Must always be last
} AoPrio;

/**
 * @enum Signals
 * unintelligible screaming
 */
typedef enum {
	// Subscribe sigs
	PLACEHOLDER_SIG = Q_USER_SIG, ///< Placeholder
	ENGINE_START_SIG,	///< Engine has set up the screen
	ENGINE_END_SIG,		///< Program is ending
	KEY_DETECT_SIG,		///< Key was detected (posted from {@link KeyMonitor}, published from {@link BindingHandler}
	MAX_SUBSCRIBE_SIG,	///< Must be after all subscribe sigs

	// Engine sigs
	TIMEOUT_SIG,		///< Timeout sig

	// Renderer
	CREATE_SECTION_SIG,	///< Creates a new section
	DELETE_SECTION_SIG,	///< Deletes a section
	CONFIG_SECTION_SIG,	///< Reconfigures a section
	PAINT_SECTION_SIG,	///< Paints a section

	// ScreenPainter
	PAINT_DIRECT_SIG,	///< Low-level painting signal
	REFRESH_SCREEN_SIG,	///< Refreshes screen

	// KeyMonitor
	KEY_SCAN_SIG,		///< Checks keyboard input

	MAX_SIG ///< Must always be last
} Signals;

//////////////////////////////
/// @defgroup Events
///		State machine events.
/// @{
//////////////////////////////

/**
 * Section configuration event.
 */
typedef struct {
	QEvt	 	  evt;
	RenderSection section;
} SectionCfgEvt;

/**
 * Key event.
 */
typedef struct {
	QEvt	evt;
	int		key; ///< Numeric key value
} KeyEvt;

/**
 * Paint event.
 */
typedef struct {
	QEvt	 evt;

	uint16_t xAnchor; ///< Horizontal anchor (from left)
	uint16_t yAnchor; ///< Vertical anchor (from top)
	uint16_t xDim;	  ///< Horizontal size
	uint16_t yDim;	  ///< Vertical size

	char canvas[MAX_SCREEN_WIDTH];
} PaintEvt;


/**
 * Event class for events with a single primitive.
 */
typedef union {
	QEvt 	  e1;
	KeyEvt	  e3;
} TinyEvt;

/**
 * Event class for events with a few primitives.
 */
typedef union {
	TinyEvt		  e1;
	SectionCfgEvt e2;
} SmallEvt;

/**
 * Event class for medium-sized events.
 */
typedef union {
	SmallEvt e1;
	PaintEvt e2;
} MediumEvt;

//////////////////////////////
/// @}
//////////////////////////////

/**
 * @struct KeyMonitor
 * Keyboard monitor.
 */
typedef struct {
	/**State machine.*/
	QActive super;

	/**Key scanner.*/
	QTimeEvt keyScanEvt;
} KeyMonitor;
AO_DEF(KeyMonitor);

/**
 * @struct BindingHandler
 * Key binding handler.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} BindingHandler;
AO_DEF(BindingHandler);

/**
 * @struct ScreenPainter
 * Screen painter.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} ScreenPainter;
AO_DEF(ScreenPainter);

/**
 * @struct RenderEngine
 * Rendering engine.
 */
typedef struct {
	/**State machine.*/
	QActive super;

	/**Sections.*/
	RenderSection sections[NUM_SECTIONS];
} RenderEngine;
AO_DEF(RenderEngine);

/**
 * @ingroup Fwk
 * @struct FileSystem
 * File system interaction logic.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileSystem;
AO_DEF(FileSystem);

/**
 * @struct FileParser
 * File parser.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileParser;
AO_DEF(FileParser);

/**
 * @struct FileFramer
 * File parser.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileFramer;
AO_DEF(FileFramer);

/**
 * @struct SaveGenerator
 * Save generator.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} SaveGenerator;
AO_DEF(SaveGenerator);

/**
 * @struct Engine
 * Central business logic.
 */
typedef struct {
	/**State machine.*/
	QActive super;

	/**Time event.*/
	QTimeEvt timeEvt;
} Engine;
AO_DEF(Engine);

//////////////////////////////
///@}
//////////////////////////////

#endif // __MAIN_H
