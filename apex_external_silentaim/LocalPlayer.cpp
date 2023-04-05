#pragma once
#include <iostream>
#include "define_util.cpp"
#include "Memory.cpp"
#include "Offsets.cpp"

class LocalPlayer
{
private:
	long m_basePointer = 0;
	long getUnresolvedBasePointer()
	{
		long unresolvedBasePointer = offsets::REGION + offsets::LOCAL_PLAYER;
		return unresolvedBasePointer;
	}
	long getBasePointer()
	{
		if (m_basePointer == 0)
			m_basePointer = mem::ReadLong(getUnresolvedBasePointer());
		return m_basePointer;
	}

public:
	void markForPointerResolution()
	{
		m_basePointer = 0;
	}
	void PredictPos(long target, my_vector* bonePos)
	{
		long basePointer = getBasePointer();
		long actWeaponID = mem::ReadLong(basePointer + offsets::OFFSET_WEAPON) & 0xFFFF;
		long currentWeapon = mem::ReadLong(offsets::REGION + offsets::ENTITY_LIST + (actWeaponID << 5));

		if (currentWeapon != 0)
		{
			float bulletSpeed = mem::ReadFloat(currentWeapon + offsets::OFFSET_BULLET_SPEED);
			float bulletGravity = mem::ReadFloat(currentWeapon + offsets::OFFSET_BULLET_SCALE);

			if (bulletSpeed > 1.f)
			{
				my_vector muzzle;
				get_camera_pos(&muzzle);
				float time = bonePos->DistTo(muzzle) / bulletSpeed;
				bonePos->z += (700.f * bulletGravity * 0.5f) * (time * time);
				my_vector velDelta;
				velDelta.x = mem::ReadFloat(target + offsets::OFFSET_ORIGIN - 0xC) * time;
				velDelta.y = mem::ReadFloat(target + offsets::OFFSET_ORIGIN - 0xC + 4) * time;
				velDelta.z = mem::ReadFloat(target + offsets::OFFSET_ORIGIN - 0xC + 8) * time;
				bonePos->x += velDelta.x;
				bonePos->y += velDelta.y;
				bonePos->z += velDelta.z;
			}
		}
	}
	void get_camera_pos(my_vector* camerapos) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::OFFSET_CAMERAPOS;
		camerapos->x = mem::ReadFloat(ptrLong);
		camerapos->y = mem::ReadFloat(ptrLong + 4);
		camerapos->z = mem::ReadFloat(ptrLong + 8);
	}
	void get_viewangles(my_vector* viewangles) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE;
		viewangles->x = mem::ReadFloat(ptrLong);
		viewangles->y = mem::ReadFloat(ptrLong + 4);
		viewangles->z = mem::ReadFloat(ptrLong + 8);
	}
	void set_viewangles(my_vector* viewangles) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE;
		mem::WriteFloat(ptrLong, viewangles->x);
		mem::WriteFloat(ptrLong + 4, viewangles->y);
	}
	void get_punchangles(my_vector* punchangles) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VEC_PUNCH_WEAPON_ANGLE;
		punchangles->x = mem::ReadFloat(ptrLong);
		punchangles->y = mem::ReadFloat(ptrLong + 4);
		punchangles->z = mem::ReadFloat(ptrLong + 8);
	}
	void get_breathangles(my_vector* breathangles) {
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::OFFSET_BREATH_ANGLES;
		breathangles->x = mem::ReadFloat(ptrLong);
		breathangles->y = mem::ReadFloat(ptrLong + 4);
		breathangles->z = mem::ReadFloat(ptrLong + 8);
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
		long ptrLong = basePointer + offsets::LOCAL_ORIGIN + (sizeof(float) * 2);
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
	bool isInAttack()
	{
		long basePointer = getBasePointer();
		long ptrLong = offsets::REGION + offsets::IN_ATTACK;
		int result = mem::ReadInt(ptrLong);
		return result > 0;
	}
	std::string getName()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::NAME;
		std::string result = mem::ReadString(ptrLong);
		return result;
	}
	bool isKnocked()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::BLEEDOUT_STATE;
		short result = mem::ReadShort(ptrLong);
		return result > 0;
	}
	bool isDead()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::LIFE_STATE;
		short result = mem::ReadShort(ptrLong);
		return result > 0;
	}
	float getPunchPitch()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VEC_PUNCH_WEAPON_ANGLE;
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	float getPunchYaw()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VEC_PUNCH_WEAPON_ANGLE + sizeof(float);
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	float getPitch()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE;
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	void setPitch(float angle)
	{
		if (angle > 90 || angle < -90)
			return;
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE;
		mem::WriteFloat(ptrLong, angle);
	}
	float getYaw()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE + sizeof(float);
		float result = mem::ReadFloat(ptrLong);
		return result;
	}
	void setYaw(float angle)
	{
		if (angle > 180 || angle < -180)
			return;
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::VIEW_ANGLE + sizeof(float);
		mem::WriteFloat(ptrLong, angle);
	}
	bool isZooming()
	{
		long basePointer = getBasePointer();
		long ptrLong = basePointer + offsets::ZOOMING;
		short result = mem::ReadShort(ptrLong);
		return result > 0;
	}
};
