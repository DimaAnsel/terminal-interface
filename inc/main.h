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

#include "render_artist.h"
#include "screen_painter.h"
#include "utilities.h"

/**
 * Template for active object structures.
 */
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
	AO_RENDER_ARTIST,	///< @see RenderArtist
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
 * Signals passed between active objects.
 */
typedef enum {
	// Subscriptions
	ENGINE_START_SIG = Q_USER_SIG, ///< Program has initialized the screen
	ENGINE_END_SIG,		///< Program is ending
	KEY_DETECT_SIG,		///< Key was detected (posted from KeyMonitor, published from BindingHandler)
	MAX_SUBSCRIBE_SIG,	///< Must be after all subscribe sigs

	// Engine
	TIMEOUT_SIG,		///< Timeout sig

	// RenderArtist
	CREATE_SECTION_SIG,	///< Creates a new section
	DELETE_SECTION_SIG,	///< Deletes a section
	CONFIG_SECTION_SIG,	///< Reconfigures a section
	PAINT_SECTION_SIG,	///< Paints a section
	PAINT_LINE_SIG,	///< Low-level painting signal

	// ScreenPainter
	REFRESH_SCREEN_SIG,	///< Refreshes screen

	// KeyMonitor
	KEY_SCAN_SIG,		///< Checks keyboard input

	MAX_SIG ///< Must always be last
} Signals;

//////////////////////////////
/// @defgroup Events Event types
///	State machine events.
/// @{
//////////////////////////////

/**
 * Section configuration event.
 */
typedef struct {
	/**Super*/
	QEvt	 	  evt;

	RenderSection section; ///< Section configuration
} SectionCfgEvt;

/**
 * Keyboard event.
 */
typedef struct {
	/**Super*/
	QEvt	evt;

	int		key; ///< Numeric key value
} KeyEvt;

/**
 * Paint event.
 */
typedef struct {
	/**Super*/
	QEvt	 evt;

	/**Alphanumeric key used to identify section.*/
	char	 sectionKey[PAINTER_KEY_LEN];

	/**Horizontal anchor (from left)*/
	uint16_t xAnchor;
	/**Vertical anchor (from top)*/
	uint16_t yAnchor;
	/**Line to be painted.*/
	char canvas[MAX_SCREEN_WIDTH];
} PaintEvt;


/**
 * Event class for events with a single primitive.
 */
typedef union {
	QEvt 	  e1; ///< Smallest event
	//! @{
	KeyEvt	  e2;
	//! @}
} TinyEvt;

/**
 * Event class for events with a few primitives.
 */
typedef union {
	TinyEvt		  e1; ///< Next smallest event type
	//! @{
	SectionCfgEvt e2;
	//! @}
} SmallEvt;

/**
 * Event class for medium-sized events.
 */
typedef union {
	SmallEvt e1; ///< Next smallest event type
	//! @{
	PaintEvt e2;
	//! @}
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
//! @{
AO_DEF(KeyMonitor);
//! @}

/**
 * @struct BindingHandler
 * Key binding handler.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} BindingHandler;
//! @{
AO_DEF(BindingHandler);
//! @}

/**
 * @struct ScreenPainter
 * Low-level screen drawing logic.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} ScreenPainter;
//! @{
AO_DEF(ScreenPainter);
//! @}

/**
 * @struct RenderArtist
 * High-level screen drawing logic.
 */
typedef struct {
	/**State machine.*/
	QActive super;

	/**Sections.*/
	RenderLayer layers[NUM_LAYERS];
} RenderArtist;
//! @{
AO_DEF(RenderArtist);
//! @}

/**
 * @ingroup Fwk
 * @struct FileSystem
 * File system interaction logic.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileSystem;
//! @{
AO_DEF(FileSystem);
//! @}

/**
 * @struct FileParser
 * File parser.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileParser;
//! @{
AO_DEF(FileParser);
//! @}

/**
 * @struct FileFramer
 * File parser.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} FileFramer;
//! @{
AO_DEF(FileFramer);
//! @}

/**
 * @struct SaveGenerator
 * Save generator.
 */
typedef struct {
	/**State machine.*/
	QActive super;
} SaveGenerator;
//! @{
AO_DEF(SaveGenerator);
//! @}

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
//! @{
AO_DEF(Engine);
//! @}

//////////////////////////////
///@}
//////////////////////////////

#endif // __MAIN_H
