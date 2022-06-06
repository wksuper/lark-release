/*
 * Copyright (c) 2020 Kui Wang
 *
 * This file is part of klogging.
 *
 * klogging is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * klogging is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with klogging; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __KLOGGING_H__
#define __KLOGGING_H__

#include <stdint.h>
#include <stddef.h>

#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#if defined(_WIN32)
#define KLOGGING_API __declspec(dllexport)
#else
#define KLOGGING_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t KLoggingOptions;

static const KLoggingOptions KLOGGING_TO_STDOUT = (0x1u << 0);
static const KLoggingOptions KLOGGING_TO_STDERR = (0x1u << 1);
static const KLoggingOptions KLOGGING_TO_LOGCAT = (0x1u << 2);
// static const KLoggingOptions _RESERVED_ = (0x1u << 3);

// static const KLoggingOptions _RESERVED_ = (0x1u << 4);
// static const KLoggingOptions _RESERVED_ = (0x1u << 5);
// static const KLoggingOptions _RESERVED_ = (0x1u << 6);
static const KLoggingOptions KLOGGING_FLUSH_IMMEDIATELY = (0x1u << 7);

// static const KLoggingOptions _RESERVED_ = (0x1u << 8);
// static const KLoggingOptions _RESERVED_ = (0x1u << 9);
// static const KLoggingOptions _RESERVED_ = (0x1u << 10);
// static const KLoggingOptions _RESERVED_ = (0x1u << 11);

static const KLoggingOptions KLOGGING_NO_TIMESTAMP = (0x1u << 12);
static const KLoggingOptions KLOGGING_NO_LOGTYPE = (0x1u << 13);
static const KLoggingOptions KLOGGING_NO_SOURCEFILE = (0x1u << 14);
// static const KLoggingOptions _RESERVED_ = (0x1u << 15);

enum KLoggingLevel {
	KLOGGING_LEVEL_OFF     = 0,
	KLOGGING_LEVEL_FATAL   = 1,
	KLOGGING_LEVEL_ERROR   = 2,
	KLOGGING_LEVEL_WARNING = 3,
	KLOGGING_LEVEL_INFO    = 4,
	KLOGGING_LEVEL_DEBUG   = 5,
	KLOGGING_LEVEL_VERBOSE = 6
};

KLOGGING_API const char *_klogging_version();
KLOGGING_API int _klogging_set(int argc, char *argv[]);
KLOGGING_API int _klogging_set_file(const char *filename);
KLOGGING_API void _klogging_enable_options(KLoggingOptions options);
KLOGGING_API void _klogging_disable_options(KLoggingOptions options);
KLOGGING_API void _klogging_set_level(enum KLoggingLevel level);
KLOGGING_API void _klogging_print(KLoggingOptions enOpts, KLoggingOptions disOpts, const char *lineEnd,
	char type, const char *file, int line, const char *function, const char *logTag,
	const char *format, ...);

// KLOG_SET() is used in the entry of main(int argc, char *argv[]).
// This enables parameters to be set when launching the program who uses klogging.
// Returns 0 on success and a negative value on failure.
static inline int KLOG_SET(int argc, char *argv[]) { return _klogging_set(argc, argv); }

// KLOG_SET_FILE() enables all the logs to be dummped to a file, and disables the dump when filename=NULL.
// Returns 0 on success and a negative value on failure.
static inline int KLOG_SET_FILE(const char *filename) { return _klogging_set_file(filename); }

// KLOG_ENABLE_OPTIONS() enables options.
static inline void KLOG_ENABLE_OPTIONS(KLoggingOptions options) { _klogging_enable_options(options); }

// KLOG_DISABLE_OPTIONS() disables options.
static inline void KLOG_DISABLE_OPTIONS(KLoggingOptions options) { _klogging_disable_options(options); }

// KLOG_SET_LEVEL() sets the logging level.
static inline void KLOG_SET_LEVEL(enum KLoggingLevel level) { _klogging_set_level(level); }

// KLOG_VERSION() returns the version string.
static inline const char *KLOG_VERSION() { return _klogging_version(); }

// klogx() APIs are used when some options need to be temporarily turned ON/OFF for this message
// or a special line-end string(e.g. "") temporarily needed for this message.
#define kloga(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'A', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define klogf(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'F', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define kloge(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'E', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define klogw(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'W', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define klogi(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'I', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define klogd(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'D', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define klogv(enOpts, disOpts, lineEnd, ...) _klogging_print(enOpts, disOpts, lineEnd, 'V', __FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)

// KLOGX() APIs are used for normal message print.

// Print when log level >= KLOGGING_LEVEL_OFF (i.e. Always print regardless of log level)
#define KLOGA(...)    kloga(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_FATAL
#define KLOGF(...)    klogf(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_ERROR
#define KLOGE(...)    kloge(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_WARNING
#define KLOGW(...)    klogw(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_INFO
#define KLOGI(...)    klogi(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_DEBUG
#define KLOGD(...)    klogd(0, 0, NULL, __VA_ARGS__)

// Print when log level >= KLOGGING_LEVEL_VERBOSE
#define KLOGV(...)    klogv(0, 0, NULL, __VA_ARGS__)

#ifdef __cplusplus
} // extern "C"
#endif

#endif
