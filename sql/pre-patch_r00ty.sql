ALTER TABLE `game_event` ADD COLUMN `hardcoded` TINYINT(3) NOT NULL DEFAULT 0;

-- Required by core, no scripts at all
DROP TABLE IF EXISTS `areatrigger_bg_entrance`;
CREATE TABLE `areatrigger_bg_entrance` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT '0' COMMENT 'Identifier',
  `team` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `bg_template` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `exit_map` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `exit_position_x` float NOT NULL DEFAULT '0',
  `exit_position_y` float NOT NULL DEFAULT '0',
  `exit_position_z` float NOT NULL DEFAULT '0',
  `exit_orientation` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Area Trigger BG Entrance';

/*Table structure for table `nostalrius_string` */

DROP TABLE IF EXISTS `nostalrius_string`;

CREATE TABLE `nostalrius_string` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `content_default` text NOT NULL,
  `content_loc1` text,
  `content_loc2` text,
  `content_loc3` text,
  `content_loc4` text,
  `content_loc5` text,
  `content_loc6` text,
  `content_loc7` text,
  `content_loc8` text,
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

