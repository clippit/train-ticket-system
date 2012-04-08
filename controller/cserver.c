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

void register_running_mode(int mode) {
  if (mode == MODE_ONLINE) {
    // TODO Not implemented yet
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
  

  if (sqlite3_close(db) != SQLITE_OK) {
    syslog(LOG_ERR, "Can't close databse: %s\n", sqlite3_errmsg(db));
    return;
  }
}

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

int login(sqlite3* db, response_t* resp, const char* username, const char* password) {
  syslog(LOG_INFO, "%s - Client login", log_time());
  int user_id = -1;
  int success = 0;

  sqlite3_stmt *stmt = NULL;
  sqlite3_prepare_v2(db, "SELECT * FROM user WHERE username=? LIMIT 1", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
  if (sqlite3_step(stmt) == SQLITE_ROW) {
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
    strcpy(resp->content, "Sorry, your username or password is invalid.");
    syslog(LOG_INFO, "%s - Client login - Username %s login failed.", log_time(), username);
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
