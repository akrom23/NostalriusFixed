CREATE TEMPORARY TABLE disabled_maps (id int(11) default 0);
-- Instances disabled at Nostal release ( pre 1.4 ) : BWL ZG AQ DM NAXX
INSERT INTO disabled_maps VALUES
(533),   -- NAXX
(531),    -- AQ40 (Temple of AQ)
(509),    -- AQ20 (Ruins of AQ)
(429),    -- DIRE MAUL
(309),    -- ZUL GURUB
(469)     -- BWL
;

update areatrigger_teleport set required_level=61 where target_map in (SELECT id FROM disabled_maps);
UPDATE creature SET spawnflags = 2 WHERE map IN (SELECT id FROM disabled_maps);
UPDATE gameobject SET spawnflags = 2 WHERE map IN (SELECT id FROM disabled_maps);

-- World boss desactivation
update creature set spawnflags=2 where id in
(14887,     -- Ysondre
14890,      -- Taerar
14888,      -- Lethon
14889,      -- Emeriss
6109,       -- Azuregos
12397       -- Lord Kazzak
);


-- Disable Alterac Valley
DELETE FROM game_event WHERE entry=18; -- AV Weekend PvP event
UPDATE battleground_template SET MinLvl = 61, MaxLvl = 61 WHERE id=1;
DELETE FROM creature_template WHERE entry IN (347,5118,7410,7427,12197,14942,15103,15106); -- Remove NPC to tag battleground


-- Remove pre1.4 mounts from vendor
delete from npc_vendor where item in (15293,15292,12330,12351,8586,13317,12353,12354,13328,13329,12302,12303,13326,13327,13334);

-- Disable Gurubashi Event (Wow patchnote 1.5)
update game_event set start_time="2020-08-04 03:00:00" where entry=16;
update game_event set start_time="2020-08-04 06:00:00" where entry=38;
update game_event set start_time="2020-08-04 05:55:00" where entry=39;

-- Disable Elemental invasions (Wow patchnote 1.5)
UPDATE gameobject SET spawnFlags=2 WHERE id IN (179664, 181650, 181651, 181542); -- Portals
UPDATE creature SET spawnFlags = 2 WHERE id IN (14460, 14461, 14458, 14457, 14462, 14464, 14455, 14454); -- Bosses and elemental adds

-- Darkmoon Faire (Wow patchnote 1.6)
--      Event
DELETE FROM game_event WHERE entry IN (4, 5, 100, 101);
--      Darkmoonfaire cards
DELETE FROM item_template WHERE entry IN (19227,19228,19230,19231,19232,19233,19234,19235,19236,19267,19268,19269,19270,19271,19272,19273,19274,19275,19276,19277,19278,19279,19280,19281,19282,19283,19284,19257,19258,19259,19260,19261,19262,19263,19264,19265,19287,19288,19289,19290);

-- Disable Stranglethorn fishing contest (Wow patchnote 1.7)
update game_event set start_time="2025-08-04 06:00:00", end_time="2030-12-30 23:00:00" where entry=14;
update game_event set start_time="2025-08-05 14:00:00", end_time="2030-12-30 23:00:00" where entry=15;


-- ----------------------------------------------
-- AQ Opening quest chain
-- Disable quest chain
DELETE FROM quest_template WHERE entry IN (8288, 8286);
