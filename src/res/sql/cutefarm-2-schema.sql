/*
    CuteFarm sqlite database schema version 2  
    PRAGMA encoding = 'UTF-8'
    
    TODO:
    trigger to check delete of all actn_x after delete on actn_fild   
    
    CHANGES:
    removed columns:
 
    added columns:
    fert.fert_withdrawal_humus
    crop_use_harv_unit
    crop_use_withdrawal_n
    crop_use_withdrawal_k2o
    crop_use_withdrawal_p2o5
    crop_use_withdrawal_s
    crop_use_withdrawal_mgo
    crop_use_withdrawal_cao
    crop_use_withdrawal_humus
    cultivar.seed_unit
    
    added table:
    actn_soil_test
    fild_x_evnt_prg
    
    added index:
    index_unique_lot_x_fild_lot_field
    
    added trigger:
    trig_upd_lot_x_fild_year_of_harvest
    trig_ins_lot_x_fild_year_of_harvest
    trg_del_prod
    trg_upd_prod
    
*/

BEGIN;

PRAGMA user_version = 200;

/*
    General farm data
*/

CREATE TABLE farm (
/*
General farm data
*/
  farm_name TEXT UNIQUE NOT NULL,
  farm_str TEXT NOT NULL,
  farm_city_code TEXT NOT NULL,
  farm_numb TEXT NOT NULL
);

/*
    Soil data
*/

CREATE TABLE soil_typ (
  soil_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_typ_name TEXT,
  soil_typ_abbr TEXT NOT NULL,
  soil_sys_id INTEGER NOT NULL,
  FOREIGN KEY (soil_sys_id) REFERENCES soil_sys (soil_sys_id) DEFERRABLE INITIALLY DEFERRED
);

CREATE TABLE soil_typ_def (
  soil_typ_def_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_typ_id INTEGER NOT NULL,
  soil_typ_def_x DOUBLE NOT NULL,
  soil_typ_def_y DOUBLE NOT NULL,
  FOREIGN KEY (soil_typ_id) REFERENCES soil_typ (soil_typ_id) DEFERRABLE INITIALLY DEFERRED
);

CREATE TABLE soil_sys (
  soil_sys_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  soil_sys_name TEXT UNIQUE NOT NULL
);

/*
    planttreatment (pltr)
    types of planttreatment (pltr_typ)
    active ingrediences (pltr_sub)
*/

CREATE TABLE pltr (
/*
planttreatment, pesticides
*/
  pltr_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_name TEXT UNIQUE NOT NULL,
  pltr_unit TEXT NOT NULL,
  pltr_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE pltr_typ (
/*
types of planttreatment
*/
  pltr_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_typ_name TEXT UNIQUE NOT NULL
);

CREATE TABLE pltr_sub (
/*
active ingrediences
*/
  pltr_sub_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_sub_name TEXT UNIQUE NOT NULL
);

/*
    pltr_x_pltr_typ (m:n)
*/

CREATE TABLE pltr_x_pltr_typ (
  pltr_x_pltr_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_id INTEGER NOT NULL REFERENCES pltr (pltr_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  pltr_typ_id INTEGER NOT NULL REFERENCES pltr_typ (pltr_typ_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED
);

CREATE UNIQUE INDEX index_pltr_x_pltr_typ
 ON pltr_x_pltr_typ ( pltr_id, pltr_typ_id  );



/*
    pltr_x_pltr_sub (m:n)
*/

CREATE TABLE pltr_x_pltr_sub (
  pltr_x_pltr_sub_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_id INTEGER NOT NULL REFERENCES pltr (pltr_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  pltr_sub_id INTEGER NOT NULL REFERENCES pltr_sub (pltr_sub_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED
);

CREATE UNIQUE INDEX index_pltr_x_pltr_sub
  ON pltr_x_pltr_sub ( pltr_id, pltr_sub_id  );


/*
    fertilizer (fert)
    (1:n)
    types of fertilizer (fert_typ)
*/

CREATE TABLE fert (
  fert_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fert_name TEXT UNIQUE NOT NULL,
  fert_typ_id INTEGER NOT NULL REFERENCES fert_typ (fert_typ_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  fert_unit TEXT NOT NULL DEFAULT 'dt',
  fert_n DOUBLE NOT NULL DEFAULT 0.00,
  fert_k2o DOUBLE NOT NULL DEFAULT 0.00,
  fert_p2o5 DOUBLE NOT NULL DEFAULT 0.00,
  fert_s DOUBLE NOT NULL DEFAULT 0.00,
  fert_mgo DOUBLE NOT NULL DEFAULT 0.00,
  fert_cao DOUBLE NOT NULL DEFAULT 0.00,
  fert_withdrawal_humus DOUBLE NOT NULL DEFAULT 0.00,
  fert_actv TEXT NOT NULL DEFAULT 'true'
);

CREATE TABLE fert_typ (
  fert_typ_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fert_typ_name TEXT UNIQUE NOT NULL
);

CREATE TABLE units (
  unit_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  unit_name TEXT,
  unit_abbr TEXT
);

CREATE TABLE user (
  user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  user_name TEXT NOT NULL
);

/*
    usage (grassland, arable etc.)
*/

CREATE TABLE fild_use (
/*
    usage (grassland, arable etc.)
*/
  fild_use_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fild_use_name TEXT UNIQUE NOT NULL,
  fild_use_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE fild (
  fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  user_id INTEGER NOT NULL REFERENCES user (user_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  fild_name TEXT NOT NULL,
  fild_nr INTEGER NOT NULL CHECK (fild_nr >= 0),
  fild_use_id INTEGER NOT NULL REFERENCES fild_use (fild_use_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  year_of_harvest INTEGER NOT NULL CHECK (year_of_harvest >= 2000)
);

CREATE UNIQUE INDEX index_fild_nr_x_year_of_harvest
  ON fild ( user_id, fild_nr, year_of_harvest);


CREATE TABLE lot (
  lot_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  gmrk TEXT NOT NULL DEFAULT '',
  flur TEXT NOT NULL DEFAULT '',
  lot_nr INTEGER NOT NULL CHECK (lot_nr >= 0),
  lot_sub_nr INTEGER CHECK (lot_sub_nr >= 0),
  lot_size DOUBLE NOT NULL CHECK (lot_size > 0),
  year_of_harvest INTEGER NOT NULL CHECK (year_of_harvest >= 2000),
  soil_typ_id INTEGER,
  lot_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE UNIQUE INDEX index_gmrk_x_flur_x_lot_nr_x_year_of_harvest
  ON lot ( gmrk, flur, lot_nr, lot_sub_nr, year_of_harvest );

CREATE TABLE lot_x_fild (
  lot_x_fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  lot_id INTEGER NOT NULL,
  fild_id INTEGER NOT NULL,
  lot_x_fild_size DOUBLE NOT NULL CHECK (lot_x_fild_size > 0),
  FOREIGN KEY (lot_id) REFERENCES lot (lot_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  FOREIGN KEY (fild_id) REFERENCES fild (fild_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED
  );
  
CREATE UNIQUE INDEX index_unique_lot_x_fild_lot_field
  ON lot_x_fild ( lot_id, fild_id );
 
/*
    Trigger preventing insert/update on lot if lot.year_of_harvest != fild.year_of_harvest
  */ 
 
CREATE TRIGGER trig_upd_lot_x_fild_year_of_harvest
  BEFORE UPDATE ON lot_x_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update or insert on table "lot_x_fild" violates constraint lot.year_of_harvest != fild.year_of_harvest')
    WHERE ( (SELECT year_of_harvest FROM fild WHERE fild_id=NEW.fild_id) != (SELECT year_of_harvest FROM lot WHERE lot_id=NEW.lot_id) );
  END;
  
CREATE TRIGGER trig_ins_lot_x_fild_year_of_harvest
  BEFORE INSERT ON lot_x_fild
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update or insert on table "lot_x_fild" violates constraint lot.year_of_harvest != fild.year_of_harvest')
    WHERE ( (SELECT year_of_harvest FROM fild WHERE fild_id=NEW.fild_id) != (SELECT year_of_harvest FROM lot WHERE lot_id=NEW.lot_id) );
  END;


/*
    Trigger preventing update on lot if total(lot_x_fild_size) > lot_size
*/

CREATE TRIGGER u_lot_lot_id_lot_x_fild_lot_id_lot_size
  BEFORE UPDATE ON [lot]
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'update on table "lot" violates constraint "total(lot_x_fild_size) <= lot_size"')
    WHERE ( SELECT (SELECT total(lot_x_fild_size) FROM lot_x_fild  WHERE lot_x_fild.lot_id=OLD.lot_id) AS size1
            WHERE size1 > (SELECT NEW.lot_size FROM lot WHERE lot.lot_id=OLD.lot_id)) IS NOT NULL;

  END;

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
  END;

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
  END;




CREATE TABLE evnt_prg (
/*
    environmental schemas
*/
  evnt_prg_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  evnt_prg_name TEXT UNIQUE NOT NULL,
  evnt_prg_abbr TEXT UNIQUE NOT NULL,
  evnt_prg_desc TEXT,
  evnt_prg_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE fild_x_evnt_prg (
/*
    field environmental schemas
*/
  fild_x_evnt_prg_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fild_id  INTEGER NOT NULL,
  evnt_prg_id INTEGER NOT NULL,
  evnt_prg_exps INTEGER NOT NULL CHECK (evnt_prg_exps >= 2000),

  FOREIGN KEY (evnt_prg_id) REFERENCES evnt_prg (evnt_prg_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  FOREIGN KEY (fild_id) REFERENCES fild (fild_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED
);

CREATE UNIQUE INDEX idx_unique_fild_x_evnt_prg ON fild_x_evnt_prg (fild_id, evnt_prg_id);

CREATE TABLE crop (
  crop_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_abbr TEXT UNIQUE,
  crop_name TEXT NOT NULL UNIQUE,
  crop_name_lat TEXT NOT NULL
);

CREATE TABLE crop_use (
  crop_use_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_id INTEGER DEFAULT NULL REFERENCES crop (crop_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  crop_use_name TEXT NOT NULL UNIQUE,
  crop_use_harv_unit TEXT NOT NULL DEFAULT 'dt', -- new in 2.0
  crop_use_withdrawal_n DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_k2o DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_p2o5 DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_s DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_mgo DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_cao DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_withdrawal_humus DOUBLE NOT NULL DEFAULT 0.00, -- new in 2.0
  crop_use_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);


CREATE TABLE cultivar (
  cultivar_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  crop_use_id INTEGER NOT NULL REFERENCES crop_use (crop_use_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  cultivar_name TEXT NOT NULL,
  seed_unit TEXT NOT NULL DEFAULT 'kg' --new in 2.0
);

CREATE UNIQUE INDEX idx_crop_use_id_x_cultivar_name
  ON cultivar ( crop_use_id, cultivar_name );


CREATE TABLE seed (
  seed_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  cultivar_id INTEGER /*NOT NULL*/ REFERENCES cultivar (cultivar_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED, -- comp. 1.0 
  seed_name TEXT,
  seed_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);
  
CREATE TABLE crop_mix (
  crop_mix_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  seed_id INTEGER NOT NULL REFERENCES seed (seed_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  cultivar_id INTEGER NOT NULL REFERENCES cultivar (cultivar_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  crop_mix_amt INTEGER NOT NULL DEFAULT 100
);
  
CREATE UNIQUE INDEX idx_cultivar_id_x_seed_name
  ON seed ( cultivar_id, seed_name );


CREATE TRIGGER seed_on_cultivar_ins
  AFTER INSERT ON cultivar
  BEGIN
  INSERT INTO seed (cultivar_id) VALUES(new.cultivar_id);
  END;

CREATE TABLE oprt (
  oprt_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  oprt_name TEXT UNIQUE NOT NULL,
  oprt_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE work (
  work_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  work_name TEXT UNIQUE NOT NULL,
  work_ak DOUBLE NOT NULL CHECK (work_ak >= 0),
  work_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE anim_spc (
  anim_spc_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  anim_spc_name TEXT UNIQUE NOT NULL,
  anim_spc_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE anim_grp (
  anim_grp_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  anim_grp_name TEXT UNIQUE NOT NULL,
  anim_spc_id INTEGER NOT NULL REFERENCES anim_spc (anim_spc_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  anim_grp_cont INTEGER NOT NULL CHECK (anim_grp_cont >= 0),
  anim_unit DOUBLE NOT NULL CHECK (anim_unit >= 0),
  anim_grp_is_actv BOOLEAN NOT NULL DEFAULT 'true'
);

CREATE TABLE prod (
  prod_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fild_id INTEGER UNIQUE NOT NULL REFERENCES fild (fild_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  crop_use_id INTEGER NOT NULL REFERENCES crop_use (crop_use_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED
);

/*
    Trigger preventing delete on prod actn_fild.fild_id = prod.fild_id
*/

CREATE TRIGGER trg_del_prod
  BEFORE DELETE ON prod
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'Delete on table "prod" violates constraint "actn_fild.fild_id = prod.fild_id"')
    WHERE ( SELECT actn_fild.fild_id FROM actn_fild
            WHERE actn_fild.fild_id=OLD.fild_id ) IS NOT NULL;
  END;

/*
    Trigger preventing update on prod actn_fild.fild_id = prod.fild_id
*/
  
CREATE TRIGGER trg_upd_prod
  BEFORE UPDATE ON prod
  FOR EACH ROW BEGIN
    SELECT RAISE(ABORT, 'Update on table "prod" violates constraint "actn_fild.fild_id = prod.fild_id"')
    WHERE ( SELECT actn_fild.fild_id FROM actn_fild
            WHERE actn_fild.fild_id=OLD.fild_id ) IS NULL
            AND ( SELECT count(actn_fild.fild_id) FROM actn_fild
            WHERE actn_fild.fild_id=OLD.fild_id) > 0;
  END;


CREATE TABLE actn (
  actn_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_date TEXT NOT NULL,
  work_id INTEGER NOT NULL REFERENCES work (work_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  oprt_id INTEGER NOT NULL REFERENCES oprt (oprt_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  year_of_harvest INTEGER NOT NULL CHECK (year_of_harvest >= 2000),
  actn_com TEXT NOT NULL DEFAULT ''
);
  
CREATE TABLE actn_fild (
  actn_fild_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  fild_id INTEGER NOT NULL REFERENCES fild (fild_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  area DOUBLE NOT NULL CHECK (area >= 0),
  fild_com TEXT NOT NULL DEFAULT ''
);

CREATE TABLE actn_fert (
  actn_fert_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  fert_id INTEGER NOT NULL REFERENCES fert (fert_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_fert_amt DOUBLE NOT NULL CHECK (actn_fert_amt > 0)
);

CREATE TABLE actn_pltr (
  actn_pltr_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  pltr_id INTEGER NOT NULL REFERENCES pltr (pltr_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_pltr_amt DOUBLE NOT NULL CHECK (actn_pltr_amt > 0)
);

CREATE TABLE actn_dril (
  actn_dril_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  seed_id INTEGER NOT NULL REFERENCES seed (seed_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_dril_amt DOUBLE NOT NULL CHECK (actn_dril_amt > 0)
);

CREATE TABLE actn_harv (
  actn_harv_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  crop_use_id INTEGER NOT NULL REFERENCES crop_use (crop_use_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_harv_amt DOUBLE NOT NULL CHECK (actn_harv_amt > 0)
);

CREATE TABLE actn_soil (
  actn_soil_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL UNIQUE REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_soil_name TEXT NOT NULL DEFAULT 'Tillage'
);

CREATE TABLE actn_medw (
  actn_medw_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id  INTEGER NOT NULL UNIQUE REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_medw_name TEXT NOT NULL DEFAULT 'Pasture'
);

CREATE TABLE actn_misc (
  actn_misc_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL UNIQUE REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_misc_name TEXT NOT NULL DEFAULT 'Misc'
);
  
CREATE TABLE actn_anim_grp (
  actn_anim_grp_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  anim_grp_id INTEGER NOT NULL REFERENCES anim_grp (anim_grp_id) ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  actn_anim_grp_amt INTEGER NOT NULL CHECK (actn_anim_grp_amt > 0)
);

CREATE TABLE actn_soil_test (
  actn_soil_test_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  actn_id INTEGER NOT NULL UNIQUE REFERENCES actn (actn_id) ON DELETE CASCADE ON UPDATE CASCADE DEFERRABLE INITIALLY DEFERRED,
  soil_test_n DOUBLE NOT NULL CHECK (soil_test_n >= 0),
  soil_test_k2o DOUBLE NOT NULL CHECK (soil_test_k2o >= 0),
  soil_test_p2o5 DOUBLE NOT NULL CHECK (soil_test_p2o5 >= 0),
  soil_test_mgo DOUBLE NOT NULL CHECK (soil_test_cao  >= 0),
  soil_test_cao DOUBLE NOT NULL CHECK (soil_test_cao >= 0),
  soil_test_s DOUBLE NOT NULL CHECK (soil_test_s >= 0),
  soil_test_humus DOUBLE NOT NULL CHECK (soil_test_humus >= 0),
  soil_test_ph DOUBLE NOT NULL CHECK (soil_test_ph >= 0),
  soil_test_lon DOUBLE NOT NULL,
  soil_test_lat DOUBLE NOT NULL,
  soil_test_soil_type TEXT NOT NULL DEFAULT '',
  soil_test_other TEXT NOT NULL DEFAULT '',
  actn_soil_test_name TEXT NOT NULL DEFAULT 'Soil Sample'
);



COMMIT;














