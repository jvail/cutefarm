BEGIN;
PRAGMA foreign_keys = 1;
INSERT INTO "crop" VALUES(9,NULL,'Summer Barley','Hordeum vulgare L. sensu lato');
INSERT INTO "crop" VALUES(10,NULL,'Winter Barley','Hordeum vulgare L. sensu lato');
INSERT INTO "crop" VALUES(11,NULL,'Summer Oats','Avena sativa L.');
INSERT INTO "crop" VALUES(12,NULL,'Winter Oats','Avena sativa L.');
INSERT INTO "crop" VALUES(16,NULL,'Summer Durum','Triticum durum Desf.');
INSERT INTO "crop" VALUES(17,NULL,'Winter Durum','Triticum durum Desf.');
INSERT INTO "crop" VALUES(19,NULL,'Batata','Solanum tuberosum L.');
INSERT INTO "crop" VALUES(27,NULL,'Milho','Zea mays L.');
INSERT INTO "crop" VALUES(35,NULL,'Summer Rye','Secale L.');
INSERT INTO "crop" VALUES(36,NULL,'Winter Rye','Secale L.');
INSERT INTO "crop" VALUES(43,NULL,'Soja','Glycine max (L.) Merr.');
INSERT INTO "crop" VALUES(62,NULL,'Sugarbeet','Beta vulgaris L. var. altissima Döll');
INSERT INTO "crop" VALUES(31,NULL,'Summer Canola','Brassica napus L. (partim)');
INSERT INTO "crop" VALUES(32,NULL,'Winter Canola','Brassica napus L. (partim)');
INSERT INTO "crop_use" VALUES(8,31,'Summer Canola', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(9,32,'Winter Canola', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(10,9,'Summer Barley', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(11,10,'Winter Barley', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(12,11,'Summer Oats', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(13,12,'Winter Oats', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(17,16,'Summer Durum', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(18,17,'Winter Durum', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(21,19,'Batata (nova)', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(23,19,'Potatoe (late)', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(34,27,'Corn (silage)', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(35,27,'Corn', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(47,43,'Soja', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(48,35,'Summer Rye', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(49,36,'Winter Rye', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(80,62,'Sugarbeet', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
INSERT INTO "crop_use" VALUES(82, NULL,'Pastagem', 'dt', 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 'true');
COMMIT;




