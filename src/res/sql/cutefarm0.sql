/*
    CuteFarm sqlite database schema version 2  
    PRAGMA encoding = 'UTF-8' 
*/



/*
    Check if db schema version 
    works with cutefarm version.
*/

PRAGMA user_version = 190#

/*
    General farm data
*/

CREATE TABLE farm (
  farm_name TEXT UNIQUE NOT NULL,
  farm_str TEXT NOT NULL,
  farm_city_code TEXT NOT NULL,
  farm_numb TEXT NOT NULL)#

/*
    Soil data
*/

CREATE TABLE soil_typ (
  soil_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_typ_name TEXT,
  soil_typ_abbr TEXT NOT NULL,
  soil_sys_id INTEGER NOT NULL )#

CREATE TABLE soil_typ_def (
  soil_typ_def_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_typ_id INTEGER NOT NULL,
  soil_typ_def_x DOUBLE NOT NULL,
  soil_typ_def_y DOUBLE NOT NULL )#

CREATE TABLE soil_sys (
  soil_sys_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_sys_name TEXT UNIQUE NOT NULL )#

/*
    planttreatment (pltr)
    types of planttreatment (pltr_typ)
    active ingrediences (pltr_sub)
*/

CREATE TABLE pltr (
  pltr_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_name TEXT UNIQUE NOT NULL,
  pltr_unit TEXT NOT NULL,
  pltr_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE pltr_typ (
  pltr_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_typ_name TEXT UNIQUE NOT NULL )#

CREATE TABLE pltr_sub (
  pltr_sub_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_sub_name TEXT UNIQUE NOT NULL )#

/*
    pltr_x_pltr_typ (m:n)
*/

CREATE TABLE pltr_x_pltr_typ (
  pltr_x_pltr_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_id INTEGER NOT NULL CONSTRAINT fk_pltr_id REFERENCES pltr (pltr_id) ON DELETE CASCADE,
  pltr_typ_id INTEGER NOT NULL CONSTRAINT fk_pltr_typ_id REFERENCES pltr_typ (pltr_typ_id) ON DELETE RESTRICT )#

CREATE UNIQUE INDEX index_pltr_x_pltr_typ
  ON pltr_x_pltr_typ ( pltr_id, pltr_typ_id  )#

/*
    Foreign Key Preventing insert on pltr_x_pltr_typ
*/

CREATE TRIGGER fki_pltr_x_pltr_typ_pltr_id_pltr_pltr_id
  BEFORE INSERT ON [pltr_x_pltr_typ]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on table "pltr_x_pltr_typ" violates foreign key constraint "fki_pltr_x_pltr_typ_pltr_id_pltr_pltr_id"')
    WHERE (SELECT pltr_id FROM pltr WHERE pltr_id = NEW.pltr_id) IS NULL;
  END#

CREATE TRIGGER fki_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id
  BEFORE INSERT ON [pltr_x_pltr_typ]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'insert on table "pltr_x_pltr_typ" violates foreign key constraint "fki_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id"')
    WHERE (SELECT pltr_typ_id FROM pltr_typ WHERE pltr_typ_id = NEW.pltr_typ_id) IS NULL;
  END#

/*
    Foreign key preventing update on pltr_x_pltr_typ
*/

CREATE TRIGGER fku_pltr_x_pltr_typ_pltr_id_pltr_pltr_id
  BEFORE UPDATE ON [pltr_x_pltr_typ]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update on table "pltr_x_pltr_typ" violates foreign key constraint "fku_pltr_x_pltr_typ_pltr_id_pltr_pltr_id"')
    WHERE (SELECT pltr_id FROM pltr WHERE pltr_id = NEW.pltr_id) IS NULL;
  END#

CREATE TRIGGER fku_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id
  BEFORE UPDATE ON [pltr_x_pltr_typ]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update on table "pltr_x_pltr_typ" violates foreign key constraint "fku_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id"')
    WHERE (SELECT pltr_typ_id FROM pltr_typ WHERE pltr_typ_id = NEW.pltr_typ_id) IS NULL;
  END#

/*
    Delete on pltr_x_pltr_typ
*/

CREATE TRIGGER fkdc_pltr_x_pltr_typ_pltr_id_pltr_pltr_id
  BEFORE DELETE ON pltr
  FOR EACH ROW BEGIN
    DELETE FROM pltr_x_pltr_typ WHERE pltr_x_pltr_typ.pltr_id = OLD.pltr_id;
  END#

CREATE TRIGGER fkd_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id
  BEFORE DELETE ON pltr_typ
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "pltr_typ" violates foreign key constraint "fkd_pltr_x_pltr_typ_pltr_typ_id_pltr_typ_pltr_typ_id"')
    WHERE (SELECT pltr_typ_id FROM pltr_x_pltr_typ WHERE pltr_typ_id = OLD.pltr_typ_id) IS NOT NULL;
  END#

/*
    pltr_x_pltr_sub (m:n)
*/

CREATE TABLE pltr_x_pltr_sub (
  pltr_x_pltr_sub_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_id INTEGER NOT NULL CONSTRAINT fk_pltr_id REFERENCES pltr (pltr_id) ON DELETE CASCADE,
  pltr_sub_id INTEGER NOT NULL CONSTRAINT fk_pltr_sub_id REFERENCES pltr_sub (pltr_sub_id) ON DELETE RESTRICT )#

CREATE UNIQUE INDEX index_pltr_x_pltr_sub
  ON pltr_x_pltr_sub ( pltr_id, pltr_sub_id  )#

/*
    Foreign Key Preventing insert on pltr_x_pltr_sub
*/

CREATE TRIGGER fki_pltr_x_pltr_sub_pltr_id_pltr_pltr_id
  BEFORE INSERT ON [pltr_x_pltr_sub]
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'insert on table "pltr_x_pltr_sub" violates foreign key constraint "fki_pltr_x_pltr_sub_pltr_id_pltr_pltr_id"')
    WHERE (SELECT pltr_id FROM pltr WHERE pltr_id = NEW.pltr_id) IS NULL;
  END#

CREATE TRIGGER fki_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id
  BEFORE INSERT ON [pltr_x_pltr_sub]
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'insert on table "pltr_x_pltr_sub" violates foreign key constraint "fki_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id"')
    WHERE (SELECT pltr_sub_id FROM pltr_sub WHERE pltr_sub_id = NEW.pltr_sub_id) IS NULL;
  END#

/*
    Foreign key preventing update on pltr_x_pltr_sub
*/

CREATE TRIGGER fku_pltr_x_pltr_sub_pltr_id_pltr_pltr_id
  BEFORE UPDATE ON [pltr_x_pltr_sub]
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'update on table "pltr_x_pltr_sub" violates foreign key constraint "fku_pltr_x_pltr_sub_pltr_id_pltr_pltr_id"')
    WHERE (SELECT pltr_id FROM pltr WHERE pltr_id = NEW.pltr_id) IS NULL;
  END#

CREATE TRIGGER fku_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id
  BEFORE UPDATE ON [pltr_x_pltr_sub]
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'update on table "pltr_x_pltr_sub" violates foreign key constraint "fku_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id"')
    WHERE (SELECT pltr_sub_id FROM pltr_sub WHERE pltr_sub_id = NEW.pltr_sub_id) IS NULL;
  END#

/*
    Delete on pltr_x_pltr_sub
*/

CREATE TRIGGER fkdc_pltr_x_pltr_sub_pltr_id_pltr_pltr_id
  BEFORE DELETE ON pltr
  FOR EACH ROW BEGIN
    DELETE FROM pltr_x_pltr_sub WHERE pltr_x_pltr_sub.pltr_id = OLD.pltr_id;
  END#

CREATE TRIGGER fkd_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id
  BEFORE DELETE ON pltr_sub
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "pltr_sub" violates foreign key constraint "fkd_pltr_x_pltr_sub_pltr_sub_id_pltr_sub_pltr_sub_id"')
    WHERE (SELECT pltr_sub_id FROM pltr_x_pltr_sub WHERE pltr_sub_id = OLD.pltr_sub_id) IS NOT NULL;
  END#

/*
    fertilizer (fert)
    (1:n)
    types of fertilizer (fert_typ)
*/

CREATE TABLE fert (
  fert_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fert_name TEXT UNIQUE NOT NULL,
  fert_typ_id INTEGER NOT NULL CONSTRAINT fk_fert_typ_id REFERENCES fert_typ (fert_typ_id) ON DELETE RESTRICT,
  fert_unit TEXT NOT NULL,
  fert_n DOUBLE NOT NULL,
  fert_k2o DOUBLE NOT NULL,
  fert_p2o5 DOUBLE NOT NULL,
  fert_s DOUBLE NOT NULL,
  fert_mgo DOUBLE NOT NULL,
  fert_cao DOUBLE NOT NULL,
  fert_actv TEXT NOT NULL DEFAULT 'true' )#

CREATE TABLE fert_typ (
  fert_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fert_typ_name TEXT UNIQUE NOT NULL )#

/*
     Preventing delete on fert_typ
*/

CREATE TRIGGER fkd_fert_fert_typ_id_fert_typ_fert_typ_id
  BEFORE DELETE ON fert_typ
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "fert_typ" violates foreign key constraint "fkd_fert_fert_typ_id_fert_typ_fert_typ_id"')
    WHERE (SELECT fert_typ_id FROM fert WHERE fert_typ_id = OLD.fert_typ_id) IS NOT NULL;
  END#


CREATE VIEW v_pltr AS SELECT
  pltr.pltr_id AS id,
  pltr_name AS name,
  group_concat(DISTINCT pltr_sub_name) AS substance,
  group_concat(DISTINCT pltr_typ_name) AS type,
  pltr_unit
  FROM pltr, pltr_x_pltr_typ, pltr_x_pltr_sub, pltr_typ, pltr_sub
  WHERE (pltr.pltr_id = pltr_x_pltr_typ.pltr_id)
  AND (pltr.pltr_id = pltr_x_pltr_sub.pltr_id)
  AND (pltr_sub.pltr_sub_id = pltr_x_pltr_sub.pltr_sub_id)
  AND (pltr_typ.pltr_typ_id = pltr_x_pltr_typ.pltr_typ_id) 
  GROUP BY pltr.pltr_id#

CREATE TABLE units (
  unit_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  unit_name TEXT,
  unit_abbr TEXT )#

CREATE TABLE user (
  user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  user_name TEXT NOT NULL)#

/*
    usage (grassland, field etc.)
*/

CREATE TABLE fild_use (
  fild_use_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fild_use_name TEXT UNIQUE NOT NULL,
  fild_use_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE fild (
  fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  user_id INTEGER NOT NULL,
  fild_name TEXT NOT NULL,
  fild_nr INTEGER NOT NULL,
  fild_use_id INTEGER NOT NULL,
  year_of_harvest INTEGER NOT NULL )#

/*
    Foreign key preventing delete on user
*/

CREATE TRIGGER fkd_fild_user_id_user_user_id
  BEFORE DELETE ON user
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "user" violates foreign key constraint "fkd_fild_user_id_user_user_id"')
    WHERE (SELECT user_id FROM fild WHERE user_id = OLD.user_id) IS NOT NULL;
  END#

/*
    Foreign key preventing delete on fild_use
*/

CREATE TRIGGER fkd_fild_fild_use_id_fild_use_fild_use_id
  BEFORE DELETE ON fild_use
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "fild_use" violates foreign key constraint "fkd_fild_fild_use_id_fild_use_fild_use_id"')
    WHERE (SELECT fild_use_id FROM fild WHERE fild_use_id = OLD.fild_use_id) IS NOT NULL;
  END#


CREATE UNIQUE INDEX index_fild_nr_x_year_of_harvest
  ON fild ( user_id, fild_nr, year_of_harvest )#

CREATE TABLE lot (
  lot_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  gmrk TEXT NOT NULL DEFAULT '',
  flur TEXT NOT NULL DEFAULT '',
  lot_nr INTEGER NOT NULL,
  lot_sub_nr INTEGER,
  lot_size DOUBLE NOT NULL,
  year_of_harvest INTEGER NOT NULL,
  soil_typ_id INTEGER,
  lot_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE UNIQUE INDEX index_gmrk_x_flur_x_lot_nr_x_year_of_harvest
  ON lot ( gmrk, flur, lot_nr, lot_sub_nr, year_of_harvest )#

CREATE TABLE lot_x_fild (
  lot_x_fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  lot_id INTEGER NOT NULL,
  fild_id INTEGER NOT NULL,
  lot_x_fild_size DOUBLE NOT NULL )#

/*
    Trigger preventing update on lot if total(lot_x_fild_size) > lot_size
*/

CREATE TRIGGER u_lot_lot_id_lot_x_fild_lot_id_lot_size
  BEFORE UPDATE ON [lot]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update on table "lot" violates constraint "total(lot_x_fild_size) > lot_size"')
    WHERE ( SELECT (SELECT total(lot_x_fild_size) FROM lot_x_fild  WHERE lot_x_fild.lot_id=OLD.lot_id) AS size1
            WHERE size1 > (SELECT NEW.lot_size FROM lot WHERE lot.lot_id=OLD.lot_id)) IS NOT NULL;

  END#

/*
    Trigger preventing update on lot_x_fild_size if actn_fild.area > total(lot_x_fild_size)
    OR lot_size > total(lot_x_fild_size)
*/

CREATE TRIGGER u_lot_x_fild_fild_id_actn_fild_area
  BEFORE UPDATE ON lot_x_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update on table "lot_x_fild" violates constraint "actn_fild.area <= total(lot_x_fild_size)"')
    WHERE ( SELECT actn_fild.fild_id FROM actn_fild
            WHERE actn_fild.fild_id=OLD.fild_id 
            AND actn_fild.area > 
            ((SELECT total(lot_x_fild_size) FROM lot_x_fild WHERE fild_id=OLD.fild_id)-OLD.lot_x_fild_size+NEW.lot_x_fild_size) ) IS NOT NULL
            /*OR
            ( SELECT total(lot_size)-((SELECT total(lot_x_fild_size) FROM lot_x_fild WHERE lot_id=OLD.lot_id)-OLD.lot_x_fild_size+NEW.lot_x_fild_size) FROM lot
              WHERE lot.lot_id=OLD.lot_id ) != 0*/;
  END#

/*
    Trigger preventing delete on lot_x_fild_size if actn_fild.area > total(lot_x_fild_size)
*/

CREATE TRIGGER d_lot_x_fild_fild_id_actn_fild_area
  BEFORE DELETE ON lot_x_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'delete on table "lot_x_fild" violates constraint "actn_fild.area <= total(lot_x_fild_size)"')
    WHERE ( SELECT actn_fild.fild_id FROM actn_fild
            WHERE actn_fild.fild_id=OLD.fild_id 
            AND actn_fild.area > 
            ((SELECT total(lot_x_fild_size) FROM lot_x_fild WHERE fild_id=OLD.fild_id)-OLD.lot_x_fild_size) ) IS NOT NULL;
  END#


CREATE TRIGGER lot_x_fild_on_fild_del
  AFTER DELETE ON fild
  BEGIN
  DELETE FROM lot_x_fild WHERE fild_id=OLD.fild_id;
  END#

CREATE TRIGGER lot_x_fild_on_lot_del
  AFTER DELETE ON lot
  BEGIN
  DELETE FROM lot_x_fild WHERE lot_id = old.lot_id;
  END#

CREATE VIEW v_fild AS
  SELECT fild.fild_id AS fild_id, user_name, fild_name, group_concat(lot_nr, '/') AS lots, total(lot_x_fild_size) AS total, fild.year_of_harvest AS year
  FROM fild, lot, lot_x_fild, user
  WHERE fild.fild_id=lot_x_fild.fild_id
  AND lot.lot_id=lot_x_fild.lot_id
  AND fild.user_id=user.user_id
  GROUP BY fild.fild_id#

CREATE VIEW v_fild_info AS
  SELECT fild.fild_id, fild_name, fild_nr, fild_use_name,
  group_concat(DISTINCT lot_nr) AS lot_nrs, sum(lot_x_fild_size) AS sum
  FROM fild, fild_use, lot, lot_x_fild
  WHERE fild.fild_id=lot_x_fild.fild_id
  AND lot.lot_id=lot_x_fild.lot_id
  AND fild.fild_use_id=fild_use.fild_use_id
  GROUP BY fild.fild_id#

CREATE TABLE evnt_prg (
  evnt_prg_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  evnt_prg_name TEXT UNIQUE NOT NULL,
  evnt_prg_abbr TEXT UNIQUE NOT NULL,
  evnt_prg_desc TEXT,
  evnt_prg_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE crop (
  crop_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_abbr TEXT UNIQUE,
  crop_name TEXT NOT NULL UNIQUE,
  crop_name_lat TEXT NOT NULL)#

CREATE TABLE crop_use (
  crop_use_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_id INTEGER DEFAULT NULL,
  crop_use_name TEXT NOT NULL UNIQUE,
  crop_use_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

/*
    Foreign key preventing delete on crop
*/

CREATE TRIGGER fkd_crop_use_crop_id_crop_crop_id
  BEFORE DELETE ON crop
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "crop_use" violates foreign key constraint "fkd_crop_use_crop_id_crop_crop_id"')
    WHERE (SELECT crop_id FROM crop_use WHERE crop_id = OLD.crop_id) IS NOT NULL;
  END#

CREATE TABLE cultivar (
  cultivar_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_use_id INTEGER NOT NULL,
  cultivar_name TEXT NOT NULL)#

CREATE UNIQUE INDEX idx_crop_use_id_x_cultivar_name
  ON cultivar ( crop_use_id, cultivar_name )#

/*
    Foreign key preventing delete on crop_use
*/

CREATE TRIGGER fkd_cultivar_crop_use_id_crop_use_crop_use_id
  BEFORE DELETE ON crop_use
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "crop_use" violates foreign key constraint "fkd_cultivar_crop_use_id_crop_use_crop_use_id"')
    WHERE (SELECT crop_use_id FROM cultivar WHERE crop_use_id = OLD.crop_use_id) IS NOT NULL;
  END#


CREATE TABLE seed (
  seed_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  cultivar_id INTEGER,
  seed_name TEXT,
  seed_is_actv BOOLEAN NOT NULL DEFAULT 'true')#
  
CREATE TABLE crop_mix (
  crop_mix_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  seed_id INTEGER NOT NULL,
  cultivar_id INTEGER NOT NULL,
  crop_mix_amt INTEGER NOT NULL DEFAULT 100)#
  
CREATE UNIQUE INDEX idx_cultivar_id_x_seed_name
  ON seed ( cultivar_id, seed_name )#
  
CREATE TRIGGER seed_on_cultivar_ins
  AFTER INSERT ON cultivar
  BEGIN
  INSERT INTO seed (cultivar_id) VALUES(new.cultivar_id);
  END#

/*
    Foreign key preventing delete on cultivar part of a crop_mix
*/

CREATE TRIGGER fkd_crop_mix_cultivar_id_cultivar_cultivar_id
  BEFORE DELETE ON cultivar
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "cultivar" violates foreign key constraint "fkd_crop_mix_cultivar_id_cultivar_cultivar_id"')
    WHERE (SELECT cultivar_id FROM crop_mix WHERE cultivar_id = OLD.cultivar_id) IS NOT NULL;
  END#

CREATE TRIGGER seed_on_cultivar_del
  AFTER DELETE ON cultivar
  BEGIN
  DELETE FROM seed WHERE cultivar_id = old.cultivar_id;
  END#

CREATE VIEW v_cultivar AS
  SELECT cultivar_id, crop_use_name || ' (' || cultivar_name || ')' AS name
  FROM cultivar, crop_use
  WHERE cultivar.crop_use_id=crop_use.crop_use_id#
  
CREATE VIEW v_crop_mix AS
  SELECT crop_mix_id, seed_name, group_concat(crop_use_name || ' (' || cultivar_name || ') ' || crop_mix_amt || '%', ', ') AS cultivars
  FROM crop_mix, seed, crop_use, cultivar
  WHERE crop_mix.seed_id=seed.seed_id
  AND crop_mix.cultivar_id=cultivar.cultivar_id
  AND cultivar.crop_use_id=crop_use.crop_use_id
  GROUP BY crop_mix.seed_id#
  
CREATE VIEW v_seed AS
  SELECT seed_id, name AS seed_name
  FROM seed, v_cultivar
  WHERE seed.cultivar_id=v_cultivar.cultivar_id
  AND seed_name IS NULL
  AND seed_is_actv = 'true'
  UNION
  SELECT seed_id, seed_name
  FROM seed
  WHERE seed_is_actv = 'true'
  AND seed_name IS NOT NULL#

CREATE TABLE oprt (
  oprt_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  oprt_name TEXT UNIQUE NOT NULL,
  oprt_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE work (
  work_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  work_name TEXT UNIQUE NOT NULL,
  work_ak DOUBLE NOT NULL,
  work_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE anim_spc (
  anim_spc_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  anim_spc_name TEXT UNIQUE NOT NULL,
  anim_spc_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

CREATE TABLE anim_grp (
  anim_grp_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  anim_grp_name TEXT UNIQUE NOT NULL,
  anim_spc_id INTEGER NOT NULL,
  anim_grp_cont INTEGER NOT NULL,
  anim_unit DOUBLE NOT NULL,
  anim_grp_is_actv BOOLEAN NOT NULL DEFAULT 'true' )#

/*
    Foreign key preventing delete on anim_spc
*/

CREATE TRIGGER fkd_anim_grp_anim_spc_id_anim_spc_anim_spc_id
  BEFORE DELETE ON anim_spc
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table anim_spc violates foreign key constraint "fkd_prod_crop_use_id_crop_use_crop_use_id"')
    WHERE (SELECT anim_spc_id FROM anim_grp WHERE anim_spc_id = OLD.anim_spc_id) IS NOT NULL;
  END#

CREATE TABLE prod (
  prod_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fild_id INTEGER UNIQUE NOT NULL,
  crop_use_id INTEGER NOT NULL )#

/*
    Foreign key preventing delete on crop_use
*/

CREATE TRIGGER fkd_prod_crop_use_id_crop_use_crop_use_id
  BEFORE DELETE ON crop_use
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table crop_use violates foreign key constraint "fkd_prod_crop_use_id_crop_use_crop_use_id"')
    WHERE (SELECT crop_use_id FROM prod WHERE crop_use_id = OLD.crop_use_id) IS NOT NULL;
  END#


CREATE TABLE actn (
  actn_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_date TEXT NOT NULL,
  work_id INTEGER NOT NULL,
  oprt_id INTEGER NOT NULL,
  year_of_harvest INTEGER NOT NULL,
  actn_com TEXT NOT NULL )#

/*
    Preventing delete on work
*/

CREATE TRIGGER fkd_actn_work_id_work_work_id
  BEFORE DELETE ON work
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "work" violates foreign key constraint "fkd_actn_work_id_work_work_id"')
    WHERE (SELECT work_id FROM actn WHERE work_id = OLD.work_id) IS NOT NULL;
  END#

/*
    Preventing delete on oprt
*/

CREATE TRIGGER fkd_actn_oprt_id_oprt_oprt_id
  BEFORE DELETE ON oprt
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "oprt" violates foreign key constraint "fkd_actn_oprt_id_oprt_oprt_id"')
    WHERE (SELECT oprt_id FROM actn WHERE oprt_id = OLD.oprt_id) IS NOT NULL;
  END#


CREATE TABLE actn_fild (
  actn_fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  fild_id INTEGER NOT NULL,
  area DOUBLE NOT NULL,
  fild_com TEXT NOT NULL )#

/*
    Preventing delete on fild
*/

CREATE TRIGGER fkd_actn_fild_fild_id_fild_fild_id
  BEFORE DELETE ON fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "fild" violates foreign key constraint "fkd_actn_fild_fild_id_fild_fild_id"')
    WHERE (SELECT fild_id FROM actn_fild WHERE fild_id = OLD.fild_id) IS NOT NULL;
  END#

/*
    Preventing update/insert on fild area > total(lot_x_fild_size)


CREATE TRIGGER u_actn_fild_area_lot_x_fild_lot_x_fild_size
  BEFORE UPDATE ON actn_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'update on table "actn_fild" violates constraint "u_actn_fild_area_lot_x_fild_lot_x_fild_size"')
    WHERE (SELECT t.total FROM (SELECT total(lot_x_fild_size) AS total FROM lot_x_fild WHERE fild_id = OLD.fild_id) AS t WHERE NEW.area > t.total) IS NOT NULL;
  END

CREATE TRIGGER i_actn_fild_area_lot_x_fild_lot_x_fild_size
  BEFORE INSERT ON actn_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'insert on table "actn_fild" violates constraint "i_actn_fild_area_lot_x_fild_lot_x_fild_size"')
    WHERE (SELECT t.total FROM (SELECT total(lot_x_fild_size) AS total FROM lot_x_fild WHERE fild_id = NEW.fild_id) AS t WHERE NEW.area > t.total) IS NOT NULL;
  END

*/

CREATE VIEW v_actn AS
  SELECT actn_id, actn_date, work_name, oprt_name, year_of_harvest, actn_com
  FROM actn, work, oprt
  WHERE actn.work_id=work.work_id
  AND actn.oprt_id=oprt.oprt_id#

CREATE TABLE actn_fert (
  actn_fert_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  fert_id INTEGER NOT NULL CONSTRAINT fk_fert_id REFERENCES fert (fert_id) ON DELETE RESTRICT,
  actn_fert_amt DOUBLE NOT NULL )#

/*
    Preventing delete on fert
*/

CREATE TRIGGER fkd_actn_fert_fert_id_fert_fert_id
  BEFORE DELETE ON fert
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table "fert" violates foreign key constraint "fkd_actn_fert_fert_id_fert_fert_id"')
    WHERE (SELECT fert_id FROM actn_fert WHERE fert_id = OLD.fert_id) IS NOT NULL;
  END#


CREATE TABLE actn_pltr (
  actn_pltr_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  pltr_id INTEGER NOT NULL CONSTRAINT fk_pltr_id REFERENCES pltr (pltr_id) ON DELETE RESTRICT,
  actn_pltr_amt DOUBLE NOT NULL )#

/*
    Foreign key preventing delete on pltr
*/

CREATE TRIGGER fkd_actn_pltr_pltr_id_pltr_pltr_id
  BEFORE DELETE ON pltr
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table pltr violates foreign key constraint')
    WHERE (SELECT pltr_id FROM actn_pltr WHERE pltr_id = OLD.pltr_id) IS NOT NULL;
  END#

CREATE TABLE actn_dril (
  actn_dril_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  seed_id INTEGER NOT NULL,
  actn_dril_amt DOUBLE NOT NULL )#

/*
    Foreign key preventing delete on seed
*/

CREATE TRIGGER fkd_actn_dril_seed_id_seed_seed_id
  BEFORE DELETE ON seed
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table seed violates foreign key constraint')
    WHERE (SELECT seed_id FROM actn_dril WHERE seed_id = OLD.seed_id) IS NOT NULL;
  END#

CREATE TABLE actn_harv (
  actn_harv_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  crop_use_id INTEGER NOT NULL,
  actn_harv_amt DOUBLE NOT NULL )#

/*
    Foreign key preventing delete on crop_use
*/

CREATE TRIGGER fkd_actn_harv_crop_use_id_crop_use_crop_use_id
  BEFORE DELETE ON crop_use
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table crop_use violates foreign key constraint "fkd_actn_harv_crop_use_id_crop_use_crop_use_id"')
    WHERE (SELECT crop_use_id FROM actn_harv WHERE crop_use_id = OLD.crop_use_id) IS NOT NULL;
  END#

CREATE TABLE actn_soil (
  actn_soil_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  actn_soil_name TEXT NOT NULL DEFAULT 'Bodenbearbeitung' )#

CREATE TABLE actn_medw (
  actn_medw_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  actn_medw_name TEXT NOT NULL DEFAULT 'Grünlandpflege' )#

CREATE TABLE actn_misc (
  actn_misc_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  actn_misc_name TEXT NOT NULL DEFAULT 'Sonstiges' )#
  
CREATE TABLE actn_anim_grp (
  actn_anim_grp_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL,
  anim_grp_id INTEGER NOT NULL,
  actn_anim_grp_amt INTEGER NOT NULL )#

/*
    Foreign key preventing delete on anim_grp
*/

CREATE TRIGGER fkd_actn_anim_grp_anim_grp_id_anim_grp_anim_grp_id
  BEFORE DELETE ON anim_grp
  FOR EACH ROW BEGIN
    SELECT RAISE(ROLLBACK, 'delete on table anim_grp violates foreign key constraint "fkd_actn_anim_grp_anim_grp_id_anim_grp_anim_grp_id"')
    WHERE (SELECT anim_grp_id FROM actn_anim_grp WHERE anim_grp_id = OLD.anim_grp_id) IS NOT NULL;
  END#

/*
    Cascading delete on actn
*/

CREATE TRIGGER fkd_actn_actn_id_actns_actn_id
  AFTER DELETE ON actn
  FOR EACH ROW BEGIN
  DELETE FROM actn_fild WHERE actn_id=old.actn_id;
  DELETE FROM actn_fert WHERE actn_id=old.actn_id;
  DELETE FROM actn_pltr WHERE actn_id=old.actn_id;
  DELETE FROM actn_dril WHERE actn_id=old.actn_id;
  DELETE FROM actn_harv WHERE actn_id=old.actn_id;
  DELETE FROM actn_soil WHERE actn_id=old.actn_id;
  DELETE FROM actn_medw WHERE actn_id=old.actn_id;
  DELETE FROM actn_anim_grp WHERE actn_id=old.actn_id;
  DELETE FROM actn_misc WHERE actn_id=old.actn_id;
  END#

CREATE VIEW v_actn_detail AS
  SELECT actn_fert.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, fert_name AS input_name, actn_fert_amt AS input_amt, area * actn_fert_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, fert, actn_fert
  WHERE actn_fert.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_fert.fert_id=fert.fert_id
  UNION ALL
  SELECT actn_pltr.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, pltr_name AS input_name, actn_pltr_amt AS input_amt, area * actn_pltr_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_pltr, pltr
  WHERE actn_pltr.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_pltr.pltr_id=pltr.pltr_id
  UNION ALL
  SELECT actn_dril.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, seed_name AS input_name, actn_dril_amt AS input_amt, area * actn_dril_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_dril, v_seed
  WHERE actn_dril.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_dril.seed_id=v_seed.seed_id
  UNION ALL
  SELECT actn_harv.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, crop_use_name AS input_name, actn_harv_amt AS input_amt, area * actn_harv_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_harv, crop_use
  WHERE actn_harv.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_harv.crop_use_id=crop_use.crop_use_id
  UNION ALL
  SELECT actn_soil.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_soil_name, NULL, NULL, fild_com
  FROM fild, actn_fild, actn_soil
  WHERE actn_soil.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  UNION ALL
  SELECT actn_medw.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_medw_name, NULL, NULL, fild_com
  FROM fild, actn_fild, actn_medw
  WHERE actn_medw.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  UNION ALL
  SELECT actn_misc.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_misc_name, NULL, NULL, fild_com
  FROM fild, actn_fild, actn_misc
  WHERE actn_misc.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  UNION ALL
  SELECT actn_anim_grp.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, anim_grp_name, actn_anim_grp_amt / area AS input_amt, actn_anim_grp_amt AS sum_amt, fild_com
  FROM fild, actn_fild, actn_anim_grp, anim_grp
  WHERE actn_anim_grp.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_anim_grp.anim_grp_id=anim_grp.anim_grp_id#

CREATE VIEW v_actn_fert AS
  SELECT actn_fert.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, fert_name AS input_name, actn_fert_amt AS input_amt, area * actn_fert_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, fert, actn_fert
  WHERE actn_fert.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_fert.fert_id=fert.fert_id#
  
CREATE VIEW v_actn_pltr AS
  SELECT actn_pltr.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, pltr_name AS input_name, actn_pltr_amt AS input_amt, area * actn_pltr_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_pltr, pltr
  WHERE actn_pltr.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_pltr.pltr_id=pltr.pltr_id#
  
CREATE VIEW v_actn_dril AS
  SELECT actn_dril.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, seed_name AS input_name, actn_dril_amt AS input_amt, area * actn_dril_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_dril, v_seed
  WHERE actn_dril.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_dril.seed_id=v_seed.seed_id#
  
CREATE VIEW v_actn_harv AS
  SELECT actn_harv.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, crop_use_name AS input_name, actn_harv_amt AS input_amt, area * actn_harv_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_harv, crop_use
  WHERE actn_harv.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_harv.crop_use_id=crop_use.crop_use_id#
  
CREATE VIEW v_actn_soil AS
  SELECT actn_soil.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_soil_name, fild_com
  FROM fild, actn_fild, actn_soil
  WHERE actn_soil.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id#
  
CREATE VIEW v_actn_medw AS
  SELECT actn_medw.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_medw_name, fild_com
  FROM fild, actn_fild, actn_medw
  WHERE actn_medw.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id#
  
CREATE VIEW v_actn_misc AS
  SELECT actn_misc.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_misc_name, fild_com
  FROM fild, actn_fild, actn_misc
  WHERE actn_misc.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id#
  
CREATE VIEW v_actn_anim_grp AS
  SELECT actn_anim_grp.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, anim_grp_name, actn_anim_grp_amt AS input_amt, fild_com
  FROM fild, actn_fild, actn_anim_grp, anim_grp
  WHERE actn_anim_grp.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_anim_grp.anim_grp_id=anim_grp.anim_grp_id#















