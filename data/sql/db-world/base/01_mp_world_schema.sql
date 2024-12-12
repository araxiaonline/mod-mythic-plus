-- Used to track upgrade ranks for stat improvements and min/max values
DROP TABLE IF EXISTS mp_stat_upgrade_ranks;
DROP TABLE IF EXISTS mp_upgrade_ranks;
CREATE TABLE mp_upgrade_ranks (
    upgradeRank     INT UNSIGNED NOT NULL,
    advancementId   INT UNSIGNED NOT NULL,
    materialId1     INT UNSIGNED NOT NULL,
    materialId2     INT UNSIGNED NOT NULL,
    materialId3     INT UNSIGNED NOT NULL,
    materialCost1   INT UNSIGNED NOT NULL,
    materialCost2   INT UNSIGNED NOT NULL,
    materialCost3   INT UNSIGNED NOT NULL,
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

-- Used to allocate trade materials based on slot upgrades
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
    dmg_bonus     INT            DEFAULT '100',
    spell_bonus   INT            DEFAULT '100',
    hp_bonus      INT            DEFAULT '100',
    difficulty    INT            DEFAULT '100',
    max           INT            DEFAULT '100',

    PRIMARY KEY (mapId)
);