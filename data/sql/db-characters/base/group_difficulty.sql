DROP TABLE IF EXISTS group_difficulty;

CREATE TABLE mp_group_difficulty (
    guid        INT UNSIGNED     NOT NULL DEFAULT '0',
    difficulty  TINYINT UNSIGNED NOT NULL DEFAULT '0',
    deaths      INT UNSIGNED     NOT NULL DEFAULT '0',
    PRIMARY KEY (guid)
);

CREATE TABLE mp_character_instance (
    guid        INT UNSIGNED     NOT NULL DEFAULT '0',
    current_difficulty  TINYINT UNSIGNED NOT NULL DEFAULT '1',
    deaths      INT UNSIGNED     DEFAULT '0',

    PRIMARY KEY (guid)
);

CREATE TABLE mp_character_dungeon_stats (
    guid        INT UNSIGNED     NOT NULL DEFAULT '0',
    dungeon_id  INT UNSIGNED     NOT NULL DEFAULT '0',
    difficulty  TINYINT UNSIGNED NOT NULL DEFAULT '0',
    deaths      INT UNSIGNED     DEFAULT '0',
    runs        INT UNSIGNED     DEFAULT '0',
    completions INT UNSIGNED  DEFAULT '0',
    failures    INT UNSIGNED  DEFAULT '0',
    total_time      INT UNSIGNED  DEFAULT '0',
    best_time    INT UNSIGNED  DEFAULT '0',
    PRIMARY KEY (guid, dungeon_id, difficulty)
);
