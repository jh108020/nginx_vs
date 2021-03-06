
/*
 * Copyright (C) Igor Sysoev
 */


#ifndef _NGX_LOG_H_INCLUDED_
#define _NGX_LOG_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>

//日志级别，由低到高，1-8
#define NGX_LOG_STDERR            0
#define NGX_LOG_EMERG             1
#define NGX_LOG_ALERT             2
#define NGX_LOG_CRIT              3
#define NGX_LOG_ERR               4
#define NGX_LOG_WARN              5
#define NGX_LOG_NOTICE            6
#define NGX_LOG_INFO              7
#define NGX_LOG_DEBUG             8

#define NGX_LOG_DEBUG_CORE        0x010		//Nginx核心模块的调试日志
#define NGX_LOG_DEBUG_ALLOC       0x020		//nginx在分配内存时使用的调试日志
#define NGX_LOG_DEBUG_MUTEX       0x040		//nginx在使用进程锁时使用的调试日志
#define NGX_LOG_DEBUG_EVENT       0x080		//nginx事件模块的调试日志
#define NGX_LOG_DEBUG_HTTP        0x100		//nginx http模块的调试日志
#define NGX_LOG_DEBUG_MAIL        0x200		//nginx 邮件模块的调试日志
#define NGX_LOG_DEBUG_MYSQL       0x400		//表示与mysql相关的nginx模块所使用的调试日志

/*
 * do not forget to update debug_levels[] in src/core/ngx_log.c
 * after the adding a new debug level
 */

#define NGX_LOG_DEBUG_FIRST       NGX_LOG_DEBUG_CORE
#define NGX_LOG_DEBUG_LAST        NGX_LOG_DEBUG_MYSQL
#define NGX_LOG_DEBUG_CONNECTION  0x80000000
#define NGX_LOG_DEBUG_ALL         0x7ffffff0

#ifdef NGX_LOG_MODULE

enum LOG_LEVEL
{
    // = Note, this first argument *must* start at 1!

    /// Shutdown the logger (decimal 1).
    LOG_SHUTDOWN = 01,

    /// Messages indicating function-calling sequence (decimal 2).
    LOG_TRACE = 02,

    /// Messages that contain information normally of use only when
    /// debugging a program (decimal 4).
    LOG_DEBUG = 04,

    /// Informational messages (decimal 8).
    LOG_INFO = 010,

    /// Conditions that are not error conditions, but that may require
    /// special handling (decimal 16).
    LOG_NOTICE = 020,

    /// Warning messages (decimal 32).
    LOG_WARNING = 040,

    /// Initialize the logger (decimal 64).
    LOG_STARTUP = 0100,

    /// Error messages (decimal 128).
    LOG_ERROR = 0200,

    /// Critical conditions, such as hard device errors (decimal 256).
    LOG_CRITICAL = 0400,

    /// A condition that should be corrected immediately, such as a
    /// corrupted system database (decimal 512).
    LOG_ALERT = 01000,

    /// A panic condition.  This is normally broadcast to all users
    /// (decimal 1024).
    LOG_EMERGENCY = 02000,

    /// The maximum logging priority.
    LOG_MAX = LOG_EMERGENCY,

    /// Do not use!!  This enum value ensures that the underlying
    /// integral type for this enum is at least 32 bits.
    LOG_ENSURE_32_BITS = 0x7FFFFFFF
};

#endif


typedef u_char *(*ngx_log_handler_pt) (ngx_log_t *log, u_char *buf, size_t len);


struct ngx_log_s {
    ngx_uint_t           log_level;//日志级别或者日志类型
    ngx_open_file_t     *file;		//日志文件

    ngx_atomic_uint_t    connection;//连接数，不为0时会输出到日志中

    ngx_log_handler_pt   handler;
    void                *data;

    /*
     * we declare "action" as "char *" because the actions are usually
     * the static strings and in the "u_char *" case we have to override
     * their types all the time
     */

    char                *action;
};


#define NGX_MAX_ERROR_STR   2048


/*********************************/

#if (NGX_HAVE_C99_VARIADIC_MACROS)

#define NGX_HAVE_VARIADIC_MACROS  1

#define ngx_log_error(level, log, ...)                                        \
    if ((log)->log_level >= level) ngx_log_error_core(level, log, __VA_ARGS__)

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...);

#define ngx_log_debug(level, log, ...)                                        \
    if ((log)->log_level & level)                                             \
        ngx_log_error_core(NGX_LOG_DEBUG, log, __VA_ARGS__)

/*********************************/

#elif (NGX_HAVE_GCC_VARIADIC_MACROS)

#define NGX_HAVE_VARIADIC_MACROS  1

#define ngx_log_error(level, log, args...)                                    \
    if ((log)->log_level >= level) ngx_log_error_core(level, log, args)

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...);

#define ngx_log_debug(level, log, args...)                                    \
    if ((log)->log_level & level)                                             \
        ngx_log_error_core(NGX_LOG_DEBUG, log, args)

/*********************************/

#else /* NO VARIADIC MACROS */

#define NGX_HAVE_VARIADIC_MACROS  0
//fmt就是可变参数
void ngx_cdecl ngx_log_error(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...);
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, va_list args);
void ngx_cdecl ngx_log_debug_core(ngx_log_t *log, ngx_err_t err,
    const char *fmt, ...);


#endif /* VARIADIC MACROS */


/*********************************/

#if (NGX_DEBUG)

#if (NGX_HAVE_VARIADIC_MACROS)

#define ngx_log_debug0  ngx_log_debug
#define ngx_log_debug1  ngx_log_debug
#define ngx_log_debug2  ngx_log_debug
#define ngx_log_debug3  ngx_log_debug
#define ngx_log_debug4  ngx_log_debug
#define ngx_log_debug5  ngx_log_debug
#define ngx_log_debug6  ngx_log_debug
#define ngx_log_debug7  ngx_log_debug
#define ngx_log_debug8  ngx_log_debug


#else /* NO VARIADIC MACROS */

#define ngx_log_debug0(level, log, err, fmt)                                  \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt)

#define ngx_log_debug1(level, log, err, fmt, arg1)                            \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1)

#define ngx_log_debug2(level, log, err, fmt, arg1, arg2)                      \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1, arg2)

#define ngx_log_debug3(level, log, err, fmt, arg1, arg2, arg3)                \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1, arg2, arg3)

#define ngx_log_debug4(level, log, err, fmt, arg1, arg2, arg3, arg4)          \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1, arg2, arg3, arg4)

#define ngx_log_debug5(level, log, err, fmt, arg1, arg2, arg3, arg4, arg5)    \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1, arg2, arg3, arg4, arg5)

#define ngx_log_debug6(level, log, err, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6)                    \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt, arg1, arg2, arg3, arg4, arg5, arg6)

#define ngx_log_debug7(level, log, err, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7)              \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt,                                     \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define ngx_log_debug8(level, log, err, fmt,                                  \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)        \
    if ((log)->log_level & level)                                             \
        ngx_log_debug_core(log, err, fmt,                                     \
                       arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)

#endif

#else /* NO NGX_DEBUG */

#define ngx_log_debug0(level, log, err, fmt)
#define ngx_log_debug1(level, log, err, fmt, arg1)
#define ngx_log_debug2(level, log, err, fmt, arg1, arg2)
#define ngx_log_debug3(level, log, err, fmt, arg1, arg2, arg3)
#define ngx_log_debug4(level, log, err, fmt, arg1, arg2, arg3, arg4)
#define ngx_log_debug5(level, log, err, fmt, arg1, arg2, arg3, arg4, arg5)
#define ngx_log_debug6(level, log, err, fmt, arg1, arg2, arg3, arg4, arg5, arg6)
#define ngx_log_debug7(level, log, err, fmt, arg1, arg2, arg3, arg4, arg5,    \
                       arg6, arg7)
#define ngx_log_debug8(level, log, err, fmt, arg1, arg2, arg3, arg4, arg5,    \
                       arg6, arg7, arg8)

#endif

/*********************************/

ngx_log_t *ngx_log_init(u_char *prefix);
ngx_log_t *ngx_log_create(ngx_cycle_t *cycle, ngx_str_t *name);
char *ngx_log_set_levels(ngx_conf_t *cf, ngx_log_t *log);
void ngx_cdecl ngx_log_abort(ngx_err_t err, const char *fmt, ...);
void ngx_cdecl ngx_log_stderr(ngx_err_t err, const char *fmt, ...);
u_char *ngx_log_errno(u_char *buf, u_char *last, ngx_err_t err);


extern ngx_module_t  ngx_errlog_module;
extern ngx_uint_t    ngx_use_stderr;


#ifdef NGX_LOG_MODULE

#ifndef NGX_LOG
#define NGX_LOG(level, fmt, ...) ngx_log_stdout(level, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

char *ngx_log_level(int level);

void ngx_cdecl ngx_log_stdout(int level, const char *func, const int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#else

#define NGX_LOG(level, fmt, ...)

#endif

#endif /* _NGX_LOG_H_INCLUDED_ */
