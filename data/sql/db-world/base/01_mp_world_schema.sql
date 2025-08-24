-- Used to track upgrade ranks for stat improvements and min/max values
DROP TABLE IF EXISTS mp_stat_upgrade_ranks;
DROP TABLE IF EXISTS mp_upgrade_ranks;
CREATE TABLE mp_upgrade_ranks (
    upgradeRank     INT UNSIGNED NOT NULL,
    advancementId   INT UNSIGNED NOT NULL,
    itemEntry1      INT UNSIGNED NOT NULL,
    itemEntry2      INT UNSIGNED NOT NULL,
    itemEntry3      INT UNSIGNED NOT NULL,
    itemCost1       INT UNSIGNED NOT NULL,
    itemCost2       INT UNSIGNED NOT NULL,
    itemCost3       INT UNSIGNED NOT NULL,
    minIncrease1    INT UNSIGNED NOT NULL,
    maxIncrease1    INT UNSIGNED NOT NULL,
    minIncrease2    INT UNSIGNED NOT NULL,
    maxIncrease2    INT UNSIGNED NOT NULL,
    minIncrease3    INT UNSIGNED NOT NULL,
    maxIncrease3    INT UNSIGNED NOT NULL,
    chanceCost1     INT UNSIGNED NOT NULL,
    chanceCost2     INT UNSIGNED NOT NULL,
    chanceCost3     INT UNSIGNED NOT NULL,

    PRIMARY KEY (upgradeRank, advancementId)
);

-- Used to allocate trade materials to a category for fusion into new tradeskills
DROP TABLE IF EXISTS mp_material_types;
CREATE TABLE mp_material_types (
    materialId  INT UNSIGNED NOT NULL,
    entry       INT UNSIGNED NOT NULL,
    name        VARCHAR(255) NOT NULL,

    PRIMARY KEY (materialId, entry)
);


-- Description: Scale factors for Mythic+ dungeons used to normalize dungeon difficulty across expansions.
DROP TABLE IF EXISTS mp_scale_factors;
CREATE TABLE mp_scale_factors (
    mapId         SMALLINT       NOT NULL,
    melee_bonus   FLOAT          DEFAULT '1',
    spell_bonus   FLOAT          DEFAULT '1',
    heal_bonus    FLOAT          DEFAULT '1',
    hp_bonus      FLOAT          DEFAULT '1',
    difficulty    FLOAT          DEFAULT '1',
    PRIMARY KEY (mapId)
);
