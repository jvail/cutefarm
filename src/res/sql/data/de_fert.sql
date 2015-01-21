BEGIN;
INSERT INTO "fert_typ" VALUES(1, 'NPK');
INSERT INTO "fert_typ" VALUES(2, 'PK');
INSERT INTO "fert" VALUES(NULL, 'NPK-16-16-16', 1, 'dt', 16, 16, 16, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'NPK-13-9-16', 1, 'dt', 13, 16, 9, 7, 4, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'NPK-6-20-30', 1, 'dt', 6, 30, 20, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'NPK-20-8-8', 1, 'dt', 20, 8, 8, 4, 3, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'PK-12-24', 2, 'dt', 0, 24, 12, 6, 0, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'PK-10-21', 2, 'dt', 0, 21, 10, 6, 4, 0.0, 0.0, 'true');
INSERT INTO "fert" VALUES(NULL, 'PK-10-30', 2, 'dt', 0, 30, 10, 4, 8, 0.0, 0.0, 'true');
COMMIT;

