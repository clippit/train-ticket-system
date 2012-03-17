PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE "user"  (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "username" TEXT UNIQUE NOT NULL,
    "password" TEXT NOT NULL,
    "register_time" TEXT NOT NULL
);
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
CREATE TABLE "order" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "user_id" INTEGER NOT NULL,
    "train_id" INTEGER NOT NULL,
    "amount" INTEGER NOT NULL DEFAULT 1,
    "order_time" TEXT NOT NULL,
    CONSTRAINT "user" FOREIGN KEY ("user_id") REFERENCES "user" ("id"),
    CONSTRAINT "trian" FOREIGN KEY ("train_id") REFERENCES "timetable" ("id")
);
COMMIT;
