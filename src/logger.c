#include <stdlib.h>  /* calloc, free */
#include <string.h>  /* strcmp */
#ifndef _WIN32
#include <syslog.h>
#endif  /* !_WIN32 */
#include <unistd.h>  /* getpid */

#include "error.h"
#include "common.h"
#include "config.h"
#include "logger.h"

static const char* bud_log_level_str(bud_log_level_t level);


bud_error_t bud_logger_new(bud_config_t* config) {
#ifndef _WIN32
  int facility;
#endif  /* !_WIN32 */

  config->logger = calloc(1, sizeof(*config->logger));
  if (strcmp(config->log.level, "debug") == 0)
    config->logger->level = kBudLogDebug;
  else if (strcmp(config->log.level, "notice") == 0)
    config->logger->level = kBudLogNotice;
  else if (strcmp(config->log.level, "fatal") == 0)
    config->logger->level = kBudLogFatal;
  else if (strcmp(config->log.level, "warning") == 0)
    config->logger->level = kBudLogWarning;
  else
    config->logger->level = kBudLogInfo;
  config->logger->stdio_enabled = config->log.stdio;
  config->logger->syslog_enabled = config->log.syslog;

#ifndef _WIN32
  if (config->logger->syslog_enabled) {
    if (strcmp(config->log.facility, "auth") == 0)
      facility = LOG_AUTH;
    else if (strcmp(config->log.facility, "cron") == 0)
      facility = LOG_CRON;
    else if (strcmp(config->log.facility, "kern") == 0)
      facility = LOG_KERN;
    else if (strcmp(config->log.facility, "lpr") == 0)
      facility = LOG_LPR;
    else if (strcmp(config->log.facility, "mail") == 0)
      facility = LOG_MAIL;
    else if (strcmp(config->log.facility, "news") == 0)
      facility = LOG_NEWS;
    else if (strcmp(config->log.facility, "syslog") == 0)
      facility = LOG_SYSLOG;
    else if (strcmp(config->log.facility, "daemon") == 0)
      facility = LOG_DAEMON;
    else if (strcmp(config->log.facility, "uucp") == 0)
      facility = LOG_UUCP;
    else if (strcmp(config->log.facility, "local0") == 0)
      facility = LOG_LOCAL0;
    else if (strcmp(config->log.facility, "local1") == 0)
      facility = LOG_LOCAL1;
    else if (strcmp(config->log.facility, "local2") == 0)
      facility = LOG_LOCAL2;
    else if (strcmp(config->log.facility, "local3") == 0)
      facility = LOG_LOCAL3;
    else if (strcmp(config->log.facility, "local4") == 0)
      facility = LOG_LOCAL4;
    else if (strcmp(config->log.facility, "local5") == 0)
      facility = LOG_LOCAL5;
    else if (strcmp(config->log.facility, "local6") == 0)
      facility = LOG_LOCAL6;
    else if (strcmp(config->log.facility, "local7") == 0)
      facility = LOG_LOCAL7;
    else
      facility = LOG_USER;
    openlog("bud", LOG_PID | LOG_NDELAY, facility);
  }
#endif  /* !_WIN32 */

  return bud_ok();
}


void bud_logger_free(bud_config_t* config) {
  if (config->logger == NULL)
    return;

#ifndef _WIN32
  if (config->logger->syslog_enabled)
    closelog();
#endif  /* !_WIN32 */
  free(config->logger);
  config->logger = NULL;
}


void bud_logva(bud_config_t* config,
               bud_log_level_t level,
               const char* fmt,
               va_list ap) {
  va_list stdio_ap;
  va_list syslog_ap;
  bud_logger_t* logger;
  int r;
  static char buf[1024];
#ifndef _WIN32
  int priority;
#endif  /* !_WIN32 */

  logger = config->logger;
  ASSERT(logger != NULL, "Logger not initalized");

  /* Ignore low-level logging */
  if (logger->level > level)
    return;

  if (logger->stdio_enabled) {
    va_copy(stdio_ap, ap);
    r = vsnprintf(buf, sizeof(buf), fmt, stdio_ap);
    ASSERT(r < (int) sizeof(buf), "Log line overflow");

    fprintf(stderr,
            "(%s) [%d] %s\n",
            bud_log_level_str(level),
#ifndef _WIN32
            getpid(),
#else
            0,
#endif  /* !_WIN32 */
            buf);
    va_end(stdio_ap);
  }

#ifndef _WIN32
  if (logger->syslog_enabled) {
    va_copy(syslog_ap, ap);
    switch (level) {
      case kBudLogDebug:
        priority = LOG_DEBUG;
        break;
      case kBudLogNotice:
        priority = LOG_NOTICE;
        break;
      case kBudLogInfo:
        priority = LOG_INFO;
        break;
      case kBudLogFatal:
        priority = LOG_ERR;
        break;
      case kBudLogWarning:
      default:
        priority = LOG_WARNING;
        break;
    }
    vsyslog(priority, fmt, syslog_ap);
    va_end(syslog_ap);
  }
#endif  /* !_WIN32 */
}

void bud_log(bud_config_t* config,
             bud_log_level_t level,
             const char* fmt,
             ...) {
  va_list ap;

  va_start(ap, fmt);
  bud_logva(config, level, fmt, ap);
  va_end(ap);
}


const char* bud_log_level_str(bud_log_level_t level) {
  switch (level) {
    case kBudLogDebug:
      return "dbg";
    case kBudLogNotice:
      return "ntc";
    case kBudLogInfo:
      return "inf";
    case kBudLogWarning:
      return "wrn";
    case kBudLogFatal:
      return "ftl";
    default:
      return "unk";
  }
}
