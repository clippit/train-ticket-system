#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <crypt.h>
#include "cserver.h"
#include "cserver_fifo.h"
#include "cserver_socket.h"

void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    c.init         = ss_init;
    c.listener     = ss_listener;
    c.exit_hook    = ss_cleanup;
    c.sigterm_hook = ss_sigterm;
  } else if (mode == MODE_OFFLINE) {
    c.init         = fs_init;
    c.listener     = fs_listener;
    c.exit_hook    = fs_cleanup;
    c.sigterm_hook = fs_sigterm;
  } else {
    errx(EXIT_FAILURE, "Internal Unknown Error.");
  }
  signal(SIGTERM, c.sigterm_hook);
  signal(SIGINT,  c.sigterm_hook);
  signal(SIGHUP,  c.sigterm_hook);
  signal(SIGCHLD, SIG_IGN);
  atexit(c.exit_hook);
}

void server_run() {
  c.init();
  
  /* Running as a daemon, close stdin stdout stderr but do not change working directory */
  puts( "Running as daemon..." );
  if ( daemon(1, 0) != 0 )
    err(EXIT_FAILURE, "Cannot run as a daemon.");

  c.listener();
}

inline const char* log_time() {
  time_t t = time(NULL);
  struct tm *local = localtime(&t);
  static char str[26];
  strftime(str, sizeof(str), "%c", local);
  return str;
}

void action_dispatch(const request_t* request, response_t* response) {
  int action = request->action;
  int user_id;
  sqlite3 *db;

  if (action == ACTION_NOACTION) {
    noaction(response);
    return;
  }

  if (sqlite3_open(DATABASE, &db) != SQLITE_OK) {
    syslog(LOG_ERR, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }
  sqlite3_busy_timeout(db, 1000);  // set a busy timeout to prevent database locking

  if (action & ACTION_REGISTER)
    user_id = register_(db, response, request->username, request->password);
  else
    user_id = login(db, response, request->username, request->password);
  if (user_id < 0) {
    syslog(LOG_ERR, "Register/Login Error.");
    sqlite3_close(db);
    return;
  }

  if (action & ACTION_SEARCH) {
    search(db, response, request->name, request->from, request->to);
  }

  /* Don't handle ACTION_ORDER and ACTION_SEARCH the same time due to shared --number parameter */
  if ((action & ACTION_ORDER) && !(action & ACTION_SEARCH)) {
    take_order(db, response, user_id, request->name, request->amount);
  }

  if (action & ACTION_VIEW) {
    view(db, response, user_id);
  }

  if (action & ACTION_REFUND) {
    refund(db, response, user_id, request->order_id);
  }
  
  if (sqlite3_close(db) != SQLITE_OK) {
    syslog(LOG_ERR, "Can't close databse: %s\n", sqlite3_errmsg(db));
    return;
  }
}


/* Action Functions
   ----------------------------- */

void noaction(response_t *resp) {
  syslog(LOG_INFO, "%s - Client noaction", log_time());
  resp->code = r_failure;
  strcpy(resp->content, "No action specified.");
}

int register_(sqlite3 *db, response_t* resp, const char* username, const char* password) {
  syslog(LOG_INFO, "%s - Client register", log_time());
  int user_id = -1;
  char welcome[USERNAME_MAX_LENGTH + 20];

  sqlite3_stmt *check_stmt = NULL;
  sqlite3_prepare_v2(db, "SELECT * FROM user WHERE username=? LIMIT 1", -1, &check_stmt, NULL);
  sqlite3_bind_text(check_stmt, 1, username, -1, SQLITE_STATIC);
  int step_ret = sqlite3_step(check_stmt);
  sqlite3_finalize(check_stmt);

  switch(step_ret) {
  case SQLITE_ROW:  // same username exists
    syslog(LOG_WARNING, "%s - Client register - Username %s already exists.", log_time(), username);
    resp->code = r_failure;
    strcpy(resp->content, "Username already exists.");
    break;
  case SQLITE_DONE:
    if ((user_id = _do_register(db, username, password)) > 0) {
      syslog(LOG_INFO, "%s - Client register - Username: %s, User ID: %d", log_time(), username, user_id);
      sprintf(welcome, "Welcome new user, %s \n\n", username);
      strcat(resp->content, welcome);
    } else {
      syslog(LOG_WARNING, "%s - Client register - Failed. Username: %s ", log_time(), username);
    }
    break;
  default:
    syslog(LOG_WARNING, "%s - Client register - Error occurs: %s", log_time(), sqlite3_errmsg(db));
    resp->code = r_failure;
    strcpy(resp->content, "An error occurred when registering.");
    break;
  }
  return user_id;
}

int login(sqlite3* db, response_t* resp, const char* username, const char* password) {
  syslog(LOG_INFO, "%s - Client login", log_time());
  int user_id = -1;
  int success = 0;

  sqlite3_stmt *stmt = NULL;
  sqlite3_prepare_v2(db, "SELECT * FROM user WHERE username=? LIMIT 1", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
  int step_ret = sqlite3_step(stmt);
  if (step_ret == SQLITE_ROW) {
    const char* encrypted_password = (const char*)sqlite3_column_text(stmt, 2);
    const char* checked_password = crypt(password, encrypted_password);
    if (strcmp(checked_password, encrypted_password) == 0) {
      success = 1;
      user_id = sqlite3_column_int(stmt, 0);
      const char *register_time = (const char*)sqlite3_column_text(stmt, 3);
      char welcome[USERNAME_MAX_LENGTH + 60];
      sprintf(welcome, "Welcome, %s (Registered at %s)\n\n", username, register_time);
      strcat(resp->content, welcome);
    }
  }

  if(success) {
    syslog(LOG_INFO, "%s - Client login - Username %s login successfully", log_time(), username);
  } else {
    resp->code = r_failure;
    strcpy(resp->content, "Sorry, you cannot login now. Maybe your username or password is incorrect. Please try again later.");
    syslog(LOG_WARNING, "%s - Client login - Username %s login failed. Status: %d. Error: %s", log_time(), username, step_ret, sqlite3_errmsg(db));
  }
    
  sqlite3_finalize(stmt);
  return user_id;
}

void search(sqlite3* db, response_t* resp, const char* name, const char* from, const char* to) {
  syslog(LOG_INFO, "%s - Client Search - Name: %s, From: %s, To: %s", log_time(), name, from, to);

  sqlite3_stmt *stmt = NULL;
  sqlite3_prepare_v2(db,
    "SELECT * FROM timetable "
    "WHERE name LIKE '%' || ? || '%' "
    "AND start LIKE '%' || ? || '%' "
    "AND end LIKE '%' || ? || '%'",
    -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, from, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3,   to, -1, SQLITE_STATIC);
  int count = 0;
  char result_header[200];
  sprintf(result_header,
    "%*s  %*s  %*s  %*s  %*s  %s  %s\n"
    "=============================================================================\n",
    TRAIN_NUMBER_MAX_LENGTH, "No.",
    TRAIN_STATION_MAX_LENGTH, "Originating Station",
    TRAIN_STATION_MAX_LENGTH, "Terminal Station",
    TRAIN_TIME_LENGTH, "Start",
    TRAIN_TIME_LENGTH, "End",
    "Price", "Avlb.");
  strcat(resp->content, result_header);
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char* name       = sqlite3_column_text(stmt, 1);
    const unsigned char* start      = sqlite3_column_text(stmt, 2);
    const unsigned char* end        = sqlite3_column_text(stmt, 3);
    const unsigned char* start_time = sqlite3_column_text(stmt, 4);
    const unsigned char* end_time   = sqlite3_column_text(stmt, 5);
    const int price                 = sqlite3_column_int(stmt, 6);
    const int available             = sqlite3_column_int(stmt, 7);
    char result[80];
    sprintf(result, "%*s  %*s  %*s  %*s  %*s  %5d  %5d\n",
      TRAIN_NUMBER_MAX_LENGTH, name,
      TRAIN_STATION_MAX_LENGTH, start,
      TRAIN_STATION_MAX_LENGTH, end,
      TRAIN_TIME_LENGTH, start_time,
      TRAIN_TIME_LENGTH, end_time,
      price, available);
    strcat(resp->content, result);
    count++;
  }

  if (count == 0) {
    strcat(resp->content, "[Sorry, no results match your criteria]\n\n");
  } else {
    char summary[30];
    sprintf(summary, "[%d record(s) found.]\n\n", count);
    strcat(resp->content, summary);
  }

  sqlite3_finalize(stmt);
}

void take_order(sqlite3* db, response_t* resp, const int user_id, const char* name, const unsigned short int amount) {
  syslog(LOG_INFO, "%s - Client Order", log_time());

  sqlite3_stmt *check_stmt = NULL;
  sqlite3_prepare_v2(db, "SELECT * FROM timetable WHERE name=? LIMIT 1", -1, &check_stmt, NULL);
  sqlite3_bind_text(check_stmt, 1, name, -1, SQLITE_STATIC);
  sqlite3_stmt *order_stmt = NULL;
  sqlite3_prepare_v2(db, "INSERT INTO [order] VALUES(NULL, ?, ?, ?, datetime('now', 'localtime'))", -1, &order_stmt, NULL);
  sqlite3_bind_int(order_stmt, 1, user_id);
  sqlite3_bind_int(order_stmt, 3, amount);
  sqlite3_stmt *update_stmt = NULL;
  sqlite3_prepare_v2(db, "UPDATE timetable SET available=? WHERE id=?", -1, &update_stmt, NULL);

  sqlite3_exec(db, "BEGIN;", 0, 0, 0);
  int check_ret = sqlite3_step(check_stmt);
  if (check_ret == SQLITE_BUSY) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, check_ret, "Order failed, please retry. SQLITE_BUSY", 1);
    return;
  } else if (check_ret == SQLITE_DONE) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, check_ret, "There is not such Train Number.", 1);
    return;
  } else if (check_ret != SQLITE_ROW) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, check_ret, "Database Error.", 1);
    return;
  }

  /* Find the train. */
  const int train_id     = sqlite3_column_int(check_stmt, 0);
  const char* start      = (const char*)sqlite3_column_text(check_stmt, 2);
  const char* end        = (const char*)sqlite3_column_text(check_stmt, 3);
  const char* start_time = (const char*)sqlite3_column_text(check_stmt, 4);
  const char* end_time   = (const char*)sqlite3_column_text(check_stmt, 5);
  const int price        = sqlite3_column_int(check_stmt, 6);
  const int available    = sqlite3_column_int(check_stmt, 7);

  if (available < amount) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, check_ret, "There is not enough available tickets.", 1);
    return;
  }

  sqlite3_bind_int(order_stmt, 2, train_id);

  int order_ret = sqlite3_step(order_stmt);
  if (order_ret != SQLITE_DONE) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, order_ret, "Order failed, please retry.", 1);
    return;
  }
  int order_id = sqlite3_last_insert_rowid(db);

  sqlite3_bind_int(update_stmt, 1, available - amount);
  sqlite3_bind_int(update_stmt, 2, train_id);
  int update_ret = sqlite3_step(update_stmt);
  if (update_ret != SQLITE_DONE) {
    _order_refund_failed(db, resp, check_stmt, order_stmt, update_stmt, update_ret, "Update remaining tickets failed.", 1);
    return;
  }

  sqlite3_exec(db, "COMMIT;", 0, 0, 0);

  syslog(LOG_INFO, "%s - Client Order - Order ID: %d, User ID: %d, Train: %s, Amount: %d", log_time(), order_id, user_id, name, amount);
  _generate_order(resp, order_id, name, start, end, start_time, end_time, price, amount, "");

  sqlite3_finalize(check_stmt);
  sqlite3_finalize(order_stmt);
  sqlite3_finalize(update_stmt);
}

void view(sqlite3* db, response_t* resp, const int user_id) {
  syslog(LOG_INFO, "%s - Client View - User ID: %d", log_time(), user_id);
  strcat(resp->content, "Your Orders:\n");

  sqlite3_stmt *stmt = NULL;
  sqlite3_prepare_v2(db,
    "SELECT o.id, o.amount, o.order_time, t.name, t.start, t.end, t.start_time, t.end_time, t.price FROM [order] AS o "
    "INNER JOIN timetable AS t ON o.train_id = t.id "
    "WHERE user_id=? "
    "ORDER BY order_time DESC", -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, user_id);
  int count = 0;
  while(sqlite3_step(stmt) == SQLITE_ROW) {
    count++;
    const int order_id     = sqlite3_column_int(stmt, 0);
    const char *order_time = (const char*)sqlite3_column_text(stmt, 2);
    if (count <= MAX_DETAIL_ORDER) {
      /* Generate detailed order info */
      const int  amount      = sqlite3_column_int(stmt, 1);
      const char *name       = (const char*)sqlite3_column_text(stmt, 3);
      const char *start      = (const char*)sqlite3_column_text(stmt, 4);
      const char *end        = (const char*)sqlite3_column_text(stmt, 5);
      const char *start_time = (const char*)sqlite3_column_text(stmt, 6);
      const char *end_time   = (const char*)sqlite3_column_text(stmt, 7);
      const int  price       = sqlite3_column_int(stmt, 8);
      _generate_order(resp, order_id, name, start, end, start_time, end_time, price, amount, order_time);
    } else {
      /* Show Order summary only */
      char short_info[50];
      sprintf(short_info, "Order ID: %d, Order Time: %s\n", order_id, order_time);
      strcat(resp->content, short_info);
    }
  }
  char summary[70];
  sprintf(summary, "[%d order(s) found. Only display details of latest %d orders.]\n\n", count, MAX_DETAIL_ORDER);
  strcat(resp->content, summary);

  sqlite3_finalize(stmt);
}

void refund(sqlite3* db, response_t* resp, const int user_id, const int order_id) {
  syslog(LOG_INFO, "%s - Client Refund", log_time());

  sqlite3_stmt *check_stmt = NULL;
  sqlite3_prepare_v2(db, "SELECT train_id, amount FROM [order] WHERE id=? AND user_id=? LIMIT 1", -1, &check_stmt, NULL);
  sqlite3_bind_int(check_stmt, 1, order_id);
  sqlite3_bind_int(check_stmt, 2, user_id);
  int step_ret = sqlite3_step(check_stmt);
  if (step_ret != SQLITE_ROW) {
    const char* errmsg = "Search Error. Was that order taken by you?";
    resp->code = r_failure;
    strcat(resp->content, errmsg);
    syslog(LOG_WARNING, "%s - Client Refund - %s, Status: %d, Error: %s", log_time(), errmsg, step_ret, sqlite3_errmsg(db));
    sqlite3_finalize(check_stmt);
    return;
  }
  const int train_id = sqlite3_column_int(check_stmt, 0);
  const int amount   = sqlite3_column_int(check_stmt, 1);

  sqlite3_stmt *delete_stmt = NULL;
  sqlite3_prepare_v2(db, "DELETE FROM [order] WHERE id=?", -1, &delete_stmt, NULL);
  sqlite3_bind_int(delete_stmt, 1, order_id);
  sqlite3_stmt *update_stmt = NULL;
  sqlite3_prepare_v2(db, "UPDATE timetable SET available=available+? WHERE id=?", -1, &update_stmt, NULL);
  sqlite3_bind_int(update_stmt, 1, amount);
  sqlite3_bind_int(update_stmt, 2, train_id);

  sqlite3_exec(db, "BEGIN;", 0, 0, 0);
  int delete_ret = sqlite3_step(delete_stmt);
  if (delete_ret != SQLITE_DONE) {
    _order_refund_failed(db, resp, check_stmt, delete_stmt, update_stmt, delete_ret, "Refund failed", 0);
    return;
  }
  int update_ret = sqlite3_step(update_stmt);
  if (update_ret != SQLITE_DONE) {
    _order_refund_failed(db, resp, check_stmt, delete_stmt, update_stmt, update_ret, "Update remaining tickets failed", 0);
    return;
  }
  sqlite3_exec(db, "COMMIT;", 0, 0, 0);

  syslog(LOG_INFO, "%s - Client Refund - Order ID: %d Refunded", log_time(), order_id);
  strcat(resp->content, "Your order has been refunded.\n");

  sqlite3_finalize(check_stmt);
  sqlite3_finalize(delete_stmt);
  sqlite3_finalize(update_stmt);
}


/* 'Private' Functions
   ----------------------------- */

const char* _encrypt_password(const char*password) {
  unsigned long seed[2];
  char salt[] = "$1$........";
  const char *const seedchars =
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";
  int i;
  /* Generate a random seed. */
  seed[0] = time(NULL);
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);
  /* Turn it into printable characters from `seedchars`. */
  for (i = 0; i < 8; i++)
    salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];
  return crypt(password, salt);
}

int _do_register(sqlite3* db, const char* username, const char* password) {
  int user_id = -1;
  const char* encrypted_password = _encrypt_password(password);
  sqlite3_stmt *stmt = NULL;
  sqlite3_prepare_v2(db, "INSERT INTO user VALUES(NULL, ?, ?, datetime('now'))", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, encrypted_password, -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    user_id = sqlite3_last_insert_rowid(db);
  }
  sqlite3_finalize(stmt);
  return user_id;
}

void _order_refund_failed(sqlite3* db, response_t* resp, sqlite3_stmt* stmt1, sqlite3_stmt* stmt2, sqlite3_stmt* stmt3, const int status, const char* errmsg, const int is_order) {
  sqlite3_exec(db, "ROLLBACK;", 0, 0, 0);
  resp->code = r_failure;
  strcat(resp->content, errmsg);
  syslog(LOG_WARNING, "%s - Client %s - %s, Status: %d, Error: %s", log_time(), is_order ? "Order" : "Refund", errmsg, status, sqlite3_errmsg(db));
  sqlite3_finalize(stmt1);
  sqlite3_finalize(stmt2);
  sqlite3_finalize(stmt3);
}

void _generate_order(response_t* resp, const int order_id, const char* name, const char* start, const char* end, const char* start_time, const char* end_time, const int price, const int amount, const char* order_time) {
  char order_diagram[550];
  sprintf(order_diagram,
    "+---------------------------------+\n"   // +---------------------------------+
    "| Order ID: %-22d|\n"                    // | Order ID:                       |
    "| Order Time: %-19s |\n"                 // | Order Time: 0000-00-00 00:00:00 |
    "|---------------------------------|\n"   // |---------------------------------|
    "| Train No. %-22s|\n"                    // | Train No.: G1111                |
    "| From:                           |\n"   // | From:                           |
    "|   %-20s (%5s)  |\n"                    // |   12345678901234567890 (12:34)  |
    "| To:                             |\n"   // | To:                             |
    "|   %-20s (%5s)  |\n"                    // |   12345678901234567890 (12:34)  |
    "|                                 |\n"   // |                                 |
    "| Unit-price: %-20d|\n"                  // | Unit-price:                     |
    "| Amount: %-24d|\n"                      // | Amount:                         |
    "|                                 |\n"   // |                                 |
    "| Total: %-25d|\n"                       // | Total:                          |
    "+---------------------------------+\n",  // +---------------------------------+
    order_id, strlen(order_time) == 0 ? "Just now" : order_time, name, start, start_time, end, end_time, price, amount, price * amount);
  strcat(resp->content, order_diagram);
}
