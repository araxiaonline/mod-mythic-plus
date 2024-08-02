/**
 * This creates the scaled up stats based on polynomial regression of the base stat growth
 */
CREATE TABLE IF NOT EXISTS `mythicplus_classlevelstats` (
  `level` tinyint unsigned NOT NULL,
  `class` tinyint unsigned NOT NULL,
  `basehp0` int unsigned NOT NULL DEFAULT '1',
  `basehp1` int unsigned NOT NULL DEFAULT '1',
  `basehp2` int unsigned NOT NULL DEFAULT '1',
  `basemana` int unsigned NOT NULL DEFAULT '0',
  `basearmor` int unsigned NOT NULL DEFAULT '1',
  `attackpower` int unsigned NOT NULL DEFAULT '0',
  `rangedattackpower` int unsigned NOT NULL DEFAULT '0',
  `damage_base` float NOT NULL DEFAULT '0',
  `damage_exp1` float NOT NULL DEFAULT '0',
  `damage_exp2` float NOT NULL DEFAULT '0',
  `comment` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci,
  PRIMARY KEY (`level`,`class`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci
;

INSERT IGNORE INTO acore_world.mythicplus_classlevelstats (class, `level`, basehp0, basehp1, basehp2,
basemana, basearmor, attackpower, rangedattackpower, damage_base, damage_exp1, damage_exp2) VALUES
(1, 81, 5489.19, 9449.55, 12640.32, 0.00, 10033.36, 714.66, 111.97, 47.9043, 131.5509, 166.3098),
(1, 82, 5641.53, 9700.79, 12855.00, 0.00, 10337.65, 790.54, 121.09, 48.5708, 131.9685, 169.2029),
(1, 83, 5797.46, 9956.22, 13042.96, 0.00, 10642.47, 872.71, 130.74, 49.2374, 132.0348, 171.8720),
(1, 84, 5956.98, 10215.83, 13204.22, 0.00, 10947.81, 961.17, 140.89, 49.9039, 131.7497, 174.3170),
(1, 85, 6120.07, 10479.62, 13338.76, 0.00, 11253.67, 1055.90, 151.56, 50.5705, 131.1133, 176.5379),
(1, 86, 6286.76, 10747.60, 13446.59, 0.00, 11560.05, 1156.93, 162.75, 51.2370, 130.1256, 178.5348),
(1, 87, 6457.02, 11019.76, 13527.71, 0.00, 11866.95, 1264.23, 174.45, 51.9035, 128.7865, 180.3077),
(1, 88, 6630.87, 11296.10, 13582.12, 0.00, 12174.38, 1377.82, 186.66, 52.5700, 127.0960, 181.8565),
(1, 89, 6808.31, 11576.62, 13609.82, 0.00, 12482.32, 1497.69, 199.39, 53.2365, 125.0542, 183.1813),
(1, 90, 6989.33, 11861.33, 13610.80, 0.00, 12790.79, 1623.85, 212.64, 53.9030, 122.6611, 184.2821),
(1, 91, 7173.93, 12150.22, 13585.08, 0.00, 13099.78, 1756.29, 226.40, 54.5695, 119.9166, 185.1587),
(1, 92, 7362.12, 12443.29, 13532.64, 0.00, 13409.29, 1895.01, 240.67, 55.2360, 116.8207, 185.8114),
(1, 93, 7553.90, 12740.55, 13453.49, 0.00, 13719.32, 2040.02, 255.46, 55.9024, 113.3736, 186.2400),
(1, 94, 7749.25, 13041.99, 13347.63, 0.00, 14029.88, 2191.31, 270.76, 56.5689, 109.5750, 186.4446),
(1, 95, 7948.20, 13347.61, 13215.06, 0.00, 14340.95, 2348.88, 286.57, 57.2353, 105.4251, 186.4251),
(1, 96, 8150.72, 13657.41, 13055.78, 0.00, 14652.55, 2512.74, 302.90, 57.9018, 100.9239, 186.1815),
(1, 97, 8356.83, 13971.40, 12869.79, 0.00, 14964.67, 2682.88, 319.75, 58.5682, 96.0713, 185.7140),
(1, 98, 8566.53, 14289.57, 12657.08, 0.00, 15277.30, 2859.31, 337.11, 59.2346, 90.8674, 185.0224),
(1, 99, 8779.81, 14611.92, 12417.67, 0.00, 15590.47, 3042.02, 354.98, 59.9010, 85.3122, 184.1067),
(1, 100, 8996.67, 14938.46, 12151.54, 0.00, 15904.15, 3231.01, 373.37, 60.5674, 79.4055, 182.9670),
(2, 81, 4391.71, 7560.03, 12640.32, 4078.27, 10008.59, 677.15, 87.20, 44.8582, 112.1715, 166.3095),
(2, 82, 4513.65, 7760.95, 12855.00, 4169.46, 10311.04, 749.46, 94.48, 45.5146, 112.7114, 169.2025),
(2, 83, 4638.44, 7965.20, 13042.96, 4261.79, 10613.90, 827.78, 102.16, 46.1711, 113.0756, 171.8714),
(2, 84, 4766.11, 8172.77, 13204.22, 4355.28, 10917.17, 912.11, 110.26, 46.8275, 113.2640, 174.3162),
(2, 85, 4896.64, 8383.67, 13338.76, 4449.92, 11220.87, 1002.43, 118.77, 47.4839, 113.2768, 176.5370),
(2, 86, 5030.04, 8597.90, 13446.59, 4545.71, 11524.99, 1098.77, 127.69, 48.1402, 113.1139, 178.5337),
(2, 87, 5166.31, 8815.44, 13527.71, 4642.65, 11829.52, 1201.10, 137.02, 48.7965, 112.7753, 180.3063),
(2, 88, 5305.45, 9036.31, 13582.12, 4740.74, 12134.48, 1309.44, 146.76, 49.4528, 112.2609, 181.8549),
(2, 89, 5447.45, 9260.51, 13609.82, 4839.99, 12439.85, 1423.78, 156.92, 50.1091, 111.5709, 183.1795),
(2, 90, 5592.32, 9488.03, 13610.80, 4940.38, 12745.64, 1544.13, 167.48, 50.7653, 110.7051, 184.2800),
(2, 91, 5740.05, 9718.88, 13585.08, 5041.93, 13051.85, 1670.48, 178.46, 51.4215, 109.6637, 185.1564),
(2, 92, 5890.66, 9953.05, 13532.64, 5144.64, 13358.47, 1802.84, 189.85, 52.0777, 108.4465, 185.8087),
(2, 93, 6044.13, 10190.55, 13453.49, 5248.49, 13665.52, 1941.20, 201.65, 52.7338, 107.0536, 186.2370),
(2, 94, 6200.47, 10431.37, 13347.63, 5353.49, 13972.98, 2085.56, 213.87, 53.3899, 105.4850, 186.4413),
(2, 95, 6359.67, 10675.51, 13215.06, 5459.65, 14280.87, 2235.93, 226.49, 54.0460, 103.7407, 186.4215),
(2, 96, 6521.74, 10922.98, 13055.78, 5566.96, 14589.17, 2392.30, 239.53, 54.7021, 101.8207, 186.1776),
(2, 97, 6686.68, 11173.77, 12869.79, 5675.42, 14897.89, 2554.68, 252.97, 55.3581, 99.7250, 185.7096),
(2, 98, 6854.49, 11427.89, 12657.08, 5785.03, 15207.03, 2723.06, 266.83, 56.0141, 97.4536, 185.0176),
(2, 99, 7025.16, 11685.34, 12417.67, 5895.80, 15516.59, 2897.44, 281.10, 56.6700, 95.0065, 184.1016),
(2, 100, 7198.70, 11946.10, 12151.54, 6007.71, 15826.56, 3077.83, 295.79, 57.3260, 92.3837, 182.9615),
(4, 81, 5489.19, 8862.85, 12640.32, 0.00, 9050.32, 714.66, 111.97, 47.9043, 131.5509, 166.3098),
(4, 82, 5641.53, 8991.59, 12855.00, 0.00, 9324.66, 790.54, 121.09, 48.5708, 131.9685, 169.2029),
(4, 83, 5797.46, 9113.21, 13042.96, 0.00, 9599.46, 872.71, 130.74, 49.2374, 132.0348, 171.8720),
(4, 84, 5956.98, 9227.74, 13204.22, 0.00, 9874.72, 961.17, 140.89, 49.9039, 131.7497, 174.3170),
(4, 85, 6120.07, 9335.15, 13338.76, 0.00, 10150.45, 1055.90, 151.56, 50.5705, 131.1133, 176.5379),
(4, 86, 6286.76, 9435.46, 13446.59, 0.00, 10426.65, 1156.93, 162.75, 51.2370, 130.1256, 178.5348),
(4, 87, 6457.02, 9528.66, 13527.71, 0.00, 10703.30, 1264.23, 174.45, 51.9035, 128.7865, 180.3077),
(4, 88, 6630.87, 9614.75, 13582.12, 0.00, 10980.43, 1377.82, 186.66, 52.5700, 127.0960, 181.8565),
(4, 89, 6808.31, 9693.74, 13609.82, 0.00, 11258.01, 1497.69, 199.39, 53.2365, 125.0542, 183.1813),
(4, 90, 6989.33, 9765.61, 13610.80, 0.00, 11536.07, 1623.85, 212.64, 53.9030, 122.6611, 184.2821),
(4, 91, 7173.93, 9830.39, 13585.08, 0.00, 11814.58, 1756.29, 226.40, 54.5695, 119.9166, 185.1587),
(4, 92, 7362.12, 9888.05, 13532.64, 0.00, 12093.56, 1895.01, 240.67, 55.2360, 116.8207, 185.8114),
(4, 93, 7553.90, 9938.61, 13453.49, 0.00, 12373.01, 2040.02, 255.46, 55.9024, 113.3736, 186.2400),
(4, 94, 7749.25, 9982.06, 13347.63, 0.00, 12652.92, 2191.31, 270.76, 56.5689, 109.5750, 186.4446),
(4, 95, 7948.20, 10018.40, 13215.06, 0.00, 12933.29, 2348.88, 286.57, 57.2353, 105.4251, 186.4251),
(4, 96, 8150.72, 10047.64, 13055.78, 0.00, 13214.13, 2512.74, 302.90, 57.9018, 100.9239, 186.1815),
(4, 97, 8356.83, 10069.76, 12869.79, 0.00, 13495.44, 2682.88, 319.75, 58.5682, 96.0713, 185.7140),
(4, 98, 8566.53, 10084.79, 12657.08, 0.00, 13777.20, 2859.31, 337.11, 59.2346, 90.8674, 185.0224),
(4, 99, 8779.81, 10092.70, 12417.67, 0.00, 14059.44, 3042.02, 354.98, 59.9010, 85.3122, 184.1067),
(4, 100, 8996.67, 10093.51, 12151.54, 0.00, 14342.13, 3231.01, 373.37, 60.5674, 79.4055, 182.9670),
(8, 81, 3853.15, 6497.25, 10112.28, 8981.17, 8066.17, 319.55, 75.82, 40.6653, 133.4978, 153.8368),
(8, 82, 3957.29, 6645.09, 10284.07, 9154.91, 8310.42, 353.50, 82.44, 41.2315, 137.6156, 156.5130),
(8, 83, 4063.40, 6793.05, 10434.49, 9330.41, 8555.06, 390.26, 89.47, 41.7978, 141.7353, 158.9818),
(8, 84, 4171.48, 6941.12, 10563.54, 9507.68, 8800.11, 429.82, 96.90, 42.3640, 145.8569, 161.2435),
(8, 85, 4281.53, 7089.30, 10671.23, 9686.70, 9045.55, 472.19, 104.72, 42.9302, 149.9803, 163.2979),
(8, 86, 4393.56, 7237.60, 10757.54, 9867.49, 9291.39, 517.36, 112.95, 43.4965, 154.1056, 165.1450),
(8, 87, 4507.55, 7386.01, 10822.49, 10050.04, 9537.63, 565.34, 121.58, 44.0627, 158.2327, 166.7849),
(8, 88, 4623.52, 7534.53, 10866.07, 10234.35, 9784.27, 616.12, 130.61, 44.6289, 162.3617, 168.2176),
(8, 89, 4741.45, 7683.17, 10888.28, 10420.42, 10031.31, 669.71, 140.04, 45.1951, 166.4926, 169.4430),
(8, 90, 4861.36, 7831.91, 10889.12, 10608.25, 10278.75, 726.10, 149.87, 45.7613, 170.6253, 170.4611),
(8, 91, 4983.24, 7980.78, 10868.59, 10797.85, 10526.58, 785.30, 160.10, 46.3276, 174.7599, 171.2721),
(8, 92, 5107.09, 8129.75, 10826.70, 10989.20, 10774.82, 847.30, 170.73, 46.8938, 178.8964, 171.8757),
(8, 93, 5232.91, 8278.84, 10763.43, 11182.32, 11023.45, 912.11, 181.76, 47.4600, 183.0347, 172.2722),
(8, 94, 5360.71, 8428.05, 10678.80, 11377.19, 11272.48, 979.72, 193.19, 48.0262, 187.1749, 172.4614),
(8, 95, 5490.47, 8577.36, 10572.80, 11573.83, 11521.91, 1050.14, 205.03, 48.5924, 191.3170, 172.4433),
(8, 96, 5622.20, 8726.79, 10445.43, 11772.23, 11771.74, 1123.36, 217.26, 49.1586, 195.4609, 172.2180),
(8, 97, 5755.91, 8876.34, 10296.69, 11972.39, 12021.96, 1199.39, 229.90, 49.7248, 199.6067, 171.7855),
(8, 98, 5891.58, 9025.99, 10126.58, 12174.31, 12272.59, 1278.23, 242.93, 50.2910, 203.7543, 171.1457),
(8, 99, 6029.23, 9175.76, 9935.10, 12378.00, 12523.61, 1359.87, 256.37, 50.8572, 207.9038, 170.2986),
(8, 100, 6168.85, 9325.65, 9722.26, 12583.44, 12775.04, 1444.31, 270.20, 51.4234, 212.0552, 169.2444);
