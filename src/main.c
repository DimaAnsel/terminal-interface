/**
 * @file main.c
 * This is where the spell begins.
 */

#include "main.h"

Q_DEFINE_THIS_FILE

/// @ingroup Fwk
/// @{
/**
 * Crash handler.
 */
void Q_onAssert(char const * const module, int loc) {
	endwin(); // clean up curses
	fprintf(stderr, "Assertion failed in %s:%d", module, loc);
	exit(-1);
}
//! @{
void QF_onStartup(void) {}
void QF_onCleanup(void) {}
//! @}
/**
 * Perform the QF clock tick processing.
 */
void QF_onClockTick(void) {
	QF_TICK_X(0U, (void *)0);
}

static QF_MPOOL_EL(TinyEvt)  l_tinyPoolSto[128];	///< Tiny event pool
static QF_MPOOL_EL(SmallEvt) l_smallPoolSto[64];	///< Small event pool
static QF_MPOOL_EL(MediumEvt) l_mediumPoolSto[32];	///< Medium event pool

static QEvt const *l_engine_queueSto[64];			///< Engine event pool
static QEvt const *l_renderArtist_queueSto[64];		///< RenderArtist event pool
static QEvt const *l_screenPainter_queueSto[64];	///< ScreenPainter event pool
static QEvt const *l_keyMonitor_queueSto[64];		///< KeyMonitor event pool
static QEvt const *l_bindingHandler_queueSto[64];	///< BindingHandler event pool

static QSubscrList l_subscrSto[MAX_SUBSCRIBE_SIG];	///< Subscription manager

/**
 * Initializes framework and starts loop.
 */
int main() {
	clear_log();

	QF_init(); /* initialize the framework */

	// constructors
	Engine_ctor();
	RenderArtist_ctor();
	ScreenPainter_ctor();
	KeyMonitor_ctor();
	BindingHandler_ctor();

	// pools
	QF_poolInit(l_tinyPoolSto,
			sizeof(l_tinyPoolSto),
			sizeof(l_tinyPoolSto[0]));
	QF_poolInit(l_smallPoolSto,
			sizeof(l_smallPoolSto),
			sizeof(l_smallPoolSto[0]));
	QF_poolInit(l_mediumPoolSto,
			sizeof(l_mediumPoolSto),
			sizeof(l_mediumPoolSto[0]));

	// subscription service
	QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));

	// starts
	QACTIVE_START(AO_KeyMonitor,
			AO_KEY_MONITOR, /* priority */
			l_keyMonitor_queueSto, Q_DIM(l_keyMonitor_queueSto),
			(void *)0, 0U, /* no stack */
			(QEvt *)0);    /* no initialization event */
	QACTIVE_START(AO_BindingHandler,
			AO_BINDING_HANDLER, /* priority */
			l_bindingHandler_queueSto, Q_DIM(l_bindingHandler_queueSto),
			(void *)0, 0U, /* no stack */
			(QEvt *)0);    /* no initialization event */
	QACTIVE_START(AO_ScreenPainter,
			AO_SCREEN_PAINTER, /* priority */
			l_screenPainter_queueSto, Q_DIM(l_screenPainter_queueSto),
			(void *)0, 0U, /* no stack */
			(QEvt *)0);    /* no initialization event */
	QACTIVE_START(AO_RenderArtist,
			AO_RENDER_ARTIST, /* priority */
			l_renderArtist_queueSto, Q_DIM(l_renderArtist_queueSto),
			(void *)0, 0U, /* no stack */
			(QEvt *)0);    /* no initialization event */

	// must be last to ensure everyone gets notified of startup
	QACTIVE_START(AO_Engine,
			AO_ENGINE, /* priority */
			l_engine_queueSto, Q_DIM(l_engine_queueSto),
			(void *)0, 0U, /* no stack */
			(QEvt *)0);    /* no initialization event */

	return QF_run(); /* run the QF application */
}
/// @}
