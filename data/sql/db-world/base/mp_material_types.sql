-- MaterialID for all droppable cloth items
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
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
  AND clt.Entry IS NULL;

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
  OR name = 'Mageroyal'

ORDER BY it.RequiredSkillRank, it.name;

-- Rare Herbs that are harder to find or drop in raids
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    4 AS materialId,
    it.entry,
    it.name
FROM item_template it
WHERE it.class = 7
  AND it.subclass = 9
  AND it.VerifiedBuild = 12340
  AND it.RequiredSkillRank = 0
  OR name != 'Mageroyal'

ORDER BY it.RequiredSkillRank, it.name;

-- Common ores that are easy to find in the world
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
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
  AND it.name NOT IN ('Elementium Ore', 'Khorium Ore')
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
WHERE it.entry IN (
    33568, 4234, 4235, 2318, 783, 2319, 4232, 8170, 8171, 4304, 8169, 21887, 38557, 38561, 38558, 8167
)
  AND it.VerifiedBuild = 12340
ORDER BY it.name;

-- Rare skinning material found in the world or dropped by enemies
REPLACE INTO mp_material_types (materialId, entry, name)
SELECT
    8 AS materialId, -- Unique ID for rare skinnable and non-skinnable materials
    it.entry,
    it.name
FROM item_template it
WHERE it.entry IN (
    44128, 15416, 15415, 15412, 15414, 15410, 29548, 29547, 25708, 29539, 15417, 7392, 7286,
    15408, 20498, 20500, 20501, 8165, 6470, 6471, 17012, 20381, 19767, 19768, 15419
)
  AND it.VerifiedBuild = 12340
ORDER BY it.name;

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
  )
ORDER BY it.name;

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
        "King\'s Amber",
        'Eye of Zul',
        'Majestic Zircon',
        'Dreadstone'
  )
ORDER BY it.name;
