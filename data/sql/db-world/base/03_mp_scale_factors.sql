-- Last Update: 2021/08/15
-- Description: Scale factors for Mythic+ dungeons used to normalize dungeon difficulty across expansions.
-- 1. Pre 60 level dungeons (13 dmg_bonus, 2 hp_bonus, max 23, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (389, 22, 19,2, 3, 23),  -- Ragefire Chasm
    (43,  19, 18,2, 3, 23),  -- Wailing Caverns
    (36,  19, 19,2, 3, 23),  -- The Deadmines
    (33,  19, 19,2, 3, 23),  -- Shadowfang Keep
    (34,  19, 19,2, 3, 23),  -- The Stockade
    (48,  19, 19,2, 3, 23),  -- Blackfathom Deeps
    (90,  19, 19,2, 3, 23),  -- Gnomeregan
    (47,  19, 19,2, 3, 23),  -- Razorfen Kraul
    (189, 19, 19,2, 3, 23),  -- Scarlet Monastery (Graveyard)
    (129, 19, 19,2, 3, 23),  -- Razorfen Downs
    (70,  19, 19,2, 3, 23),  -- Uldaman
    (209, 19, 19,2, 3, 23),  -- Zul'Farrak
    (349, 19, 19,2, 3, 23)   -- Maraudon
ON DUPLICATE KEY UPDATE mapId = mapId;

-- 2. Level 60 dungeons for classic (15 dmg_bonus, 3 hp_bonus, max 25, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (289, 17, 20,3, 3, 25),  -- Scholomance
    (109, 17, 20,3, 3, 25),  -- Sunken Temple
    (329, 17, 20,3, 3, 25),  -- Stratholme
    (229, 17, 20,3, 3, 25),  -- Blackrock Spire (Lower)
    (230, 17, 20,3, 3, 25),  -- Blackrock Spire (Upper)
    (429, 17, 20,3, 3, 25),  -- Dire Maul
    (269, 17, 20,3, 3, 25)   -- Temple of Atal'Hakkar
ON DUPLICATE KEY UPDATE mapId = mapId;

-- 3. Pre 70 dungeons in Burning Crusade (15 dmg_bonus, 4 hp_bonus, max 26, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (542, 16, 14,4, 3, 26),  -- Hellfire The Blood Furnace
    (543, 16, 14,4, 3, 26),  -- Hellfire Ramparts
    (545, 16, 14,4, 3, 26),  -- Coilfang Steamvaults
    (546, 16, 14,4, 3, 26),  -- Coilfang Reservoir: The Underbog
    (547, 16, 14,4, 3, 26),  -- Coilfang Reservoir: The Underbog
    (557, 16, 14,4, 3, 26),  -- Auchindoun: Mana-Tombs
    (558, 16, 14,4, 3, 26),  -- Auchindoun: Auchenai Crypts
    (560, 16, 14,4, 3, 26)   -- Caverns of Time: Old Hillsbrad Foothills
ON DUPLICATE KEY UPDATE mapId = mapId;

-- 4. Level 70 dungeons in Burning Crusade (14 dmg_bonus, 4 hp_bonus, max 29, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (540, 14, 13,4, 3, 29),  -- Shattered Halls
    (556, 14, 13,4, 3, 29),  -- Auchindoun: Sethekk Halls
    (555, 14, 13,4, 3, 29),  -- Auchindoun: Shadow Labyrinth
    (553, 14, 13,4, 3, 29),  -- Tempest Keep: The Botanica
    (554, 14, 13,4, 3, 29),  -- Tempest Keep: The Mechanar
    (552, 14, 13,4, 3, 29),  -- Tempest Keep: The Arcatraz
    (585, 14, 13,4, 3, 29)   -- Magisters' Terrace
ON DUPLICATE KEY UPDATE mapId = mapId;

-- 5. Pre 80 dungeons in Wrath of the Lich King (17 dmg_bonus, 3 hp_bonus, max 30, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (574, 19, 12,3, 3, 30),  -- Utgarde Keep
    (619, 19, 12,3, 3, 30),  -- Ahn'kahet: The Old Kingdom
    (576, 19, 12,3, 3, 30),  -- The Nexus
    (600, 19, 12,3, 3, 30),  -- Drak'Tharon Keep
    (601, 19, 12,3, 3, 30),  -- Azjol-Nerub
    (608, 19, 12,3, 3, 30)   -- The Violet Hold
ON DUPLICATE KEY UPDATE mapId = mapId;

-- 6. Level 80 dungeons in Wrath of the Lich King (19 dmg_bonus, 4 hp_bonus, max 33, difficulty 3)
INSERT INTO mp_scale_factors (mapId, dmg_bonus, spell_bonus, hp_bonus, difficulty, max)
VALUES
    (595, 19, 13,5, 3, 33),  -- The Culling of Stratholme
    (604, 19, 13,5, 3, 33),  -- Gundrak
    (599, 19, 13,5, 3, 33),  -- Halls of Stone
    (602, 19, 13,5, 3, 33),  -- Halls of Lightning
    (578, 19, 13,5, 3, 33),  -- The Oculus
    (650, 19, 13,5, 3, 33),  -- Trial of the Champion
    (632, 19, 13,5, 3, 33),  -- The Forge of Souls
    (658, 19, 13,5, 3, 33),  -- Pit of Saron
    (668, 19, 13,5, 3, 33)   -- Halls of Reflection
ON DUPLICATE KEY UPDATE mapId = mapId;
