-- This rebalances lower level iLevel gear
UPDATE item_template
SET
  stat_value1 = CASE
    WHEN stat_type1 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value1 * 0.75)
    WHEN stat_type1 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value1 * 0.80)
    WHEN stat_type1 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value1 * 0.90)
    ELSE stat_value1
  END,
  stat_value2 = CASE
    WHEN stat_type2 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value2 * 0.75)
    WHEN stat_type2 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value2 * 0.80)
    WHEN stat_type2 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value2 * 0.90)
    ELSE stat_value2
  END,
  stat_value3 = CASE
    WHEN stat_type3 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value3 * 0.75)
    WHEN stat_type3 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value3 * 0.80)
    WHEN stat_type3 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value3 * 0.90)
    ELSE stat_value3
  END,
  stat_value4 = CASE
    WHEN stat_type4 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value4 * 0.75)
    WHEN stat_type4 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value4 * 0.80)
    WHEN stat_type4 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value4 * 0.90)
    ELSE stat_value4
  END,
  stat_value5 = CASE
    WHEN stat_type5 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value5 * 0.75)
    WHEN stat_type5 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value5 * 0.80)
    WHEN stat_type5 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value5 * 0.90)
    ELSE stat_value5
  END,
  stat_value6 = CASE
    WHEN stat_type6 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value6 * 0.75)
    WHEN stat_type6 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value6 * 0.80)
    WHEN stat_type6 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value6 * 0.90)
    ELSE stat_value6
  END,
  stat_value7 = CASE
    WHEN stat_type7 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value7 * 0.75)
    WHEN stat_type7 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value7 * 0.80)
    WHEN stat_type7 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value7 * 0.90) ELSE stat_value7
  END,
  stat_value8 = CASE
    WHEN stat_type8 IN (38, 45) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value8 * 0.75)
    WHEN stat_type8 IN (38, 45) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value8 * 0.80)
    WHEN stat_type8 IN (38, 45) AND ItemLevel > 315 AND ItemLevel <= 325 THEN FLOOR(stat_value8 * 0.90)
    ELSE stat_value8
  END
WHERE
   entry BETWEEN 20000000 and 21000000;

-- Rebalance primary strength, agi, intellect down for lower ilvl items
UPDATE item_template
SET
  stat_value1 = CASE
    WHEN stat_type1 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value1 * 0.85)
    WHEN stat_type1 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value1 * 0.90)
    ELSE stat_value1
  END,
  stat_value2 = CASE
    WHEN stat_type2 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value2 * 0.85)
    WHEN stat_type2 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value2 * 0.90)
    ELSE stat_value2
  END,
  stat_value3 = CASE
    WHEN stat_type3 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value3 * 0.85)
    WHEN stat_type3 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value3 * 0.90)
    ELSE stat_value3
  END,
  stat_value4 = CASE
    WHEN stat_type4 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value4 * 0.85)
    WHEN stat_type4 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value4 * 0.90)
    ELSE stat_value4
  END,
  stat_value5 = CASE
    WHEN stat_type5 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value5 * 0.85)
    WHEN stat_type5 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value5 * 0.90)
    ELSE stat_value5
  END,
  stat_value6 = CASE
    WHEN stat_type6 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value6 * 0.85)
    WHEN stat_type6 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value6 * 0.90)
    ELSE stat_value6
  END,
  stat_value7 = CASE
    WHEN stat_type7 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value7 * 0.85)
    WHEN stat_type7 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value7 * 0.90)
    ELSE stat_value7
  END,
  stat_value8 = CASE
    WHEN stat_type8 IN (3,4,5) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value8 * 0.85)
    WHEN stat_type8 IN (3,4,5) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value8 * 0.90)
    ELSE stat_value8
  END
WHERE
   entry BETWEEN 20000000 and 21000000;

-- Nerf hit / spell hit crit rating and others items 300-315 by 15% and 10%
UPDATE item_template
SET
  stat_value1 = CASE
    WHEN (stat_type1 BETWEEN 16 AND 37 OR stat_type1 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value1 * 0.85)
    WHEN (stat_type1 BETWEEN 16 AND 37 OR stat_type1 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value1 * 0.90)
    ELSE stat_value1
  END,
  stat_value2 = CASE
    WHEN (stat_type2 BETWEEN 16 AND 37 OR stat_type2 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value2 * 0.85)
    WHEN (stat_type2 BETWEEN 16 AND 37 OR stat_type2 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value2 * 0.90)
    ELSE stat_value2
  END,
  stat_value3 = CASE
    WHEN (stat_type3 BETWEEN 16 AND 37 OR stat_type3 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value3 * 0.85)
    WHEN (stat_type3 BETWEEN 16 AND 37 OR stat_type3 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value3 * 0.90)
    ELSE stat_value3
  END,
  stat_value4 = CASE
    WHEN (stat_type4 BETWEEN 16 AND 37 OR stat_type4 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value4 * 0.85)
    WHEN (stat_type4 BETWEEN 16 AND 37 OR stat_type4 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value4 * 0.90)
    ELSE stat_value4
  END,
  stat_value5 = CASE
    WHEN (stat_type5 BETWEEN 16 AND 37 OR stat_type5 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value5 * 0.85)
    WHEN (stat_type5 BETWEEN 16 AND 37 OR stat_type5 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value5 * 0.90)
    ELSE stat_value5
  END,
  stat_value6 = CASE
    WHEN (stat_type6 BETWEEN 16 AND 37 OR stat_type6 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value6 * 0.85)
    WHEN (stat_type6 BETWEEN 16 AND 37 OR stat_type6 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value6 * 0.90)
    ELSE stat_value6
  END,
  stat_value7 = CASE
    WHEN (stat_type7 BETWEEN 16 AND 37 OR stat_type7 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value7 * 0.85)
    WHEN (stat_type7 BETWEEN 16 AND 37 OR stat_type7 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value7 * 0.90)
    ELSE stat_value7
  END,
  stat_value8 = CASE
    WHEN (stat_type8 BETWEEN 16 AND 37 OR stat_type8 = 44) AND ItemLevel BETWEEN 300 AND 305 THEN FLOOR(stat_value8 * 0.85)
    WHEN (stat_type8 BETWEEN 16 AND 37 OR stat_type8 = 44) AND ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(stat_value8 * 0.90)
    ELSE stat_value8
  END
WHERE
   entry BETWEEN 20000000 and 21000000;

-- reduce damage on lower level items
UPDATE item_template
SET
  dmg_min1 = CASE
    WHEN ItemLevel BETWEEN 300 AND 305 THEN FLOOR(dmg_min1 * 0.85)
    WHEN ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(dmg_min1 * 0.90)
    ELSE dmg_min1
  END,
  dmg_max1 = CASE
    WHEN ItemLevel BETWEEN 300 AND 305 THEN FLOOR(dmg_max1 * 0.85)
    WHEN ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(dmg_max1 * 0.90)
    ELSE dmg_max1
  END,
  dmg_min2 = CASE
    WHEN ItemLevel BETWEEN 300 AND 305 THEN FLOOR(dmg_min2 * 0.85)
    WHEN ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(dmg_min2 * 0.90)
    ELSE dmg_min2
  END,
  dmg_max2 = CASE
    WHEN ItemLevel BETWEEN 300 AND 305 THEN FLOOR(dmg_max2 * 0.85)
    WHEN ItemLevel > 305 AND ItemLevel <= 315 THEN FLOOR(dmg_max2 * 0.90)
    ELSE dmg_max2
  END
WHERE
   entry BETWEEN 20000000 and 21000000;


