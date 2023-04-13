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
	uint64_t commands_ptr = 0x1420AAA00;

	SilentaimInfo *info = (SilentaimInfo*)p;

	bool *has_lockedOnPlayer_ptr = info->has_lockedOnPlayer;
	float *silent_aim_x_ptr = info->silent_aim_x;
	float *silent_aim_y_ptr = info->silent_aim_y;

	for (;;) {
		//printf("*has_lockedOnPlayer_ptr = %s\n", (*has_lockedOnPlayer_ptr == true) ? "true" : "false");

		bool attack_flag = false;
		{
			uint64_t commands_ptr = 0x1420AAA00;
			uint32_t sequence_number = mem::ReadInt(0x14165353C);
			CUserCmd* cmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * ((uint64_t)sequence_number % 750)));

			int bef_buttons = mem::ReadInt((long)cmd + 0x38);
			attack_flag = ((bef_buttons & 1) != 0);
		}

		if (attack_flag == true) {
			if (*has_lockedOnPlayer_ptr == true) {
				long tmp_ptr1 = mem::ReadLong(0x14163A880);
				if (tmp_ptr1 != 0) {
					if (mem::ReadInt(tmp_ptr1 + 0x2028) < 7) {//choke < 7
						long localplayer_base = mem::ReadLong(offsets::REGION + offsets::LOCAL_PLAYER);
						if (localplayer_base != 0) {
							long actWeaponID = mem::ReadLong(localplayer_base + offsets::OFFSET_WEAPON) & 0xFFFF;
							long currentWeapon = mem::ReadLong(offsets::REGION + offsets::ENTITY_LIST + (actWeaponID << 5));
							if (currentWeapon != 0) {
								char semiauto_flag = mem::ReadChar(currentWeapon + 0x1C0C);
								float m_nextReadyTime = mem::ReadFloat(currentWeapon + 0x1648);
								float nextPrimaryAttackTime = mem::ReadFloat(currentWeapon + offsets::m_nextPrimaryAttackTime);
								float server_time = mem::ReadFloat(mem::ReadLong(0x141537228) + 36);
								//if (nextPrimaryAttackTime <= server_time) {
								//if (m_nextReadyTime == 0.0f || m_nextReadyTime <= server_time) {
								if ((semiauto_flag != 0 && nextPrimaryAttackTime <= server_time) ||
									(m_nextReadyTime == 0.0f || m_nextReadyTime <= server_time)) {
									uint32_t bak1 = mem::ReadInt(mem::ReadLong(0x14163A880) + 13024);
									uint32_t bak2 = mem::ReadInt(0x1485FB900);
									double bak3 = (double)mem::ReadLong(mem::ReadLong(0x14163A880) + 8456);
									double bak4 = (double)mem::ReadLong(0x14163A8D0);
									double bak5 = (double)mem::ReadLong(0x1423177F0);

									StopSendPacket();

									int32_t current_number = mem::ReadInt(0x14165353C);
									int32_t iDesiredCmdNumber = current_number + 1;
									CUserCmd* old_usercmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * (((uint64_t)iDesiredCmdNumber - 1) % 750)));
									CUserCmd* usercmd = (CUserCmd*)(mem::ReadLong((uint64_t)commands_ptr + 248) + (552 * ((uint64_t)iDesiredCmdNumber % 750)));

									while (mem::ReadInt((long)usercmd) < iDesiredCmdNumber) {
										StopSendPacket();
										std::this_thread::yield();
									}

									//mem::WriteFloat((long)old_usercmd + 0xC, 0.0f);
									//mem::WriteFloat((long)old_usercmd + 0xC + 4, 0.0f);
									mem::WriteFloat((long)old_usercmd + 0xC, *silent_aim_x_ptr);
									mem::WriteFloat((long)old_usercmd + 0xC + 4, *silent_aim_y_ptr);

									//printf("silent x = %f, y = %f\n", *silent_aim_x_ptr, *silent_aim_y_ptr);

									int bef_buttons = mem::ReadInt((long)old_usercmd + 0x38);
									mem::WriteInt((long)old_usercmd + 0x38, bef_buttons | 1);

									mem::WriteInt(mem::ReadLong(0x14163A880) + 13024, bak1);
									mem::WriteInt(0x1485FB900, bak2);
									mem::WriteLong(mem::ReadLong(0x14163A880) + 8456, (long)bak3);
									mem::WriteLong(0x14163A8D0, (long)bak4);
									mem::WriteLong(0x1423177F0, (long)bak5);

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

	printf("START MAIN LOOP!\n");

	for (;;) {
		try {
			localPlayer.markForPointerResolution();
			for (int i = 0; i < players->size(); i++) {
				Player *player = players->at(i);
				player->markForPointerResolution();
			}

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
	mem::WriteInt(mem::ReadLong(0x14163A880) + 13024, 0);
	mem::WriteInt(0x1485FB900, 0);
	mem::WriteLong(mem::ReadLong(0x14163A880) + 8456, (long)DBL_MAX);

	mem::WriteLong(0x14163A8D0, (long)(-DBL_MAX / 2));
	mem::WriteLong(0x1423177F0, (long)(DBL_MAX / 2));
}
