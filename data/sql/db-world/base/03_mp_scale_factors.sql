-- Last Update: 2025/01/17
-- Description: Scale factors for Mythic+ dungeons used to normalize dungeon difficulty across expansions.
-- 1. Pre 60 level dungeons (melee_bonus: 1.9-2.2, spell_bonus: 1.8-1.9, hp_bonus: 2, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (389, 2.2, 1.9, 1.9, 2, 3),  -- Ragefire Chasm
    (43,  1.9, 1.8, 1.8, 2, 3),  -- Wailing Caverns
    (36,  1.9, 1.9, 1.9, 2, 3),  -- The Deadmines
    (33,  1.9, 1.9, 1.9, 2, 3),  -- Shadowfang Keep
    (34,  1.9, 1.9, 1.9, 2, 3),  -- The Stockade
    (48,  1.9, 1.9, 1.9, 2, 3),  -- Blackfathom Deeps
    (90,  1.9, 1.9, 1.9, 2, 3),  -- Gnomeregan
    (47,  1.9, 1.9, 1.9, 2, 3),  -- Razorfen Kraul
    (189, 1.9, 1.9, 1.9, 2, 3),  -- Scarlet Monastery (Graveyard)
    (129, 1.9, 1.9, 1.9, 2, 3),  -- Razorfen Downs
    (70,  1.9, 1.9, 1.9, 2, 3),  -- Uldaman
    (209, 1.9, 1.9, 1.9, 2, 3),  -- Zul'Farrak
    (349, 1.9, 1.9, 1.9, 2, 3)   -- Maraudon
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 2. Level 60 dungeons for classic (melee_bonus: 1.7, spell_bonus: 2.0, hp_bonus: 3, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (289, 1.7, 2.0, 2.0, 3, 3),  -- Scholomance
    (109, 1.7, 2.0, 2.0, 3, 3),  -- Sunken Temple
    (329, 1.7, 2.0, 2.0, 3, 3),  -- Stratholme
    (229, 1.7, 2.0, 2.0, 3, 3),  -- Blackrock Spire (Lower)
    (230, 1.7, 2.0, 2.0, 3, 3),  -- Blackrock Spire (Upper)
    (429, 1.7, 2.0, 2.0, 3, 3),  -- Dire Maul
    (269, 1.7, 2.0, 2.0, 3, 3)   -- Temple of Atal'Hakkar
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 3. Pre 70 dungeons in Burning Crusade (melee_bonus: 1.6, spell_bonus: 1.4, hp_bonus: 4, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (542, 1.6, 1.4, 1.4, 4, 3),  -- Hellfire The Blood Furnace
    (543, 1.6, 1.4, 1.4, 4, 3),  -- Hellfire Ramparts
    (545, 1.6, 1.4, 1.4, 4, 3),  -- Coilfang Steamvaults
    (546, 1.6, 1.4, 1.4, 4, 3),  -- Coilfang Reservoir: The Underbog
    (547, 1.6, 1.4, 1.4, 4, 3),  -- Coilfang Reservoir: The Underbog
    (557, 1.6, 1.4, 1.4, 4, 3),  -- Auchindoun: Mana-Tombs
    (558, 1.6, 1.4, 1.4, 4, 3),  -- Auchindoun: Auchenai Crypts
    (560, 1.6, 1.4, 1.4, 4, 3)   -- Caverns of Time: Old Hillsbrad Foothills
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 4. Level 70 dungeons in Burning Crusade (melee_bonus: 1.4, spell_bonus: 1.3, hp_bonus: 4, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (540, 1.4, 1.3, 1.3, 4, 3),  -- Shattered Halls
    (556, 1.4, 1.3, 1.3, 4, 3),  -- Auchindoun: Sethekk Halls
    (555, 1.4, 1.3, 1.3, 4, 3),  -- Auchindoun: Shadow Labyrinth
    (553, 1.4, 1.3, 1.3, 4, 3),  -- Tempest Keep: The Botanica
    (554, 1.4, 1.3, 1.3, 4, 3),  -- Tempest Keep: The Mechanar
    (552, 1.4, 1.3, 1.3, 4, 3),  -- Tempest Keep: The Arcatraz
    (585, 1.4, 1.3, 1.3, 4, 3)   -- Magisters' Terrace
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 5. Pre 80 dungeons in Wrath of the Lich King (melee_bonus: 1.9, spell_bonus: 1.2, hp_bonus: 3, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (574, 1.9, 1.2, 1.2, 3, 3),  -- Utgarde Keep
    (619, 1.9, 1.2, 1.2, 3, 3),  -- Ahn'kahet: The Old Kingdom
    (576, 1.9, 1.2, 1.2, 3, 3),  -- The Nexus
    (600, 1.9, 1.2, 1.2, 3, 3),  -- Drak'Tharon Keep
    (601, 1.9, 1.2, 1.2, 3, 3),  -- Azjol-Nerub
    (608, 1.9, 1.2, 1.2, 3, 3)   -- The Violet Hold
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 6. Level 80 dungeons in Wrath of the Lich King (melee_bonus: 1.9, spell_bonus: 1.3, hp_bonus: 5, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (595, 1.9, 1.3, 1.3, 5, 3),  -- The Culling of Stratholme
    (604, 1.9, 1.3, 1.3, 5, 3),  -- Gundrak
    (599, 1.9, 1.3, 1.3, 5, 3),  -- Halls of Stone
    (602, 1.9, 1.3, 1.3, 5, 3),  -- Halls of Lightning
    (578, 1.9, 1.3, 1.3, 5, 3),  -- The Oculus
    (650, 1.9, 1.3, 1.3, 5, 3),  -- Trial of the Champion
    (632, 1.9, 1.3, 1.3, 5, 3),  -- The Forge of Souls
    (658, 1.9, 1.3, 1.3, 5, 3),  -- Pit of Saron
    (668, 1.9, 1.3, 1.3, 5, 3)   -- Halls of Reflection
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;

-- 7. Raid instances (melee_bonus: 2.6, spell_bonus: 2.0, heal_bonus: 1.6, hp_bonus: 3, difficulty: 3)
INSERT INTO mp_scale_factors (mapId, melee_bonus, spell_bonus, heal_bonus, hp_bonus, difficulty)
VALUES
    (169, 2.6, 2.0, 1.6, 3, 3),  -- Emerald Dream
    (249, 2.8, 1.6, 1.6, 3, 3),  -- Onyxia's Lair
    (309, 2.0, 1.3, 1.6, 3, 3),  -- Zul'Gurub
    (409, 2.2, 1.4, 1.6, 3, 3),  -- Molten Core
    (469, 2.6, 2.0, 1.6, 3, 3),  -- Blackwing Lair
    (509, 2.6, 2.0, 1.6, 3, 3),  -- Ruins of Ahn'Qiraj
    (531, 2.6, 2.0, 1.6, 3, 3),  -- Ahn'Qiraj Temple
    (532, 2.6, 2.0, 1.6, 3, 3),  -- Karazhan
    (533, 2.0, 1.8, 1.6, 3, 3),  -- Naxxramas
    (534, 2.6, 2.0, 1.6, 3, 3),  -- The Battle for Mount Hyjal
    (544, 2.0, 2.0, 1.6, 3, 3),  -- Magtheridon's Lair
    (548, 2.6, 2.0, 1.6, 3, 3),  -- Coilfang: Serpentshrine Cavern
    (550, 2.6, 2.0, 1.6, 3, 3),  -- Tempest Keep
    (564, 2.9, 2.8, 1.6, 3, 3),  -- Black Temple
    (565, 2.6, 2.0, 1.6, 3, 3),  -- Gruul's Lair
    (568, 2.6, 2.0, 1.6, 3, 3),  -- Zul'Aman
    (580, 2.6, 2.0, 1.6, 3, 3),  -- The Sunwell
    (603, 2.6, 2.0, 1.6, 3, 3),  -- Ulduar
    (615, 2.6, 2.0, 1.6, 3, 3),  -- The Obsidian Sanctum
    (616, 2.6, 2.0, 1.6, 3, 3),  -- The Eye of Eternity
    (624, 2.6, 2.0, 1.6, 3, 3),  -- Vault of Archavon
    (631, 2.8, 2.4, 1.6, 5, 3),  -- Icecrown Citadel
    (649, 3.0, 2.3, 1.6, 4, 3),  -- Trial of the Crusader
    (724, 2.68, 2.0, 1.6, 3, 3)   -- The Ruby Sanctum
AS new_values
ON DUPLICATE KEY UPDATE
    melee_bonus = new_values.melee_bonus,
    spell_bonus = new_values.spell_bonus,
    heal_bonus = new_values.heal_bonus,
    hp_bonus = new_values.hp_bonus,
    difficulty = new_values.difficulty;
