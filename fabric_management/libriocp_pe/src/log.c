/*
 * Copyright (c) 2014, Prodrive Technologies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/**
 * @file log.c
 * Logging facility
 * Setting environment variable LOG_FILENAME will write all output to the file
 */
#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "inc/riocp_pe_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

static FILE *riocp_log_file;
static enum riocp_log_level loglevel = RIOCP_LOG_TRACE;
static riocp_log_output_func_t riocp_log_output_func;

/**
 * Initialize logging facility
 * - environment variable RIOCP_LOG
 *   - progname: Write logging to application name (/tmp/{progname}.log)
 *   - stderr: Write logging to stderr
 *   - stdout: Write logging to stdout
 * - environment variable RIOCP_LOGLEVEL (case insensitive, where first character
 *   is also sufficient)
 *   - Error
 *   - Warn
 *   - Info
 *   - Debug
 *   - Trace
 */
static void riocp_log_init(void)
{
	ssize_t len;
	char buf[512] = { 0 };
	char filename[512] = { 0 };
	char *env_filename = getenv("RIOCP_LOG");
	const char *env_loglevel = getenv("RIOCP_LOGLEVEL");

	if (!riocp_log_file) {
		if (env_filename != NULL) {
			if (strcmp(env_filename, "progname") == 0) {
				len = readlink("/proc/self/exe", buf, sizeof(buf));
				if (len == -1) {
					riocp_log_file = stderr;
				} else {
					env_filename = basename(buf);
					snprintf(filename, sizeof(filename) - 1,
						"/tmp/%s.log", env_filename);
					riocp_log_file = fopen(filename, "a+");
				}
			} else if (strcmp(env_filename, "stderr") == 0) {
				riocp_log_file = stderr;
			} else if (strcmp(env_filename, "stdout") == 0) {
				riocp_log_file = stdout;
			} else {
				riocp_log_file = fopen(env_filename, "a+");
			}
		} else if (env_loglevel != NULL) {
			/* Set default riocp_log_file to stderr when log level is set */
			riocp_log_file = stderr;
		}
	}

	if (env_loglevel != NULL) {
		switch (env_loglevel[0]) {
		case 'e':
		case 'E':
			loglevel = RIOCP_LOG_ERROR;
			break;
		case 'w':
		case 'W':
			loglevel = RIOCP_LOG_WARN;
			break;
		case 'i':
		case 'I':
			loglevel = RIOCP_LOG_INFO;
			break;
		case 'd':
		case 'D':
			loglevel = RIOCP_LOG_DEBUG;
			break;
		case 't':
		case 'T':
			loglevel = RIOCP_LOG_TRACE;
			break;
		default:
			loglevel = RIOCP_LOG_NONE;
			break;
		}
	}
}

/**
 * Exit logging facility, close opened logfile
 */
void riocp_log_exit(void)
{
	if (riocp_log_file == NULL || riocp_log_file == stderr
			|| riocp_log_file == stdout)
		return;

	fclose(riocp_log_file);
	riocp_log_file = NULL;
}

/**
 * Convert log level to string representation
 * @param  level Log level
 */
static char *riocp_log_level2str(enum riocp_log_level level)
{
	if (level == RIOCP_LOG_NONE)
		return (char *)"NONE";
	else if (level == RIOCP_LOG_ERROR)
		return (char *)"ERROR";
	else if (level == RIOCP_LOG_WARN)
		return (char *)"WARN";
	else if (level == RIOCP_LOG_INFO)
		return (char *)"INFO";
	else if (level == RIOCP_LOG_DEBUG)
		return (char *)"DEBUG";
	else if (level == RIOCP_LOG_TRACE)
		return (char *)"TRACE";

	return (char *)"Invalid log level";
}

/**
 * Register logging callback function and set log level
 * @param level      Log level
 * @param outputfunc Function to be registered
 */
int RIOCP_SO_ATTR riocp_log_register_callback(enum riocp_log_level level,
	riocp_log_output_func_t outputfunc)
{
	loglevel = level;
	riocp_log_output_func = outputfunc;

	return 0;
}

/**
 * Log a message
 * @param level  Log level
 * @param func   Name of function (e.g __func__)
 * @param file   Filename of line to be logged (e.g __FILE__)
 * @param line   Line number to be logged (e.g __LINE__)
 * @param format Printf style logging format (with vargs)
 */
int RIOCP_SO_ATTR riocp_log(enum riocp_log_level level, const char *func, const char *file,
	const unsigned int line, const char *format, ...)
{
	va_list args;
	char fmt[256];
	char flat[256];

	if (level == RIOCP_LOG_NONE)
		return 0;

	if (!riocp_log_file && riocp_log_output_func == NULL)
		riocp_log_init();

	if (loglevel >= level) {
		va_start(args, format);
		snprintf(fmt, sizeof(fmt), "[%s: %s:%i %s] %s",
			riocp_log_level2str(level), file, line, func, format);
		if (riocp_log_output_func != NULL) {
			vsnprintf(flat, sizeof(flat), fmt, args);
			(*riocp_log_output_func)(level, flat);
		} else {
			if (riocp_log_file)
				vfprintf(riocp_log_file, fmt, args);
		}
		va_end(args);
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
