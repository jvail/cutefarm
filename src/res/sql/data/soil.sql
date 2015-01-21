/*
    Soil data
    
    CREATE TABLE soil_typ (
      soil_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
      soil_typ_name TEXT,
      soil_typ_abbr TEXT NOT NULL,
      soil_sys_id INTEGER NOT NULL )#

    CREATE TABLE soil_typ_def (
      soil_typ_def_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
      soil_typ_id INTEGER NOT NULL,
      soil_typ_def_x REAL NOT NULL,
      soil_typ_def_y REAL NOT NULL )#

    CREATE TABLE soil_sys (
      soil_sys_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
      soil_sys_name TEXT UNIQUE NOT NULL )#
*/

INSERT INTO soil_sys VALUES(1, 'Bodenkundliche Kartieranleitung KA4/KA5');
INSERT INTO soil_sys VALUES(2, 'Österreichische Bodenkundliche Gesellschaft');
INSERT INTO soil_sys VALUES(3, 'Bodenkundliche Gesellschaft der Schweiz');
INSERT INTO soil_sys VALUES(4, 'FAO / World Reference Base');

INSERT INTO soil_typ VALUES(1, NULL, 'Ss', 1);
INSERT INTO soil_typ_def VALUES(NULL, 1, 0, 0);
INSERT INTO soil_typ_def VALUES(NULL, 1, 5, 0);
INSERT INTO soil_typ_def VALUES(NULL, 1, 5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 1, 0, 10);

INSERT INTO soil_typ VALUES(2, NULL, 'St2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 2, 5, 0);
INSERT INTO soil_typ_def VALUES(NULL, 2, 17.5, 0);
INSERT INTO soil_typ_def VALUES(NULL, 2, 17.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 2, 5, 10);

INSERT INTO soil_typ VALUES(3, NULL, 'St3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 3, 17.5, 0);
INSERT INTO soil_typ_def VALUES(NULL, 3, 25, 0);
INSERT INTO soil_typ_def VALUES(NULL, 3, 25, 15);
INSERT INTO soil_typ_def VALUES(NULL, 3, 17.5, 15);

INSERT INTO soil_typ VALUES(4, NULL, 'Ts4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 4, 25, 0);
INSERT INTO soil_typ_def VALUES(NULL, 4, 35, 0);
INSERT INTO soil_typ_def VALUES(NULL, 4, 35, 15);
INSERT INTO soil_typ_def VALUES(NULL, 4, 25, 15);

INSERT INTO soil_typ VALUES(5, NULL, 'Ts3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 5, 35, 0);
INSERT INTO soil_typ_def VALUES(NULL, 5, 45, 0);
INSERT INTO soil_typ_def VALUES(NULL, 5, 45, 15);
INSERT INTO soil_typ_def VALUES(NULL, 5, 35, 15);

INSERT INTO soil_typ VALUES(6, NULL, 'Ts2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 6, 45, 0);
INSERT INTO soil_typ_def VALUES(NULL, 6, 65, 0);
INSERT INTO soil_typ_def VALUES(NULL, 6, 65, 15);
INSERT INTO soil_typ_def VALUES(NULL, 6, 45, 15);

INSERT INTO soil_typ VALUES(7, NULL, 'Tt', 1);
INSERT INTO soil_typ_def VALUES(NULL, 7, 65, 0);
INSERT INTO soil_typ_def VALUES(NULL, 7, 100, 0);
INSERT INTO soil_typ_def VALUES(NULL, 7, 65, 35);

INSERT INTO soil_typ VALUES(8, NULL, 'Su2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 8, 0, 10);
INSERT INTO soil_typ_def VALUES(NULL, 8, 5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 8, 5, 25);
INSERT INTO soil_typ_def VALUES(NULL, 8, 0, 25);

INSERT INTO soil_typ VALUES(9, NULL, 'Sl2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 9, 5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 9, 7.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 9, 7.5, 25);
INSERT INTO soil_typ_def VALUES(NULL, 9, 5, 25);

INSERT INTO soil_typ VALUES(10, NULL, 'Sl3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 10, 7.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 10, 12.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 10, 12.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 10, 7.5, 40);

INSERT INTO soil_typ VALUES(11, NULL, 'Sl4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 11, 12.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 11, 17.5, 10);
INSERT INTO soil_typ_def VALUES(NULL, 11, 17.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 11, 12.5, 40);

INSERT INTO soil_typ VALUES(12, NULL, 'Ls4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 12, 17.5, 15);
INSERT INTO soil_typ_def VALUES(NULL, 12, 25, 15);
INSERT INTO soil_typ_def VALUES(NULL, 12, 25, 30);
INSERT INTO soil_typ_def VALUES(NULL, 12, 17.5, 30);

INSERT INTO soil_typ VALUES(13, NULL, 'Lts', 1);
INSERT INTO soil_typ_def VALUES(NULL, 13, 25, 15);
INSERT INTO soil_typ_def VALUES(NULL, 13, 45, 15);
INSERT INTO soil_typ_def VALUES(NULL, 13, 45, 30);
INSERT INTO soil_typ_def VALUES(NULL, 13, 25, 30);

INSERT INTO soil_typ VALUES(14, NULL, 'Tl', 1);
INSERT INTO soil_typ_def VALUES(NULL, 14, 45, 15);
INSERT INTO soil_typ_def VALUES(NULL, 14, 65, 15);
INSERT INTO soil_typ_def VALUES(NULL, 14, 65, 30);
INSERT INTO soil_typ_def VALUES(NULL, 14, 45, 30);

INSERT INTO soil_typ VALUES(15, NULL, 'Su3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 15, 0, 25);
INSERT INTO soil_typ_def VALUES(NULL, 15, 7.5, 25);
INSERT INTO soil_typ_def VALUES(NULL, 15, 7.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 15, 0, 40);

INSERT INTO soil_typ VALUES(16, NULL, 'Ls3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 16, 17.5, 30);
INSERT INTO soil_typ_def VALUES(NULL, 16, 25, 30);
INSERT INTO soil_typ_def VALUES(NULL, 16, 25, 40);
INSERT INTO soil_typ_def VALUES(NULL, 16, 17.5, 40);

INSERT INTO soil_typ VALUES(17, NULL, 'Lt2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 17, 25, 30);
INSERT INTO soil_typ_def VALUES(NULL, 17, 35, 30);
INSERT INTO soil_typ_def VALUES(NULL, 17, 35, 50);
INSERT INTO soil_typ_def VALUES(NULL, 17, 25, 50);

INSERT INTO soil_typ VALUES(18, NULL, 'Lt3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 18, 35, 30);
INSERT INTO soil_typ_def VALUES(NULL, 18, 45, 30);
INSERT INTO soil_typ_def VALUES(NULL, 18, 45, 50);
INSERT INTO soil_typ_def VALUES(NULL, 18, 35, 50);

INSERT INTO soil_typ VALUES(19, NULL, 'Tu2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 19, 45, 30);
INSERT INTO soil_typ_def VALUES(NULL, 19, 65, 30);
INSERT INTO soil_typ_def VALUES(NULL, 19, 65, 35);
INSERT INTO soil_typ_def VALUES(NULL, 19, 45, 55);

INSERT INTO soil_typ VALUES(20, NULL, 'Su4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 20, 0, 40);
INSERT INTO soil_typ_def VALUES(NULL, 20, 7.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 20, 7.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 20, 0, 50);

INSERT INTO soil_typ VALUES(21, NULL, 'Slu', 1);
INSERT INTO soil_typ_def VALUES(NULL, 21, 7.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 21, 17.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 21, 17.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 21, 7.5, 50);

INSERT INTO soil_typ VALUES(22, NULL, 'Ls2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 22, 17.5, 40);
INSERT INTO soil_typ_def VALUES(NULL, 22, 25, 40);
INSERT INTO soil_typ_def VALUES(NULL, 22, 25, 50);
INSERT INTO soil_typ_def VALUES(NULL, 22, 17.5, 50);

INSERT INTO soil_typ VALUES(23, NULL, 'Us', 1);
INSERT INTO soil_typ_def VALUES(NULL, 23, 0, 50);
INSERT INTO soil_typ_def VALUES(NULL, 23, 7.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 23, 7.5, 80);
INSERT INTO soil_typ_def VALUES(NULL, 23, 0, 80);

INSERT INTO soil_typ VALUES(24, NULL, 'Uls', 1);
INSERT INTO soil_typ_def VALUES(NULL, 24, 7.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 24, 17.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 24, 17.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 24, 7.5, 65);

INSERT INTO soil_typ VALUES(25, NULL, 'Lu', 1);
INSERT INTO soil_typ_def VALUES(NULL, 25, 17.5, 50);
INSERT INTO soil_typ_def VALUES(NULL, 25, 30, 50);
INSERT INTO soil_typ_def VALUES(NULL, 25, 30, 65);
INSERT INTO soil_typ_def VALUES(NULL, 25, 17.5, 65);

INSERT INTO soil_typ VALUES(26, NULL, 'Tu3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 26, 30, 50);
INSERT INTO soil_typ_def VALUES(NULL, 26, 45, 50);
INSERT INTO soil_typ_def VALUES(NULL, 26, 45, 55);
INSERT INTO soil_typ_def VALUES(NULL, 26, 35, 65);
INSERT INTO soil_typ_def VALUES(NULL, 26, 30, 65);

INSERT INTO soil_typ VALUES(27, NULL, 'Uu', 1);
INSERT INTO soil_typ_def VALUES(NULL, 27, 0, 80);
INSERT INTO soil_typ_def VALUES(NULL, 27, 7.5, 80);
INSERT INTO soil_typ_def VALUES(NULL, 27, 7.5, 92.5);
INSERT INTO soil_typ_def VALUES(NULL, 27, 0, 100);

INSERT INTO soil_typ VALUES(28, NULL, 'Ut2', 1);
INSERT INTO soil_typ_def VALUES(NULL, 28, 7.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 28, 12.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 28, 12.5, 87.5);
INSERT INTO soil_typ_def VALUES(NULL, 28, 7.5, 92.5);

INSERT INTO soil_typ VALUES(29, NULL, 'Ut3', 1);
INSERT INTO soil_typ_def VALUES(NULL, 29, 12.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 29, 17.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 29, 17.5, 82.5);
INSERT INTO soil_typ_def VALUES(NULL, 29, 12.5, 87.5);

INSERT INTO soil_typ VALUES(30, NULL, 'Ut4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 30, 17.5, 65);
INSERT INTO soil_typ_def VALUES(NULL, 30, 25, 65);
INSERT INTO soil_typ_def VALUES(NULL, 30, 25, 75);
INSERT INTO soil_typ_def VALUES(NULL, 30, 17.5, 82.5);

INSERT INTO soil_typ VALUES(31, NULL, 'Tu4', 1);
INSERT INTO soil_typ_def VALUES(NULL, 31, 25, 65);
INSERT INTO soil_typ_def VALUES(NULL, 31, 35, 65);
INSERT INTO soil_typ_def VALUES(NULL, 31, 25, 75);









