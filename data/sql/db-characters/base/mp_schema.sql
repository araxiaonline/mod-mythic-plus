
CREATE TABLE IF NOT EXISTS mp_character_instance (
    character_guid        INT UNSIGNED     NOT NULL DEFAULT '0',
    current_difficulty  TINYINT UNSIGNED NOT NULL DEFAULT '1',
    deaths      INT UNSIGNED     DEFAULT '0',

    PRIMARY KEY (guid)
);

CREATE TABLE IF NOT EXISTS mp_character_dungeon_stats (
    character_guid        INT UNSIGNED     NOT NULL DEFAULT '0',
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
