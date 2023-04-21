#include <stdint.h>
#include <float.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <thread>
#include <pthread.h>
#include "define_util.cpp"
#include "Aimbot.cpp"
#include "Memory.cpp"
#include "Offsets.cpp"
#include "Sense.cpp"


void StopSendPacket();

struct CUserCmd {
	int command_number; //0x0
	int tick_count; //0x4
	float command_time; //0x8
	float viewangle_x; //0xC
	float viewangle_y; //viewangle_x + 0x4
	float viewangle_z; //viewangle_x + 0x8
	char pad_0x18[0x14]; //0x18
	float forwardmove; //0x2C
	float sidemove; //0x30
	float upmove; //0x34
	int buttons; //0x38
	char impulse; //0x3C
};


void *silentaim_th_func(void *p) {
	uint64_t commands_ptr = 0x1420a9960;

	SilentaimInfo *info = (SilentaimInfo*)p;

	bool *has_lockedOnPlayer_ptr = info->has_lockedOnPlayer;
	float *silent_aim_x_ptr = info->silent_aim_x;
	float *silent_aim_y_ptr = info->silent_aim_y;

	for (;;) {
		//printf("*has_lockedOnPlayer_ptr = %s\n", (*has_lockedOnPlayer_ptr == true) ? "true" : "false");

		bool attack_flag = false;
		{
			uint64_t commands_ptr = 0x1420a9960;
			uint32_t sequence_number = mem::ReadInt(0x1416524BC);
			CUserCmd* cmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * ((uint64_t)sequence_number % 750)));

			int bef_buttons = mem::ReadInt((long)cmd + 0x38);
			attack_flag = ((bef_buttons & 1) != 0);
		}

				if (attack_flag == true) {
					if (*has_lockedOnPlayer_ptr == true) {
						long tmp_ptr1 = mem::ReadLong(offsets::tmp_ptr1);
						if (tmp_ptr1 != 0) {
							if (mem::ReadInt(tmp_ptr1 + 0x2028) < 7) {//choke < 7
							long localplayer_base = mem::ReadLong(offsets::REGION + offsets::LOCAL_PLAYER);
								if (localplayer_base != 0) {
								long actWeaponID = mem::ReadLong(localplayer_base + offsets::OFFSET_WEAPON) & 0xFFFF;
								long currentWeapon = mem::ReadLong(offsets::REGION + offsets::ENTITY_LIST + (actWeaponID << 5));
								if (currentWeapon != 0) {
								float m_nextReadyTime = mem::ReadFloat(currentWeapon + 0x1648);
								//float nextPrimaryAttackTime = mem::ReadFloat(currentWeapon + offsets::m_nextPrimaryAttackTime);
								float server_time = mem::ReadFloat(mem::ReadLong(offsets::server_time) + 36);
								//if (nextPrimaryAttackTime <= server_time) {
								if (m_nextReadyTime == 0.0f || m_nextReadyTime <= server_time) {
									uint32_t bak1 = mem::ReadInt(mem::ReadLong(offsets::tmp_ptr1) + 13024);
									uint32_t bak2 = mem::ReadInt(offsets::bak2);
									double bak3 = (double)mem::ReadLong(mem::ReadLong(offsets::tmp_ptr1) + 8456);
									double bak4 = (double)mem::ReadLong(offsets::bak4);
									double bak5 = (double)mem::ReadLong(offsets::bak5);
 
									StopSendPacket();
 
									int32_t current_number = mem::ReadInt(offsets::sequence_number);
									int32_t iDesiredCmdNumber = current_number + 1;
									CUserCmd* old_usercmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * (((uint64_t)iDesiredCmdNumber - 1) % 750)));
									CUserCmd* usercmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * ((uint64_t)iDesiredCmdNumber % 750)));
 
									while (mem::ReadInt((long)usercmd) < iDesiredCmdNumber) {
										StopSendPacket();
										std::this_thread::yield();
									}
									int32_t old_current_number = mem::ReadInt(offsets::sequence_number);
 
									//mem::WriteFloat((long)old_usercmd + 0xC, 0.0f);
									//mem::WriteFloat((long)old_usercmd + 0xC + 4, 0.0f);
									mem::WriteFloat((long)old_usercmd + 0xC, *silent_aim_x_ptr);
									mem::WriteFloat((long)old_usercmd + 0xC + 4, *silent_aim_y_ptr);
 
									//printf("silent x = %f, y = %f\n", *silent_aim_x_ptr, *silent_aim_y_ptr);
 
									int bef_buttons = mem::ReadInt((long)old_usercmd + 0x38);
									mem::WriteInt((long)old_usercmd + 0x38, bef_buttons | 1);
 
									mem::WriteInt(mem::ReadLong(offsets::tmp_ptr1) + 13024, bak1);
									mem::WriteInt(offsets::bak2, bak2);
									mem::WriteLong(mem::ReadLong(offsets::tmp_ptr1) + 8456, (long)bak3);
									mem::WriteLong(offsets::bak4, (long)bak4);
									mem::WriteLong(offsets::bak5, (long)bak5);
 
									//printf("silent [time=%f]\n", server_time);
								}
							}
						}
					}
				}
			}
		}
				
	}
}

int main(int argc, char *argv[]) {
	if (getuid()) {
		printf("MUST RUN AS ROOT!\n");
		return -1;
	}
	if (mem::GetPID() == 0) {
		printf("GAME NOT FOUND. EXITING!\n");
		return -1;
	}

	bool has_lockedOnPlayer = false;
	float silent_aim_x = 0.0f;
	float silent_aim_y = 0.0f;
	SilentaimInfo info = {&has_lockedOnPlayer, &silent_aim_x, &silent_aim_y}; 
	pthread_t pthread;
	pthread_create(&pthread, NULL, &silentaim_th_func, &info);

	Level level;
	LocalPlayer localPlayer;

	std::vector<Player *> *players = new std::vector<Player *>;
	for (int i = 0; i < 60; i++) {
		players->push_back(new Player(i));
	}

	Aimbot *aimbot = new Aimbot(&level, &localPlayer, players, &has_lockedOnPlayer, &silent_aim_x, &silent_aim_y);
	int counter = 0;
	Sense *sense = new Sense(&level, &localPlayer, players);
	printf("START MAIN LOOP!\n");

	for (;;) {
		try {
			localPlayer.markForPointerResolution();
			for (int i = 0; i < players->size(); i++) {
				Player *player = players->at(i);
				player->markForPointerResolution();
			}
			sense->update(counter);
			aimbot->update();
		}
		catch (...) {
			//printf("ERROR (LOADING SCREEN?). SLEEPING FOR 10 SECONDS\n");
			//std::this_thread::sleep_for(std::chrono::seconds(10));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void StopSendPacket() {
	mem::WriteInt(mem::ReadLong(offsets::tmp_ptr1) + 13024, 0);
	mem::WriteInt(offsets::bak2, 0);
	mem::WriteLong(mem::ReadLong(offsets::tmp_ptr1) + 8456, (long)DBL_MAX);
 
	mem::WriteLong(offsets::bak4, (long)(-DBL_MAX / 2));
	mem::WriteLong(offsets::bak5, (long)(DBL_MAX / 2));
}
