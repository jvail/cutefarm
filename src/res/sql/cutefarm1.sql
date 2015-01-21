/*
    CuteFarm sqlite database update 1
    

*/



/*
    Check if db schema version 
    works with cutefarm version.
*/

PRAGMA user_version = 191#


/*
    plant treatment prices
*/

CREATE TABLE price_pltr (
  price_pltr_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  pltr_id INTEGER NOT NULL CONSTRAINT fk_pltr_id REFERENCES pltr (pltr_id) ON DELETE CASCADE ON UPDATE CASCADE,
  price_date TEXT NOT NULL,
  price DOUBLE NOT NULL DEFAULT 0.00 CHECK (price >= 0.00))#
  
CREATE UNIQUE INDEX index_price_pltr_pltr_id_date ON price_pltr ( pltr_id, price_date  )#

/*
    fertilizer prices
*/

CREATE TABLE price_fert (
  price_fert_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  fert_id INTEGER NOT NULL CONSTRAINT fk_fert_id REFERENCES fert (fert_id) ON DELETE CASCADE ON UPDATE CASCADE,
  price_date TEXT NOT NULL,
  price DOUBLE NOT NULL DEFAULT 0.00 CHECK (price >= 0.00))#
  
CREATE UNIQUE INDEX index_price_fert_fert_id_date ON price_fert ( fert_id, price_date  )#

/*
    seed prices
*/

CREATE TABLE price_seed (
  price_seed_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  seed_id INTEGER NOT NULL CONSTRAINT fk_seed_id REFERENCES seed (seed_id) ON DELETE CASCADE ON UPDATE CASCADE,
  price_date TEXT NOT NULL,
  price DOUBLE NOT NULL DEFAULT 0.00 CHECK (price >= 0.00))#
  
CREATE UNIQUE INDEX index_price_seed_seed_id_date ON price_seed ( seed_id, price_date  )#

/*
    diesel prices
*/

CREATE TABLE price_diesel (
  price_diesel_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  price_date TEXT UNIQUE NOT NULL,
  price DOUBLE NOT NULL DEFAULT 0.00 CHECK (price >= 0.00))#
  

/*
    operation productivity h/ha + diesel/ha
*/

ALTER TABLE oprt ADD COLUMN (productivity DOUBLE NOT NULL DEFAULT 0.00 CHECK (productivity >= 0.00)#
ALTER TABLE oprt ADD COLUMN (diesel_use DOUBLE NOT NULL DEFAULT 0.00 CHECK (diesel_use >= 0.00)#

/*
    worker price
*/

CREATE TABLE price_work (
  price_work_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
  work_id INTEGER NOT NULL CONSTRAINT fk_work_id REFERENCES work (work_id) ON DELETE CASCADE ON UPDATE CASCADE,
  price_date TEXT NOT NULL,
  price DOUBLE NOT NULL DEFAULT 0.00 CHECK (price >= 0.00))#
  
CREATE UNIQUE INDEX index_price_work_work_id_date ON price_work ( work_id, price_date  )#


/*
    nutrient withdrawals
*/

ALTER TABLE crop_use ADD COLUMN (withdrawal_n DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_k2o DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_p2o5 DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_s DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_mgo DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_cao DOUBLE NOT NULL DEFAULT 0.00)#
ALTER TABLE crop_use ADD COLUMN (withdrawal_humus DOUBLE NOT NULL DEFAULT 0.00)#


ALTER TABLE fert ADD COLUMN (withdrawal_humus DOUBLE NOT NULL DEFAULT 0.00)#











