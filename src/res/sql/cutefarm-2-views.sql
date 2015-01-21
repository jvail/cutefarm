
/*

+ v_seedV2

*/

BEGIN;

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
  GROUP BY pltr.pltr_id;
  
CREATE VIEW v_fild AS
  SELECT fild.fild_id AS fild_id, user_name, fild_name, group_concat(lot_nr, '/') AS lots, total(lot_x_fild_size) AS total, fild.year_of_harvest AS year
  FROM fild, lot, lot_x_fild, user
  WHERE fild.fild_id=lot_x_fild.fild_id
  AND lot.lot_id=lot_x_fild.lot_id
  AND fild.user_id=user.user_id
  GROUP BY fild.fild_id;

CREATE VIEW v_fild_info AS
  SELECT fild.fild_id, fild_name, fild_nr, fild_use_name,
  group_concat(DISTINCT lot_nr) AS lot_nrs, sum(lot_x_fild_size) AS sum
  FROM fild, fild_use, lot, lot_x_fild
  WHERE fild.fild_id=lot_x_fild.fild_id
  AND lot.lot_id=lot_x_fild.lot_id
  AND fild.fild_use_id=fild_use.fild_use_id
  GROUP BY fild.fild_id;
  
CREATE VIEW v_cultivar AS
  SELECT cultivar_id, crop_use_name || ' (' || cultivar_name || ')' AS name, seed_unit
  FROM cultivar, crop_use
  WHERE cultivar.crop_use_id=crop_use.crop_use_id;
  
CREATE VIEW v_crop_mix AS
  SELECT crop_mix_id, seed_name, group_concat(crop_use_name || ' (' || cultivar_name || ') ' || crop_mix_amt || '%', ', ') AS cultivars
  FROM crop_mix, seed, crop_use, cultivar
  WHERE crop_mix.seed_id=seed.seed_id
  AND crop_mix.cultivar_id=cultivar.cultivar_id
  AND cultivar.crop_use_id=crop_use.crop_use_id
  GROUP BY crop_mix.seed_id;
  
CREATE VIEW v_seed AS
  SELECT seed_id, name AS seed_name, seed_unit
  FROM seed, v_cultivar
  WHERE seed.cultivar_id=v_cultivar.cultivar_id
  AND seed_name IS NULL
  AND seed_is_actv = 'true'
  UNION
  SELECT seed_id, seed_name, 'kg/ha'
  FROM seed
  WHERE seed_is_actv = 'true'
  AND seed_name IS NOT NULL;
  
  CREATE VIEW v_seedV2 AS
  SELECT seed_id, name AS seed_name, seed_unit
  FROM seed, v_cultivar
  WHERE seed.cultivar_id=v_cultivar.cultivar_id
  AND seed_name IS NULL
  AND seed_is_actv = 'true';
  
CREATE VIEW v_actn AS
  SELECT actn_id, actn_date, work_name, oprt_name, year_of_harvest, actn_com
  FROM actn, work, oprt
  WHERE actn.work_id=work.work_id
  AND actn.oprt_id=oprt.oprt_id;
  
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
  AND actn_anim_grp.anim_grp_id=anim_grp.anim_grp_id
  UNION ALL
  SELECT actn_soil_test.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, 'Bodenprobe', NULL, NULL, fild_com
  FROM fild, actn_fild, actn_soil_test
  WHERE actn_soil_test.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id;

CREATE VIEW v_actn_fert AS
  SELECT actn_fert.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, fert_name AS input_name, actn_fert_amt AS input_amt, area * actn_fert_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, fert, actn_fert
  WHERE actn_fert.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_fert.fert_id=fert.fert_id;
  
CREATE VIEW v_actn_pltr AS
  SELECT actn_pltr.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, pltr_name AS input_name, actn_pltr_amt AS input_amt, area * actn_pltr_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_pltr, pltr
  WHERE actn_pltr.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_pltr.pltr_id=pltr.pltr_id;
  
CREATE VIEW v_actn_dril AS
  SELECT actn_dril.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, seed_name AS input_name, actn_dril_amt AS input_amt, area * actn_dril_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_dril, v_seed
  WHERE actn_dril.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_dril.seed_id=v_seed.seed_id;
  
CREATE VIEW v_actn_harv AS
  SELECT actn_harv.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, crop_use_name AS input_name, actn_harv_amt AS input_amt, area * actn_harv_amt  AS sum_amt, fild_com
  FROM fild, actn_fild, actn_harv, crop_use
  WHERE actn_harv.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_harv.crop_use_id=crop_use.crop_use_id;
  
CREATE VIEW v_actn_soil AS
  SELECT actn_soil.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_soil_name, fild_com
  FROM fild, actn_fild, actn_soil
  WHERE actn_soil.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id;
  
CREATE VIEW v_actn_medw AS
  SELECT actn_medw.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_medw_name, fild_com
  FROM fild, actn_fild, actn_medw
  WHERE actn_medw.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id;
  
CREATE VIEW v_actn_misc AS
  SELECT actn_misc.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, actn_misc_name, fild_com
  FROM fild, actn_fild, actn_misc
  WHERE actn_misc.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id;
  
CREATE VIEW v_actn_anim_grp AS
  SELECT actn_anim_grp.actn_id AS actn_id, actn_fild.fild_id AS fild_id, fild_name, area, anim_grp_name, actn_anim_grp_amt AS input_amt, fild_com
  FROM fild, actn_fild, actn_anim_grp, anim_grp
  WHERE actn_anim_grp.actn_id=actn_fild.actn_id
  AND actn_fild.fild_id=fild.fild_id
  AND actn_anim_grp.anim_grp_id=anim_grp.anim_grp_id;
  
CREATE VIEW v_nut_per_fild_id AS
    SELECT
    fild_id,
    fild_nr,
    total(n_kg_per_ha) AS n_kg_per_ha,
    total(k2o_kg_per_ha) AS k2o_kg_per_ha,
    total(p2o5_kg_per_ha) AS p2o5_kg_per_ha,
    total(s_kg_per_ha) AS s_kg_per_ha,
    total(mgo_kg_per_ha) AS mgo_kg_per_ha,
    total(cao_kg_per_ha) AS cao_kg_per_ha,
    total(humus_kg_per_ha) AS humus_kg_per_ha
    FROM
    --fert per fild_id
    (SELECT
    a.fild_id AS fild_id,
    fild_nr,
    total(n_kg_per_fild/fild_size) AS n_kg_per_ha,
    total(k2o_kg_per_fild/fild_size) AS k2o_kg_per_ha,
    total(p2o5_kg_per_fild/fild_size) AS p2o5_kg_per_ha,
    total(s_kg_per_fild/fild_size) AS s_kg_per_ha,
    total(mgo_kg_per_fild/fild_size) AS mgo_kg_per_ha,
    total(cao_kg_per_fild/fild_size) AS cao_kg_per_ha,
    total(humus_kg_per_fild/fild_size) AS humus_kg_per_ha
    FROM (
        SELECT
        fild_id,
        area*actn_fert_amt*fert_n AS n_kg_per_fild,
        area*actn_fert_amt*fert_k2o AS k2o_kg_per_fild,
        area*actn_fert_amt*fert_p2o5 AS p2o5_kg_per_fild,
        area*actn_fert_amt*fert_s AS s_kg_per_fild,
        area*actn_fert_amt*fert_mgo AS mgo_kg_per_fild,
        area*actn_fert_amt*fert_cao AS cao_kg_per_fild,
        area*actn_fert_amt*fert_withdrawal_humus AS humus_kg_per_fild
        FROM actn_fild, actn, actn_fert, fert
        WHERE actn_fild.actn_id=actn.actn_id
        AND actn_fert.actn_id=actn.actn_id
        AND actn_fert.fert_id=fert.fert_id
    ) AS a
    JOIN (
        SELECT fild.fild_id AS fild_id, fild_nr, total(lot_x_fild_size) AS fild_size FROM fild, lot_x_fild
        WHERE fild.fild_id=lot_x_fild.fild_id
        GROUP BY fild.fild_id
    ) AS b ON a.fild_id=b.fild_id
    GROUP BY a.fild_id
    UNION ALL
    --harv per fild_id
    SELECT
    a.fild_id AS fild_id,
    fild_nr,
    total(n_kg_per_fild/fild_size) AS n_kg_per_ha,
    total(k2o_kg_per_fild/fild_size) AS k2o_kg_per_ha,
    total(p2o5_kg_per_fild/fild_size) AS p2o5_kg_per_ha,
    total(s_kg_per_fild/fild_size) AS s_kg_per_ha,
    total(mgo_kg_per_fild/fild_size) AS mgo_kg_per_ha,
    total(cao_kg_per_fild/fild_size) AS cao_kg_per_ha,
    total(humus_kg_per_fild/fild_size) AS humus_kg_per_ha
    FROM (
        SELECT
        fild_id,
        area*actn_harv_amt*crop_use_withdrawal_n AS n_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_k2o AS k2o_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_p2o5 AS p2o5_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_s AS s_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_mgo AS mgo_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_cao AS cao_kg_per_fild,
        area*actn_harv_amt*crop_use_withdrawal_humus AS humus_kg_per_fild
        FROM actn_fild, actn, actn_harv, crop_use
        WHERE actn_fild.actn_id=actn.actn_id
        AND actn_harv.actn_id=actn.actn_id
        AND actn_harv.crop_use_id=crop_use.crop_use_id
    ) AS a
    JOIN (
        SELECT fild.fild_id AS fild_id, fild_nr, total(lot_x_fild_size) AS fild_size FROM fild, lot_x_fild
        WHERE fild.fild_id=lot_x_fild.fild_id
        GROUP BY fild.fild_id

    ) AS b ON a.fild_id=b.fild_id
    GROUP BY a.fild_id )
    GROUP BY fild_id;
  
  COMMIT;