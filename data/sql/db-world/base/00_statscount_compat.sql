-- mod-mythic-plus item SQL (09_custom_items, 14_emblem_gear, 16_molten_core_mythic_gear) declares an
-- `item_template`.`StatsCount` column. AzerothCore has no such column: the core *computes* the stat count
-- at load time from stat_type*/stat_value* (see ObjectMgr::LoadItemTemplates), and the base world schema
-- never defines it. On a snapshot-seeded realm the item files are pre-recorded in `updates` and skipped, so
-- the mismatch is masked; on a CLEAN build they apply fresh and fail with
-- `ERROR 1054 Unknown column 'StatsCount'`. This migration ensures the column exists (inert — the core
-- ignores it) so the module's item inserts apply unmodified. Sorts before 09/14/16 in the base/ apply order.
-- The AC updater applies each module file once (by hash), so a plain ALTER is safe here.
ALTER TABLE `item_template` ADD COLUMN `StatsCount` TINYINT UNSIGNED NOT NULL DEFAULT 0;
