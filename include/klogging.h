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
#include <stdio.h>

#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#if defined(_WIN32)
#define KLOGGING_API __declspec(dllexport)
#else
#define KLOGGING_API
#endif

typedef uint16_t KLoggingOptions;

static const KLoggingOptions KLOGGING_TO_STDOUT = (0x1 << 0);
static const KLoggingOptions KLOGGING_TO_STDERR = (0x1 << 1);
static const KLoggingOptions KLOGGING_TO_LOGCAT = (0x1 << 2);
// static const KLoggingOptions _RESERVED_ = (0x1 << 3);

// static const KLoggingOptions _RESERVED_ = (0x1 << 4);
// static const KLoggingOptions _RESERVED_ = (0x1 << 5);
// static const KLoggingOptions _RESERVED_ = (0x1 << 6);
static const KLoggingOptions KLOGGING_FLUSH_IMMEDIATELY = (0x1 << 7);

// static const KLoggingOptions _RESERVED_ = (0x1 << 8);
// static const KLoggingOptions _RESERVED_ = (0x1 << 9);
// static const KLoggingOptions _RESERVED_ = (0x1 << 10);
// static const KLoggingOptions _RESERVED_ = (0x1 << 11);

static const KLoggingOptions KLOGGING_NO_TIMESTAMP = (0x1 << 12);
static const KLoggingOptions KLOGGING_NO_LOGTYPE = (0x1 << 13);
static const KLoggingOptions KLOGGING_NO_SOURCEFILE = (0x1 << 14);
// static const KLoggingOptions _RESERVED_ = (0x1 << 15);

enum KLoggingLevel {
	KLOGGING_LEVEL_OFF = 0,
	KLOGGING_LEVEL_ERROR = 1,
	KLOGGING_LEVEL_WARNING = 2,
	KLOGGING_LEVEL_INFO = 3,
	KLOGGING_LEVEL_DEBUG = 4,
	KLOGGING_LEVEL_VERBOSE = 5
};

#ifdef __cplusplus

const char *_cpp_klogging_version();

// CPP APIs
class KLOGGING_API KLogging {
public:
	KLogging();

	int Set(int argc, char *argv[]);
	int SetFile(const char *filename);
	void SetOptions(KLoggingOptions options) { m_options = options; }
	KLoggingOptions GetOptions() const { return m_options; }
	void SetLevel(KLoggingLevel level) { m_level = level; }
	int SetLineEnd(const char *end);

	void c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void c(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);
	void e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void e(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);
	void w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void w(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);
	void i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void i(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);
	void d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void d(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);
	void v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
	void v(const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);

	~KLogging();

private:
	KLogging(const KLogging &);
	KLogging &operator=(const KLogging &);
	inline bool IsToConsole() const { return m_options & (KLOGGING_TO_STDOUT | KLOGGING_TO_STDERR); }
	inline bool CanPrintError() const { return m_level >= KLOGGING_LEVEL_ERROR; }
	inline bool CanPrintWarning() const { return m_level >= KLOGGING_LEVEL_WARNING; }
	inline bool CanPrintInfo() const { return m_level >= KLOGGING_LEVEL_INFO; }
	inline bool CanPrintDebug() const { return m_level >= KLOGGING_LEVEL_DEBUG; }
	inline bool CanPrintVerbose() const { return m_level >= KLOGGING_LEVEL_VERBOSE; }
	void Print(char type, const char *file, int line, const char *function, const char *log_tag, const char *format, va_list args);

	FILE *m_file;
	KLoggingOptions m_options;
	KLoggingLevel m_level;
	char m_lineEnd[32];
};

extern KLogging _klogging;

static inline int KLOG_SET(int argc, char *argv[]) { return _klogging.Set(argc, argv); }
static inline int KLOG_SET_FILE(const char *filename) { return _klogging.SetFile(filename); }
static inline void KLOG_SET_OPTIONS(KLoggingOptions options) { _klogging.SetOptions(options); }
static inline KLoggingOptions KLOG_GET_OPTIONS() { return _klogging.GetOptions(); }
static inline void KLOG_SET_LEVEL(enum KLoggingLevel level) { _klogging.SetLevel(level); }
static inline int KLOG_SET_LINEEND(const char *end) { return _klogging.SetLineEnd(end); }
#define KVERSION()        _cpp_klogging_version()
#define KCONSOLE(...) _klogging.c(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGE(...)    _klogging.e(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGW(...)    _klogging.w(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGI(...)    _klogging.i(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGD(...)    _klogging.d(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGV(...)    _klogging.v(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)

#else

/* C APIs */
const char *_klogging_version();
int _klogging_set(int argc, char *argv[]);
int _klogging_set_file(const char *filename);
void _klogging_set_options(KLoggingOptions options);
KLoggingOptions _klogging_get_options();
void _klogging_set_level(enum KLoggingLevel level);
int _klogging_set_lineend(const char *end);
void _klogging_c(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_e(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_w(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_i(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_d(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);
void _klogging_v(const char *file, int line, const char *function, const char *log_tag, const char *format, ...);

static inline int KLOG_SET(int argc, char *argv[]) { return _klogging_set(argc, argv); }
static inline int KLOG_SET_FILE(const char *filename) { return _klogging_set_file(filename); }
static inline void KLOG_SET_OPTIONS(KLoggingOptions options) { _klogging_set_options(options); }
static inline KLoggingOptions KLOG_GET_OPTIONS() { return _klogging_get_options(); }
static inline void KLOG_SET_LEVEL(enum KLoggingLevel level) { _klogging_set_level(level); }
static inline int KLOG_SET_LINEEND(const char *end) { return _klogging_set_lineend(end); }
#define KVERSION()        _klogging_version()
#define KCONSOLE(...)  _klogging_c(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGE(...)    _klogging_e(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGW(...)    _klogging_w(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGI(...)    _klogging_i(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGD(...)    _klogging_d(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)
#define KLOGV(...)    _klogging_v(__FILE__, __LINE__, __FUNCTION__, LOG_TAG, __VA_ARGS__)

#endif

#endif
