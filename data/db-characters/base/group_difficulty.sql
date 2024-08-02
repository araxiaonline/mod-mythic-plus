DROP TABLE IF EXISTS group_difficulty;

CREATE TABLE group_difficulty (
    guid        INT UNSIGNED     NOT NULL DEFAULT '0',
    difficulty  TINYINT UNSIGNED NOT NULL DEFAULT '0',
    PRIMARY KEY (guid)
);
