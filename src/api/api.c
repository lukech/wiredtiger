/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2008-2011 WiredTiger, Inc.
 *	All rights reserved.
 */

#include "wt_internal.h"

/*
 * __session_close --
 *	WT_SESSION->close method.
 */
static int
__session_close(WT_SESSION *wt_session, const char *config)
{
	BTREE_SESSION *btree_session;
	CONNECTION *conn;
	SESSION *session;
	WT_CURSOR *cursor;
	int ret;

	conn = (CONNECTION *)wt_session->connection;
	session = (SESSION *)wt_session;
	ret = 0;

	while ((cursor = TAILQ_FIRST(&session->cursors)) != NULL)
		WT_TRET(cursor->close(cursor, config));

	while ((btree_session = TAILQ_FIRST(&session->btrees)) != NULL) {
		TAILQ_REMOVE(&session->btrees, btree_session, q);
		WT_TRET(btree_session->btree->close(btree_session->btree, 0));
		__wt_free(session, btree_session);
	}

	TAILQ_REMOVE(&conn->sessions_head, session, q);
	WT_TRET(session->close(session, 0));

	return (0);
}

/*
 * __session_open_cursor --
 *	WT_SESSION->open_cursor method.
 */
static int
__session_open_cursor(WT_SESSION *wt_session,
    const char *uri, WT_CURSOR *to_dup, const char *config, WT_CURSOR **cursorp)
{
	SESSION *session;

	WT_UNUSED(to_dup);

	session = (SESSION *)wt_session;

	if (strncmp(uri, "table:", 6) == 0)
		return (__wt_cursor_open(session, uri, config, cursorp));

	__wt_err(session, 0, "Unknown cursor type '%s'\n", uri);
	return (EINVAL);
}

/*
 * __session_create_table --
 *	WT_SESSION->create_table method.
 */
static int
__session_create_table(WT_SESSION *wt_session,
    const char *name, const char *config)
{
	BTREE *btree;
	CONNECTION *conn;
	SESSION *session;

	WT_UNUSED(config);

	session = (SESSION *)wt_session;
	conn = (CONNECTION *)wt_session->connection;

	WT_RET(conn->btree(conn, 0, &btree));
	WT_RET(btree->open(btree, name, 0666, WT_CREATE));

	WT_RET(__wt_session_add_btree(session, btree));

	return (0);
}

/*
 * __session_rename_table --
 *	WT_SESSION->rename_table method.
 */
static int
__session_rename_table(WT_SESSION *wt_session,
    const char *oldname, const char *newname, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(oldname);
	WT_UNUSED(newname);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __session_drop_table --
 *	WT_SESSION->drop_table method.
 */
static int
__session_drop_table(
    WT_SESSION *wt_session, const char *name, const char *config)
{
	SESSION *session;
	WT_CONFIG_ITEM cvalue;
	int force, ret;

	WT_UNUSED(wt_session);
	WT_UNUSED(name);
	WT_UNUSED(config);

	session = (SESSION *)wt_session;
	force = 0;

	CONFIG_LOOP(session, config, cvalue)
		CONFIG_ITEM("force")
			force = (cvalue.val != 0);
	CONFIG_END(session);

	/* TODO: Combine the table name with the conn home to make a filename. */

	ret = remove(name);

	return (force ? 0 : ret);
}

/*
 * __session_truncate_table --
 *	WT_SESSION->truncate_table method.
 */
static int
__session_truncate_table(WT_SESSION *wt_session,
    const char *name, WT_CURSOR *start, WT_CURSOR *end, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(name);
	WT_UNUSED(start);
	WT_UNUSED(end);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __session_verify_table --
 *	WT_SESSION->verify_table method.
 */
static int
__session_verify_table(WT_SESSION *wt_session, const char *name, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(name);
	WT_UNUSED(config);

	return (0);
}

/*
 * __session_begin_transaction --
 *	WT_SESSION->begin_transaction method.
 */
static int
__session_begin_transaction(WT_SESSION *wt_session, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __session_commit_transaction --
 *	WT_SESSION->commit_transaction method.
 */
static int
__session_commit_transaction(WT_SESSION *wt_session, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __session_rollback_transaction --
 *	WT_SESSION->rollback_transaction method.
 */
static int
__session_rollback_transaction(WT_SESSION *wt_session, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __session_checkpoint --
 *	WT_SESSION->checkpoint method.
 */
static int
__session_checkpoint(WT_SESSION *wt_session, const char *config)
{
	WT_UNUSED(wt_session);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __conn_load_extension --
 *	WT_CONNECTION->load_extension method.
 */
static int
__conn_load_extension(WT_CONNECTION *wt_conn, const char *path, const char *config)
{
	WT_UNUSED(wt_conn);
	WT_UNUSED(path);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __conn_add_cursor_type --
 *	WT_CONNECTION->add_cursor_type method.
 */
static int
__conn_add_cursor_type(WT_CONNECTION *wt_conn,
    const char *prefix, WT_CURSOR_TYPE *ctype, const char *config)
{
	WT_UNUSED(wt_conn);
	WT_UNUSED(prefix);
	WT_UNUSED(ctype);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __conn_add_collator --
 *	WT_CONNECTION->add_collator method.
 */
static int
__conn_add_collator(WT_CONNECTION *wt_conn,
    const char *name, WT_COLLATOR *collator, const char *config)
{
	WT_UNUSED(wt_conn);
	WT_UNUSED(name);
	WT_UNUSED(collator);
	WT_UNUSED(config);

	return (ENOTSUP);
}

/*
 * __conn_add_extractor --
 *	WT_CONNECTION->add_extractor method.
 */
static int
__conn_add_extractor(WT_CONNECTION *wt_conn,
    const char *name, WT_EXTRACTOR *extractor, const char *config)
{
	WT_UNUSED(wt_conn);
	WT_UNUSED(name);
	WT_UNUSED(extractor);
	WT_UNUSED(config);

	return (ENOTSUP);
}

static const char *
__conn_get_home(WT_CONNECTION *wt_conn)
{
	return (((CONNECTION *)wt_conn)->home);
}

/*
 * __conn_is_new --
 *	WT_CONNECTION->is_new method.
 */
static int
__conn_is_new(WT_CONNECTION *wt_conn)
{
	WT_UNUSED(wt_conn);

	return (0);
}

/*
 * __conn_close --
 *	WT_CONNECTION->close method.
 */
static int
__conn_close(WT_CONNECTION *wt_conn, const char *config)
{
	int ret;
	CONNECTION *conn;
	SESSION *session;
	WT_SESSION *wt_session;

	ret = 0;
	conn = (CONNECTION *)wt_conn;

	while ((session = TAILQ_FIRST(&conn->sessions_head)) != NULL) {
		wt_session = &session->iface;
		WT_TRET(wt_session->close(wt_session, config));
	}

	__wt_free(&conn->default_session, conn->home);
	WT_TRET(conn->close(conn, 0));
	return (ret);
}

/*
 * __conn_open_session --
 *	WT_CONNECTION->open_session method.
 */
static int
__conn_open_session(WT_CONNECTION *wt_conn,
    WT_ERROR_HANDLER *error_handler, const char *config, WT_SESSION **wt_sessionp)
{
	static WT_SESSION stds = {
		NULL,
		__session_close,
		__session_open_cursor,
		__session_create_table,
		__session_rename_table,
		__session_drop_table,
		__session_truncate_table,
		__session_verify_table,
		__session_begin_transaction,
		__session_commit_transaction,
		__session_rollback_transaction,
		__session_checkpoint,
	};
	CONNECTION *conn;
	SESSION *session;
	int ret;

	WT_UNUSED(config);

	conn = (CONNECTION *)wt_conn;

	WT_ERR(conn->session(conn, 0, &session));
	/*
	 * XXX
	 * Kludge while there is a separate __wt_conection_session method.
	 * We shouldn't be overwriting the connection pointer, particularly not
	 * through a static struct that is shared between threads.
	 */
	stds.connection = wt_conn;
	session->iface = stds;
	TAILQ_INIT(&session->cursors);
	TAILQ_INIT(&session->btrees);
	WT_ASSERT(NULL, conn->default_session.error_handler != NULL);
	if (error_handler != NULL)
		session->error_handler = error_handler;

	TAILQ_INSERT_HEAD(&conn->sessions_head, session, q);

	STATIC_ASSERT(offsetof(CONNECTION, iface) == 0);
	*wt_sessionp = &session->iface;

	if (0) {
err:		if (session != NULL)
			(void)__wt_session_close(session);
		__wt_free(&conn->default_session, session);
	}

	return (0);
}

/*
 * wiredtiger_open --
 *	Main library entry point: open a new connection to a WiredTiger
 *	database.
 */
int
wiredtiger_open(const char *home, WT_ERROR_HANDLER *error_handler,
    const char *config, WT_CONNECTION **wt_connp)
{
	static int library_init = 0;
	static WT_CONNECTION stdc = {
		__conn_load_extension,
		__conn_add_cursor_type,
		__conn_add_collator,
		__conn_add_extractor,
		__conn_close,
		__conn_get_home,
		__conn_is_new,
		__conn_open_session
	};
	CONNECTION *conn;
	int ret;

	WT_UNUSED(config);

	*wt_connp = NULL;

	if (error_handler == NULL)
		error_handler = __wt_error_handler_default;

	/*
	 * We end up here before we do any real work.   Check the build itself,
	 * and do some global stuff.
	 */
	if (library_init == 0) {
		WT_RET(__wt_library_init());
		library_init = 1;
	}

	/*
	 * !!!
	 * We don't yet have a session handle to pass to the memory allocation
	 * functions.
	 */
	WT_RET(__wt_calloc(NULL, 1, sizeof(CONNECTION), &conn));
	conn->iface = stdc;
	WT_ERR(__wt_strdup(NULL, home, &conn->home));
	TAILQ_INIT(&conn->sessions_head);

	conn->default_session.iface.connection = &conn->iface;
	conn->default_session.error_handler = error_handler;

	/* XXX conn flags, including WT_MEMORY_CHECK */
	WT_ERR(__wt_connection_config(conn));

	/* XXX configure cache size */

	WT_ERR(conn->open(conn, home, 0644, 0));

	STATIC_ASSERT(offsetof(CONNECTION, iface) == 0);
	*wt_connp = &conn->iface;

	if (0) {
err:		if (conn->home != NULL)
			__wt_free(NULL, conn);
		conn->close(conn, 0);
		__wt_free(NULL, conn);
	}

	return (ret);
}
