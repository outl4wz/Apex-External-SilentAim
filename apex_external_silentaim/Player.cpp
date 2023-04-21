#pragma once
#include <iostream>
#include "Utils.cpp"
#include "Offsets.cpp"
#include "Memory.cpp"

class Player
{
private:
	int m_entityListIndex;
	float m_lastVisibleTime;
	long m_basePointer = 0;

public:
	Player(int entityListIndex)
	{
		m_entityListIndex = entityListIndex;
	}
	long getUnresolvedBasePointer()
	{
		long unresolvedBasePointer = offsets::REGION + offsets::ENTITY_LIST + ((m_entityListIndex + 1) << 5);
		return unresolvedBasePointer;
	}
	long getBasePointer()
	{
		if (m_basePointer == 0)
			m_basePointer = mem::ReadLong(getUnresolvedBasePointer());
		return m_basePointer;
	}
	int get_m_entityListIndex() {
		return m_entityListIndex;
	}
	void markForPointerResolution()
	{
		m_basePointer = 0;
	}
	void get_bone_pos(int bone_id, my_vector* bonepos) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::OFFSET_ORIGIN;
		my_vector pos;
		pos.x = mem::ReadFloat(ptrLong);
		pos.y = mem::ReadFloat(ptrLong + 4);
		pos.z = mem::ReadFloat(ptrLong + 8);

		long bone_ptr = mem::ReadLong(basePointer + offsets::OFFSET_BONES);
		int boneloc = (bone_id * 0x30);

		bonepos->x = pos.x + mem::ReadFloat(bone_ptr + boneloc + 0xCC);
		bonepos->y = pos.y + mem::ReadFloat(bone_ptr + boneloc + 0xCC + 4 + 0xC);
		bonepos->z = pos.z + mem::ReadFloat(bone_ptr + boneloc + 0xCC + 4 + 0xC + 4 + 0xC);
	}
	bool isDead()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LIFE_STATE;
		short result = mem::ReadShort(ptrLong);
		return result > 0;
	}
	bool isKnocked()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::BLEEDOUT_STATE;
		short result = mem::ReadShort(ptrLong);
		return result > 0;
	}
	std::string getName()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::NAME;
		std::string result = mem::ReadString(ptrLong);
		return result;
	}
	bool isValid()
	{
		return getBasePointer() > 0 && !isDead();
	}
	std::string getInvalidReason()
	{
		if (getBasePointer() == 0)
			return "Unresolved base pointer";
		else if (isDead())
			return "Player is dead";
		else if (getName().empty())
			return "Name is empty";
		else
			return "Player is valid";
	}
	float getLocationX()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LOCAL_ORIGIN;
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	float getLocationY()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LOCAL_ORIGIN + sizeof(float);
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	float getLocationZ()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LOCAL_ORIGIN + sizeof(float) + sizeof(float);
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	int getTeamNumber()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::TEAM_NUMBER;
		int result = mem::ReadInt(ptrLong);
		return result;
	}
	int getShieldsValue()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::CURRENT_SHIELDS;
		int result = mem::ReadInt(ptrLong);        
		return result;
	}
	void setCustomGlow()
    {
        long basePointer = getBasePointer();
        long glowDistance = basePointer + 0x3B4;


        long offset1 = basePointer + 0x2C4;
        long offset2 = basePointer + 0x3c8;
        long offset3 = basePointer + 0x3d0;




        mem::WriteInt(offset1, 1512990053);
        mem::WriteInt(offset2, 1);
        mem::WriteInt(offset3, 2);

        mem::WriteFloat(glowDistance, 99999999.0f);

    }
	
    void setCustomGlowRed()
    {
        long basePointer = getBasePointer();

        long redColor = basePointer + 0x1D0;
        long greenColor = basePointer + 0x1D4;
        long blueColor = basePointer + 0x1D8;

        mem::WriteFloat(redColor, 255.0f);
        mem::WriteFloat(greenColor, 0.0f);
        mem::WriteFloat(blueColor, 0.0f);

    }
    void setCustomGlowGreen()
    {
        long basePointer = getBasePointer();

        long redColor = basePointer + 0x1D0;
        long greenColor = basePointer + 0x1D4;
        long blueColor = basePointer + 0x1D8;


        mem::WriteFloat(redColor, 0.0f);
        mem::WriteFloat(greenColor, 100.0f);
        mem::WriteFloat(blueColor, 0.0f);
    }
    void setCustomGlowWhite()
    {
        long basePointer = getBasePointer();

        long redColor = basePointer + 0x1D0;
        long greenColor = basePointer + 0x1D4;
        long blueColor = basePointer + 0x1D8;

        mem::WriteFloat(redColor, 255.0f);
        mem::WriteFloat(greenColor, 255.0f);
        mem::WriteFloat(blueColor, 255.0f);
    }
    void setCustomGlowBlue()
    {
        long basePointer = getBasePointer();

        long redColor = basePointer + 0x1D0;
        long greenColor = basePointer + 0x1D4;
        long blueColor = basePointer + 0x1D8;

        mem::WriteFloat(redColor, 0.0f);
        mem::WriteFloat(greenColor, 117.0f);
        mem::WriteFloat(blueColor, 209.0f);
    }
    void setCustomGlowPurple()
    {
        long basePointer = getBasePointer();

        long redColor = basePointer + 0x1D0;
        long greenColor = basePointer + 0x1D4;
        long blueColor = basePointer + 0x1D8;

        mem::WriteFloat(redColor, 126.0f);
        mem::WriteFloat(greenColor, 0.0f);
        mem::WriteFloat(blueColor, 255.0f);
    }
	int getShieldValue() {
        long basePointer = getBasePointer();
        long shieldOffset = basePointer + 0x0170;
        int result = mem::ReadInt(shieldOffset);
        return result;
    }
	int getGlowEnable()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_ENABLE;
		int result = mem::ReadInt(ptrLong);
		return result;
	}
	void setGlowEnable(int glowEnable)
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_ENABLE;
		mem::WriteInt(ptrLong, glowEnable);
	}
	int getGlowThroughWall()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_THROUGH_WALL;
		int result = mem::ReadInt(ptrLong);
		return result;
	}
	void setGlowThroughWall(int glowThroughWall)
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_THROUGH_WALL;
		mem::WriteInt(ptrLong, glowThroughWall);
	}
	int getGlowColorRed()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_COLOR;
		int result = mem::ReadInt(ptrLong);
		return result;
	}
	void setGlowColorRed(float color)
	{
		if (color > 100)
			color = 100;
		if (color < 0)
			color = 0;
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_COLOR;
		mem::WriteFloat(ptrLong, color);
	}
	void setGlowColorGreen(float color)
	{
		if (color > 100)
			color = 100;
		if (color < 0)
			color = 0;
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_COLOR + sizeof(float);
		mem::WriteFloat(ptrLong, color);
	}
	void setGlowColorBlue(float color)
	{
		if (color > 100)
			color = 100;
		if (color < 0)
			color = 0;
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::GLOW_COLOR + sizeof(float) + sizeof(float);
		;
		mem::WriteFloat(ptrLong, color);
	}
	float getLastVisibleTime()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LAST_VISIBLE_TIME;
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	bool isVisible()
	{
		const float lastVisibleTime = getLastVisibleTime();
		const bool isVisible = lastVisibleTime > m_lastVisibleTime;
		m_lastVisibleTime = lastVisibleTime;
		return isVisible;
	}
	void print()
	{
		std::cout << "Player[" + std::to_string(m_entityListIndex) + "]:\n";
		std::cout << "\tUnresolvedBasePointer:\t\t\t" + mem::convertPointerToHexString(getUnresolvedBasePointer()) + "\n";
		std::cout << "\tBasePointer:\t\t\t\t" + mem::convertPointerToHexString(getBasePointer()) + "\n";
		std::cout << "\tIsValid:\t\t\t\t" + std::to_string(isValid()) + "\n";
		std::cout << "\tInvalidReason:\t\t\t\t" + getInvalidReason() + "\n";
		if (!isValid())
		{
			std::cout << "\tLocationOriginX:\t\t\t" + utils::convertNumberToString(getLocationX()) + "\n";
			std::cout << "\tLocationOriginY:\t\t\t" + utils::convertNumberToString(getLocationY()) + "\n";
			std::cout << "\tLocationOriginZ:\t\t\t" + utils::convertNumberToString(getLocationZ()) + "\n";
			std::cout << "\tTeamNumber:\t\t\t\t" + utils::convertNumberToString(getTeamNumber()) + "\n";
			std::cout << "\tGlowEnable:\t\t\t\t" + utils::convertNumberToString(getGlowEnable()) + "\n";
			std::cout << "\tGlowThroughWall:\t\t\t" + utils::convertNumberToString(getGlowThroughWall()) + "\n";
		}
	}
};
