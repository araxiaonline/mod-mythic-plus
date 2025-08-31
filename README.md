# Mythic Plus Module for AzerothCore

🚨 **This is not mythic like in retail.** 🚨

This module started with the intention of creating a retail-like mythic plus system but has evolved into something unique. Instead, think of it as an **expansion of existing Wrath to new end game conte r** that reuses much of the existing dungeons and creatures, overhauling them to much higher difficulties with new loot mechanics.

## Overview

The Mythic Plus module transforms existing World of Warcraft dungeons (Wrath of the Lich King to add additional endgame content) into challenging, scaled encounters with progressive difficulty levels, enhanced rewards, and unique gameplay mechanics to extend gameplay experience.

## Features

### Core Systems
- **Progressive Difficulty Scaling**: Multiple tiers of difficulty that scale creature stats, abilities, and rewards  
- **Player Advancement System**: Custom advancements for mythic plus progression, completion times, and special challenges
- **Voiced NPCs**: Enhanced immersion with voice acting for key NPCs and encounters
- **Dynamic Loot System**: New loot mechanics with scaled rewards based on difficulty completed
- **Death Penalty System**: Configurable death penalties that affect progression
- **Time-Based Challenges**:
- (In progress)

### Massive Content Expansion
- **50,000+ New Items**: Completely new item database with scaled equipment for all difficulty tiers
- **New Rare Materials**: Custom crafting materials and rare drops exclusive to mythic plus content
- **Reworked Tier Gear**: All expansion tier sets completely reworked and rebalanced for mythic plus scaling
- **Cross-Expansion Integration**: Unified progression system spanning Vanilla through Wrath

### Difficulty Tiers
The module supports multiple difficulty levels with configurable scaling:

- **Mythic Dungeons**: Base enhanced difficulty with improved creature stats and new rare material drops
- **Legendary Dungeons**: In progress 
- **Ascendant Dungeons**: In progress

### Per-Dungeon Customization
- **Database-Driven Scaling**: Each dungeon can have unique scaling values configured through database tables
- **Individual Encounter Tuning**: Specific creatures and bosses can have custom multipliers independent of global settings
- **Dungeon-Specific Materials**: Unique rare materials and rewards exclusive to certain dungeons or difficulty combinations

### Creature Enhancements
- **Health & Damage Scaling**: Configurable multipliers for different difficulty tiers

- **Voiced Encounters**: Important NPCs and bosses feature voice acting for enhanced immersion

- **Cross-Expansion Itemization**: Unified item progression that spans all supported expansions

## Installation

### Prerequisites
-- Trickerer Acore or Azerothcore 
- MySQL database 8.4
- C++ compiler with C++20 support
- docker for most tested experience
- companion lua scripts **more details coming here**

### Build Process

1. **Clone the repository** into your AzerothCore modules directory:
   ```bash
   cd /path/to/azerothcore/modules
   git clone https://github.com/araxiaonline/mod-mythic-plus.git
   ```

2. **Rebuild AzerothCore** with the module:
   ```bash
   cd /path/to/azerothcore/build
   make -j$(nproc)
   ```

3. **Install database changes**:
   Database changes will be installed using azerothcores built in script, if it does not you can use the following commands below: 

   ```bash
   # Apply the SQL files from data/sql/ in the correct order
   # Note: This includes extensive item database additions (50,000+ items)
   # and achievement system integration
   mysql -u username -p acore_world < modules/mod-mythic-plus/data/sql/db-characters/*.sql
   mysql -u username -p acore_world < modules/mod-mythic-plus/data/sql/db-world/*.sql
   
   # Verify the massive item additions were applied successfully
   # The module includes a complete item database overhaul
   ```

### Configuration

1. **Copy the configuration file**:
   ```bash
   cp modules/mod-mythic-plus/conf/mod-mythic-plus.conf.dist /path/to/azerothcore/etc/modules/mod-mythic-plus.conf
   ```

2. **Edit the configuration file** to customize your settings:
   ```bash
   nano /path/to/azerothcore/etc/modules/mod-mythic-plus.conf
   ```

## Configuration Options

### Global Settings
```ini
# Enable/disable the entire Mythic Plus system
MythicPlus.Enabled = 1

# Enable enhanced rewards system
MythicPlus.EnableEnhancedRewards = 1

# Enable achievement system integration
MythicPlus.EnableAchievementSystem = 1

# Enable death limit system for dungeons
MythicPlus.EnableDeathLimits = 1
```

### Difficulty Scaling
```ini
# Mythic tier creature scaling
MythicPlus.Mythic.DungeonHealth = 1.5
MythicPlus.Mythic.DungeonDmg = 1.25
MythicPlus.Mythic.DungeonDmgSpell = 1.3
MythicPlus.Mythic.DungeonArmor = 1.25
MythicPlus.Mythic.DungeonXPGain = 1.5

# Legendary tier creature scaling  
MythicPlus.Legendary.DungeonHealth = 2.25
MythicPlus.Legendary.DungeonDmg = 2.25
MythicPlus.Legendary.DungeonDmgSpell = 2.75
MythicPlus.Legendary.DungeonArmor = 3.25
MythicPlus.Legendary.DungeonXPGain = 85

# Ascendant tier creature scaling
MythicPlus.Ascendant.DungeonHealth = 3.25
MythicPlus.Ascendant.DungeonDmg = 3.25
MythicPlus.Ascendant.DungeonDmgSpell = 3.75
MythicPlus.Ascendant.DungeonArmor = 3.25
MythicPlus.Ascendant.DungeonXPGain = 87
```

### Death Penalty Settings
```ini
# Death limits before failure
MythicPlus.Death.DeathLimit = 5

# Enable/disable death penalties
MythicPlus.EnableDeathDifficulties = 1

# Death penalty scaling
MythicPlus.DisableDamageSpell = 1
```

## Supported Dungeons

The module enhances dungeons from multiple expansions:

### Wrath of the Lich King
- All major dungeons receive scaling enhancements
- Heroic and normal modes both supported
- Enhanced creature abilities and mechanics

###
### Cross-Expansion Features
- Unified scaling system across all supported content
- Consistent reward progression regardless of original expansion
- Balanced challenge curve from entry-level to extreme difficulti
## Commands

### Player Commands
- **`.mythicplus info`** - Display current dungeon difficulty and stats
- **`.mythicplus status`** - Show personal mythic plus progression and achievements
- **`.mythicplus materials`** - View collected rare materials and crafting components
- **`.mythicplus tokens`** - Check endgame token balance and available exchanges
- **`.mythicplus achievements`** - Display mythic plus specific achievement progress
- **`.mythicplus reset`** - Reset personal statistics (if enabled)

### GM Commands  
- **`.mythicplus enable <level>`** - Set dungeon difficulty level
- **`.mythicplus disable`** - Disable mythic plus for current dungeon
- **`.mythicplus reload`** - Reload configuration without restart
- **`.mythicplus debug <player>`** - Show debug information for specified player
- **`.mythicplus scaling <dungeon_id> <multiplier>`** - Set custom scaling for specific dungeon
- **`.mythicplus materials add <player> <material_id> <count>`** - Add rare materials to player
- **`.mythicplus tokens grant <player> <token_type> <amount>`** - Grant endgame tokens

## Gameplay Mechanics

### Difficulty Progression
1. **Entry Level**: Start with Mythic difficulty for enhanced challenge
2. **Intermediate**: Progress to Legendary for significant stat increases
3. **Advanced**: Attempt Ascendant level for extreme challenges
4. **Mastery**: Custom difficulty configurations for server-specific content

### Death System
- Players have limited deaths before dungeon failure
- Death count affects final rewards and achievements  
- Configurable penalties including reduced experience or currency

### Reward Scaling
- **50,000+ New Items**: Massive equipment database with items scaling from entry-level mythic to extreme ascendant tiers
- **Rare Material System**: Exclusive crafting materials that drop only in mythic plus content, used for high-end crafting and upgrades
- **Endgame Token Economy**: Multiple token types for different content tiers, exchangeable for premium rewards
- **Reworked Tier Sets**: Complete overhaul of all expansion tier gear, rescaled and rebalanced for mythic plus progression
- **Achievement Rewards**: Exclusive items, titles, and cosmetic rewards locked behind achievement completion
- **Cross-Expansion Progression**: Items and materials from different expansions integrated into a unified progression system

## Customization

### Adding Custom Difficulties
The module supports adding custom difficulty tiers by:
1. Editing the configuration file to add new scaling values
2. Updating database entries for custom creature stats
3. Implementing custom reward tables for new difficulties

### Creature Modifications
- Individual creature scaling can be customized per dungeon
- Special abilities can be added or modified through database entries
- Custom spell rotations and AI behaviors configurable

### Loot Customization
- **50,000+ Item Integration**: Massive custom item database with proper scaling and progression
- **Rare Material Drops**: Custom drop tables for exclusive mythic plus materials
- **Endgame Token Distribution**: Configurable token rewards based on difficulty and performance
- **Tier Set Reworks**: All expansion tier sets completely rebalanced with new set bonuses and scaling
- **Per-Dungeon Loot Tables**: Each dungeon can have unique reward pools and exclusive items
- **Achievement-Gated Rewards**: Special items and materials locked behind specific achievement completions

## Troubleshooting

### Common Issues

**Module not loading**:
- Verify the module was compiled correctly
- Check server logs for compilation errors
- Ensure all dependencies are met

**Database errors**:
- Confirm all SQL files were applied in correct order
- Verify database user has appropriate permissions
- Check for conflicting modules or custom modifications

**Configuration not working**:
- Verify configuration file is in correct location
- Check file permissions and syntax
- Use `.mythicplus reload` command after making changes

**Performance issues**:
- Monitor server resources during high-difficulty encounters
- Adjust scaling values if creatures are too powerful
- Consider reducing concurrent mythic plus dungeons

### Debug Mode
Enable debug logging by setting:
```ini
MythicPlus.DebugMode = 1
```

This provides detailed information about:
- Creature scaling calculations
- Reward calculations  
- Player progression tracking
- System performance metrics

## Contributing

### Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes following the existing code style
4. Test thoroughly on a development server
5. Submit a pull request with detailed description

### Code Standards
- Follow AzerothCore coding conventions
- Include appropriate comments and documentation
- Test all changes across multiple difficulty levels
- Ensure compatibility with other popular modules

### Bug Reports
When reporting bugs, include:
- AzerothCore version and commit hash
- Module version/commit
- Configuration file settings
- Detailed reproduction steps
- Server logs showing the issue

## Credits

**Development Team**: Araxia Online Development Team
- **Lead Developer**: ben-of-codecraft
- **Contributors**: james-huston

**Special Thanks**:
- AzerothCore development team for the excellent foundation
- Community members who provided testing and feedback
- Original World of Warcraft developers for creating the content we enhance

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.

## Support

- **GitHub Issues**: Report bugs and request features
- **Community Discord**: Join discussions and get help from other users
- **Documentation**: Check the wiki for additional guides and tutorials

---

*Remember: This module significantly increases dungeon difficulty. Start with lower difficulties and work your way up to avoid frustration. The enhanced challenge is designed to provide engaging content for experienced players seeking greater difficulty than the original game provided.*



