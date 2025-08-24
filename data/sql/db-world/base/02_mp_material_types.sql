TRUNCATE TABLE mp_material_types;

-- MaterialID for all droppable cloth items
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT DISTINCT
    1 AS materialId, -- Assign the same materialId to all rows
    it.entry,
    it.name
FROM item_template it
         LEFT JOIN creature_loot_template clt ON (it.entry = clt.Item)
WHERE it.name LIKE '%Cloth'
  AND it.class = 7
  AND it.subclass = 5
  AND it.name NOT LIKE 'Bolt%'
  AND it.VerifiedBuild = 12340
  AND clt.Entry IS NOT NULL;

-- Crafted cloth items that are not dropped by enemies
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    2 AS materialId, -- Assign the same materialId to all rows
    it.entry,
    it.name
FROM item_template it
         LEFT JOIN creature_loot_template clt ON (it.entry = clt.Item)
WHERE it.name LIKE '%Cloth'
  AND it.class = 7
  AND it.subclass = 5
  AND it.name NOT LIKE 'Bolt%'
  AND it.VerifiedBuild = 12340
  AND clt.Entry IS NULL;

-- Common Herbs available to herbalists
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    3 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE it.class = 7
  AND it.subclass = 9
  AND it.VerifiedBuild = 12340
  AND it.RequiredSkillRank != 0
  AND it.name NOT IN (
    'Black Lotus',
    'Ghost Mushroom',
    'Dreamfoil',
    'Bloodvine',
    'Fel Lotus',
    'Netherbloom',
    'Nightmare Vine',
    'Frost Lotus',
    "Adder's Tongue",
    'Fire Leaf',
    'Lichbloom',
    'Icethorn',
    "Talandra's Rose"
);
-- Rare Herbs that are harder to find or drop in raids
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    4 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE it.name IN (
    'Black Lotus',          -- Classic: Rare spawn in Dire Maul and outdoor zones, extremely rare in raids.
    'Ghost Mushroom',       -- Classic: Found in Maraudon (Dungeon-only herb).
    'Bloodvine',            -- Classic: Drops from enemies in Zul'Gurub.
    'Fel Lotus',            -- TBC: Extremely rare herb drop while gathering Felweed, Dreaming Glory, etc., in Outland.
    'Netherbloom',          -- TBC: Rare spawn in Netherstorm, also dropped by some elites.
    'Nightmare Vine',       -- TBC: Found in Shadowmoon Valley and drops in Black Temple from specific mobs.
    'Frost Lotus',          -- Wrath: Extremely rare spawn from herb nodes or dungeons like Utgarde Pinnacle.
    "Adder's Tongue",       -- Wrath: Rare spawn from dungeon nodes in Drak'Tharon Keep and Gundrak.
    'Fire Leaf',            -- Wrath: Rare herb found in Wintergrasp, sometimes in dungeon PvP areas.
    'Icethorn',             -- Wrath: Harvested from specific dungeon nodes in higher-level Northrend zones.
    "Talandra's Rose"       -- Wrath: Found sparsely in Zul'Drak dungeons like Gundrak.
);

-- Common ores that are easy to find in the world
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT DISTINCT
    5 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE it.class = 7
  AND it.subclass = 7
  AND it.VerifiedBuild = 12340
  AND it.name NOT LIKE '%Bar'
  AND it.name NOT LIKE '%Ingot'
  AND it.name NOT LIKE '%Stone'
  AND it.name NOT LIKE '%Shard'
  AND it.name NOT IN ('Coal', 'Elemental Flux', 'Hardened Khorium')
  AND it.name NOT IN ('Elementium Ore', 'Dark Iron Ore', 'Primal Nether', 'Runed Orb', 'Crusader Orb', 'Blood of the Mountain')
ORDER BY it.name;

-- Rare ores that are harder to find or drop in raids
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT DISTINCT
    6 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name Like '%Obsidian Shard%'
    OR it.name IN ('Elementium Ore', 'Dark Iron Ore', 'Primal Nether', 'Runed Orb', 'Crusader Orb', 'Blood of the Mountain')
    and entry <= 2000000
ORDER BY it.name;

-- Common skins that are easy to find in the world
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    7 AS materialId, -- Unique ID for commonly skinnable materials
    it.entry,
    it.name
FROM item_template it
WHERE it.name IN (
    'Borean Leather',
    'Heavy Leather',
    'Heavy Hide',
    'Light Leather',
    'Light Hide',
    'Medium Leather',
    'Medium Hide',
    'Rugged Leather',
    'Rugged Hide',
    'Thick Leather',
    'Thick Hide',
    'Knothide Leather',
    'Icy Dragonscale',
    'Jormungar Scale',
    'Nerubian Chitin',
    'Turtle Scale'
);
-- Rare skinning material found in the world or dropped by enemies
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    8 AS materialId, -- Unique ID for rare skinnable and non-skinnable materials
    it.entry,
    it.name
FROM item_template it
WHERE it.name IN (
    'Black Dragonscale',
    'Blue Dragonscale',
    'Green Dragonscale',
    'Red Dragonscale',
    'Scale of Onyxia',
    'Nether Dragonscales',
    'Wind Scales',
    'Thick Clefthoof Leather',
    'Cobra Scales',
    'Devilsaur Leather',
    'Green Whelp Scale',
    'Black Whelp Scale',
    'Perfect Deviate Scale',
    'Core Leather',
    'Dreamscale',
    'Primal Bat Leather',
    'Primal Tiger Leather'
);

-- Common uncut gems that are easy to find in the world
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    9 AS materialId, -- Unique ID for common uncut gems
    it.entry,
    it.name
FROM item_template it
WHERE
  it.VerifiedBuild = 12340   -- Wrath of the Lich King build
  AND it.name NOT LIKE '%Cut%'   -- Exclude pre-cut gems
  AND it.name IN (
      'Malachite',
      'Tigerseye',
      'Moss Agate',
      'Shadowgem',
      'Jade',
      'Lesser Moonstone',
      'Citrine',
      'Aquamarine',
      'Star Ruby',
      'Azerothian Diamond',
      'Huge Emerald',
      'Large Opal',
      'Blue Sapphire',
      'Arcane Crystal',
      'Bloodstone',
      'Sun Crystal',
      'Chalcedony',
      'Dark Jade',
      'Shadow Crystal',
      'Huge Citrine',
      'Monarch Topaz',
      'Forest Emerald',
      'Sky Sapphire',
      "Autumn's Glow",
      'Twilight Opal',
      'Scarlet Ruby'
  );

-- Rare gems that are harder to find or drop in raids
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    10 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
   it.name IN (
        -- TBC Epic Gems
        'Crimson Spinel',
        'Empyrean Sapphire',
        'Lionseye',
        'Shadowsong Amethyst',
        'Pyrestone',
        'Seaspray Emerald',

        -- WotLK Epic Gems
        'Cardinal Ruby',
        'Ametrine',
        "King's Amber",
        'Eye of Zul',
        'Majestic Zircon',
        'Dreadstone'
  );

-- common enchantment materials that are easy to find in the world
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    11 AS materialId,  -- Example materialId for enchantment items
    it.entry,
    it.name
FROM item_template it
WHERE
    it.class = 7
    AND it.subclass = 12
    AND it.VerifiedBuild = 12340
    AND it.name NOT LIKE '%Rod%'
    AND it.name NOT IN (
        -- Classic Rare Enchantment Items
        'Righteous Orb',                 -- Drops in Stratholme.
        'Lava Core',                     -- Rare drop in Molten Core.
        'Black Diamond',                 -- Rare drop in Blackrock Depths and related areas.
        'Essence of the Red',            -- Reward from Vaelastrasz in Blackwing Lair.
        'Onyxia Scale',                  -- Dropped by Onyxia, used in fire resistance recipes.

        -- TBC Rare Enchantment Items
        'Primal Might',                  -- Crafted using all Primals, very rare due to material costs.
        'Heart of Darkness',             -- Rare drop in Black Temple.
        'Sunmote',                       -- Drops in Sunwell Plateau.
        'Nether Vortex',                 -- Raid-exclusive crafting material in TBC.

        -- WotLK Rare Enchantment Items
        'Eternal Might',                 -- Custom material replacing Eternal combinations, rare in high-level content.
        'Frozen Orb',                    -- Drops in WotLK heroic dungeons.
        'Runed Orb',                     -- Drops in Ulduar.
        'Crusader Orb'                   -- Drops in Trial of the Crusader raid.
    );

-- Rare enchantment materials that are harder to find or drop in raids
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    12 AS materialId,  -- Example materialId for enchantment items
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic Rare Enchantment Items
        'Righteous Orb',                 -- Drops in Stratholme.
        'Lava Core',                     -- Rare drop in Molten Core.
        'Black Diamond',                 -- Rare drop in Blackrock Depths and related areas.
        'Essence of the Red',            -- Reward from Vaelastrasz in Blackwing Lair.
        'Onyxia Scale',                  -- Dropped by Onyxia, used in fire resistance recipes.

        -- TBC Rare Enchantment Items
        'Primal Might',                  -- Crafted using all Primals, very rare due to material costs.
        'Heart of Darkness',             -- Rare drop in Black Temple.
        'Sunmote',                       -- Drops in Sunwell Plateau.
        'Nether Vortex',                 -- Raid-exclusive crafting material in TBC.

        -- WotLK Rare Enchantment Items
        'Eternal Might',                 -- Custom material replacing Eternal combinations, rare in high-level content.
        'Frozen Orb',                    -- Drops in WotLK heroic dungeons.
        'Runed Orb',                     -- Drops in Ulduar.
        'Crusader Orb'                   -- Drops in Trial of the Crusader raid.
    );

-- Common materials that related to water / frost
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    13 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Elemental Water',
        'Essence of Water',
        'Glacial Fragments',

        -- TBC
        'Arctic Fur',
        'Thick Dawnstone',

        -- WotLK
        'Frost Lotus',
        'Frostweave Cloth'
    );

-- Rare materials that related to water / frost
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    14 AS materialId, -- Unique ID for rare frost resistance materials
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Frozen Rune',
        'Core of Earth',

        -- TBC
        'Frost-Infused Shard',
        'Nether Residuum',
        'Primal Water',

        -- WotLK
        'Primordial Saronite',
        'Icy Dragonscale',
        'Eternal Water'
    );

-- Common materials that relate to fire
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    15 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Elemental Fire',
        'Essence of Fire',
        'Heart of Fire',
        'Small Flame Sac',
        'Core of Flame',

        -- TBC
        'Mote of Fire',
        'Flame Spessarite',             -- Common fire-themed gem for jewelcrafting.
        'Smoldering Core',              -- Fire-themed drop from elementals.

        -- WotLK
        'Fire Leaf',
        'Crystallized Fire'
    );

-- Rare materials that relate to fire resistance gear or fire-themed weapons
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    16 AS materialId, -- Unique ID for rare fire resistance and fire weapon crafting materials
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Fiery Core',                 -- Key material for crafting fire resistance gear in Molten Core.
        'Lava Core',                  -- Used in crafting fire resistance gear and recipes.
        'Sulfuron Ingot',             -- Rare drop from Molten Core, used for crafting legendary fire weapons.

        -- TBC
        'Primal Fire',                -- Core material for fire crafting in TBC.
        'Hardened Adamantite Bar',    -- Material for crafting fire-resistant shields and gear.

        -- WotLK
        'Primordial Saronite',        -- Used for crafting high-level resistance and utility gear.
        'Eternal Fire',               -- Central material for fire-themed crafting.
        'Runed Orb',                  -- Used in crafting fire-related weapons and gear.
        'Smouldering Crystals',       -- High-value fire crafting material from fire-themed mobs.
        'Burning Embers'              -- Rare crafting material for WotLK fire-themed items.
    );

-- Common materials that relate to nature
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    17 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Elemental Earth',
        'Essence of Earth',
        'Living Essence',
        'Small Venom Sac',
        "Un'Goro Soil",
        'Ichor of Undeath',              -- Used in nature-themed recipes.

        -- TBC
        'Mote of Earth',                 -- Combines into Primal Earth.
        'Mote of Life',                  -- Combines into Primal Life.
        'Terocone',                      -- Nature-themed herb used in alchemy.

        -- WotLK
        'Crystallized Earth',            -- Combines into Eternal Earth.
        'Crystallized Life',             -- Combines into Eternal Life.
        'Icethorn',                      -- Herb with thematic ties to nature resistance potions.
        'Ancient Moss'
    );

-- Rare Nature resistance materials
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    18 AS materialId, -- Unique ID for rare nature resistance and nature weapon crafting materials
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Heart of the Wild',             -- Rare material for crafting nature resistance gear.
        'Living Essence',                -- Rare crafting material dropped by nature elementals.
        "Gahz'rilla's Electrified Scale",-- Rare item from Zul'Farrak for nature resistance gear.

        -- TBC
        'Primal Earth',                  -- Central crafting material for resistance and nature-themed items.
        'Primal Life',                   -- Rare material for nature-themed crafting.

        -- WotLK
        'Eternal Earth',                 -- Central material for nature-themed crafting.
        'Eternal Life',                  -- Rare material for nature-themed crafting.
        'Runed Orb',                     -- Rare crafting material for nature-themed gear.
        "Adder's Tongue"                -- Herb used in nature resistance recipes.
    );


-- Common materials that relate to shadow
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    19 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Shadowgem',                    -- Common low-level gem with shadow theme.
        'Elemental Shadow',             -- Basic crafting reagent from shadow elementals.
        'Essence of Shadow',            -- Dropped by shadow-themed elementals in high-level zones.

        -- TBC
        'Mote of Shadow',               -- Combines into Primal Shadow, used in crafting.
        'Dark Rune',                    -- Dropped in Scholomance and used for shadow resistance crafting.

        -- WotLK
        'Crystallized Shadow',          -- Combines into Eternal Shadow.
        'Deadnettle',                    -- Herb with thematic ties to shadow crafting.
        'Shadowy Essence'               -- Rare Northrend material for shadow crafting.
    );

-- Rare materials that relate to shadow resistance gear or shadow-themed weapons
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    20 AS materialId, -- Unique ID for rare shadow resistance and shadow weapon crafting materials
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Heart of Darkness',            -- Rare drop used for shadow resistance gear in TBC and Classic.
        'Shadow Oil',                   -- Crafted alchemical reagent for shadow resistance.
        'Black Lotus',                  -- Rare herb used in high-level shadow potions.

        -- TBC
        'Primal Shadow',                -- Central crafting material for shadow resistance.
        'Nightmare Seed',               -- Rare herb used in shadow resistance recipes.
        'Felcloth',                     -- Cloth with shadow resistance used in tailoring.


        -- WotLK
        'Eternal Shadow',               -- Core material for shadow resistance crafting.
        'Runed Orb'                    -- Rare material for shadow-themed gear and weapons.
    );

-- Common materials that relate to arcane resistance gear or arcane-themed weapons
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    21 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Arcane Crystal',              -- Rare mining material used in Arcanite Bars.
        'Essence of Magic',            -- Dropped by arcane elementals.
        'Lesser Magic Essence',        -- Early enchanting material.
        'Greater Magic Essence',       -- Enchanting material with arcane ties.

        -- TBC
        'Mote of Mana',                -- Combines into Primal Mana.

        -- WotLK
        'Crystallized Mana'           -- Combines into Eternal Mana.
    );


-- Rare materials that relate to arcane resistance gear or arcane-themed weapons
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    22 AS materialId, -- Unique ID for rare arcane resistance and arcane weapon crafting materials
    it.entry,
    it.name
FROM item_template it
WHERE
    it.name IN (
        -- Classic
        'Golden Pearl',               -- Rare material used in crafting arcane resistance gear.
        'Arcanite Bar',               -- Transmuted from Arcane Crystals, used in crafting high-end gear.

        -- TBC
        'Primal Mana',                -- Essential for arcane resistance crafting.
        'Nether Vortex',              -- Raid-exclusive crafting material.
        'Void Sphere',                -- Rare arcane-themed gem obtained through badge purchases or raids.

        -- WotLK
        'Eternal Mana',               -- Central crafting material for arcane resistance gear.
        'Saronite Animus'            -- Rare drop from arcane-themed enemies in Icecrown Citadel.
    );

REPLACE INTO mp_material_types (materialId, entry, name) VALUES (23, 911002, 'Veilstone');
