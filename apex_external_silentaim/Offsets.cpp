#pragma once
namespace offsets
{
	// buttons
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const long IN_ATTACK = 0x07871c50;            // [Buttons] -> in_attack
	// core
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const long REGION = 0x140000000;              // [Mine]          -> Region
	const long LEVEL = 0x1639960;                 // [Miscellaneous] -> LevelName
	const long LOCAL_PLAYER = (0x020c1130 + 0x8);       // [Miscellaneous] -> AVC_GameMovement+0x8
	const long ENTITY_LIST = 0x1d0fe58;           // [Miscellaneous] -> cl_entitylist
	// entity
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const long LOCAL_ORIGIN = 0x0158; // [DataMap.CBaseViewModel]    -> m_localOrigin
	// player
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const long GLOW_ENABLE = (0x03c0 + 0x8);        // [RecvTable.DT_HighlightSettings] -> m_highlightServerContextID + 0x8
	const long GLOW_THROUGH_WALL = (0x03c0 + 0x10); // [RecvTable.DT_HighlightSettings] -> m_highlightServerContextID + 0x10
	const long GLOW_COLOR = 0x1d0;                  // [Miscellaneous]                  -> glow_color
	const long TEAM_NUMBER = 0x044c;                // [RecvTable.DT_BaseEntity]        -> m_iTeamNum
	const long NAME = 0x0589;                       // [RecvTable.DT_BaseEntity]        -> m_iName
	const long LIFE_STATE = 0x0798;                 // [RecvTable.DT_Player]            -> m_lifeState
	const long VEC_PUNCH_WEAPON_ANGLE = 0x24b0;     // [DataMap.C_Player]               -> m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
	const long VIEW_ANGLE = (0x25ac - 0x14);        // [DataMap.C_Player]               -> m_ammoPoolCapacity - 0x14
	const long BLEEDOUT_STATE = 0x2740;             // [RecvTable.DT_Player]            -> m_bleedoutState
	const long ZOOMING = 0x1c51;                    // [RecvTable.DT_Player]            -> m_bZooming
	const long LAST_VISIBLE_TIME = 0x1a78;          // [Miscellaneous]                  -> CPlayer!lastVisibleTime
	const long CURRENT_SHIELDS = 0x0170;             // m_shieldHealth
	
	const long OFFSET_BULLET_SPEED = 0x1f48;
	const long OFFSET_BULLET_SCALE = 0x1F50;

	const long OFFSET_WEAPON = 0x1a1c; //[DataMap.C_BaseCombatCharacter]m_latestPrimaryWeapons

	const long m_nextPrimaryAttackTime = 0x164c;

	const long OFFSET_ORIGIN = 0x014c; //m_vecAbsOrigin
	const long OFFSET_CAMERAPOS = 0x1f48; //CPlayer!camera_origin
	const long OFFSET_BREATH_ANGLES = (VIEW_ANGLE - 0x10);

	const long OFFSET_BONES = (0x0e98 + 0x48); //m_nForceBone + 0x48
	
	//silentaim 
	const long commands_ptr = 0x1420a9960;
   	const long sequence_number = 0x1416524BC; 
   	const long tmp_ptr1 = 0x141639800;
   	const long server_time =0x141536228; 
   	const long bak2 = 0x1485fa900; 
    	const long bak4 = 0x141639850; 
   	const long bak5 = 0x142316770;
}
