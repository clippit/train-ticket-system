PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE "user"  (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "username" TEXT UNIQUE NOT NULL,
    "password" TEXT NOT NULL,
    "register_time" TEXT NOT NULL
);
INSERT INTO user VALUES(1,'user1','$1$k31UDa7H$RPzkHrlJeqUMNW8/55BKa/',datetime('now', 'localtime'));
INSERT INTO user VALUES(2,'user2','$1$9G1UD0AH$rvIzIVXLOHqdpvZ76fpVW0',datetime('now', 'localtime'));
CREATE TABLE "timetable" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "name" TEXT NOT NULL,
    "start" TEXT NOT NULL,
    "end" TEXT NOT NULL,
    "start_time" TEXT NOT NULL,
    "end_time" TEXT NOT NULL,
    "price" INTEGER NOT NULL DEFAULT 0,
    "available" INTEGER NOT NULL
);
INSERT INTO timetable VALUES(1,'G1','Beijing South','Shanghai Hongqiao','09:00',"13:48",555,100);
INSERT INTO timetable VALUES(2,'G7001','Nanjing','Shanghai','07:00','08:39',140,90);
INSERT INTO timetable VALUES(3,'G7131','Nanjing','Shanghai Hongqiao','10:54',"12:50",140,3);
INSERT INTO timetable VALUES(4,'G42','Hangzhou','Beijing South','13:25',"19:49",631,20);
INSERT INTO timetable VALUES(5,'G18','Shanghai Hongqiao','Beijing South','15:00',"19:55",555,78);
INSERT INTO timetable VALUES(6,'G7074','Shanghai','Nanjing','17:42',"19:40",140,32);
CREATE TABLE "order" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "user_id" INTEGER NOT NULL,
    "train_id" INTEGER NOT NULL,
    "amount" INTEGER NOT NULL DEFAULT 1,
    "order_time" TEXT NOT NULL,
    CONSTRAINT "user" FOREIGN KEY ("user_id") REFERENCES "user" ("id"),
    CONSTRAINT "trian" FOREIGN KEY ("train_id") REFERENCES "timetable" ("id")
);
DELETE FROM sqlite_sequence;
INSERT INTO "sqlite_sequence" VALUES('user',2);
INSERT INTO "sqlite_sequence" VALUES('timetable',6);
COMMIT;
