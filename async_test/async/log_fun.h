#ifndef _LOG_FUN_H_
#define _LOG_FUN_H_

#include "types.h"

/**
 * @typedef log_lvl_t
 * @brief   typedef of log_lvl
 */

/**
 * @enum  log_lvl
 * @brief 日志等级
 */
typedef enum log_lvl 
{
	/*! system is unusable -- 0 */
	log_lvl_emerg,
	/*! action must be taken immediately -- 1 */
	log_lvl_alert,
	/*! critical conditions -- 2 */
	log_lvl_crit,
	/*! error conditions -- 3 */
	log_lvl_error,
	/*! warning conditions  -- 4 */
	log_lvl_warning,
	/*! normal but significant condition -- 5 */
	log_lvl_notice,
	/*! informational -- 6 */
	log_lvl_info,
	/*! debug-level messages -- 7 */
	log_lvl_debug,
	/*! trace-level messages -- 8。如果定义了宏LOG_USE_SYSLOG，则log_lvl_trace==log_lvl_debug */
#ifndef LOG_USE_SYSLOG
	log_lvl_trace,
#else
	log_lvl_trace = log_lvl_debug,
#endif

	log_lvl_max
} log_lvl_t;

/**
 * @typedef log_dest_t
 * @brief   typedef of log_dest
 */

/**
 * @enum  log_dest
 * @brief 日志输出方式
 */
typedef enum log_dest 
{
	/*! 仅输出到屏幕  */
	log_dest_terminal	= 1,
	/*! 仅输出到文件 */
	log_dest_file		= 2,
	/*! 既输出到屏幕，也输出到文件 */
	log_dest_both		= 3
} log_dest_t;

/**
  * @brief 初始化日志记录功能。如果使用自动轮转，必须保证每天写的日志文件个数不能超过最大文件个数（maxfiles），
  *        否则日志会写乱掉。
  *
  * @param dir 日志保存目录。如果填0，则在屏幕中输出日志。
  * @param lvl 日志输出等级。如果设置为log_lvl_notice，则log_lvl_notice以上等级的日志都不输出。
  * @param size 每个日志文件的大小限制（byte），超过这个大小则自动创建一个新的日志文件。
  * @param maxfiles 每种等级的日志的最大文件个数，用于控制日志轮转。个数越少，日志轮转时效率越高。
  *                 如果填0，则表示不使用日志轮转功能。
  * @param pre_name 日志文件名前缀。
  *
  * @see set_log_dest, log_init_t, enable_multi_thread
  *
  * @return 成功返回0，失败返回-1。
  */
int  log_init(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name);

/**
  * @brief 初始化日志记录功能。按时间周期创建新的日志文件。
  *
  * @param dir 日志保存目录。如果填0，则在屏幕中输出日志。
  * @param lvl 日志输出等级。如果设置为log_lvl_notice，则log_lvl_notice以上等级的日志都不输出。
  * @param pre_name 日志文件名前缀。
  * @param logtime 每个日志文件保存logtime分钟的日志，最大不能超过30000000分钟。假设logtime为15，则每小时产生4个日志文件，每个文件保存15分钟日志。
  *
  * @see set_log_dest, log_init, enable_multi_thread
  *
  * @return 成功返回0，失败返回-1。
  */
int log_init_t(const char* dir, log_lvl_t lvl, const char* pre_name, int logtime);

#endif
