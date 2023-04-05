#pragma once
#include <cfloat>
#include <vector>
#include "LocalPlayer.cpp"
#include "Player.cpp"
#include "Math.cpp"
#include "Level.cpp"
#include "math.h"

#define AIMBOT_FOV 50
#define AIMBOT_DISTANCE 100

class Aimbot
{
private:
	Level *m_level;
	LocalPlayer *m_localPlayer;
	std::vector<Player *> *m_players;

	Player *m_lockedOnPlayer = nullptr;

	bool *lockedOnPlayer_flag_ptr;
	float *silent_aim_x_ptr;
	float *silent_aim_y_ptr;
public:
	Aimbot(Level *level,
		   LocalPlayer *localPlayer,
		   std::vector<Player *> *players,
		   bool *lockedOnPlayer_flag_ptr,
		   float *silent_aim_x_ptr,
		   float *silent_aim_y_ptr)
	: lockedOnPlayer_flag_ptr(lockedOnPlayer_flag_ptr), silent_aim_x_ptr(silent_aim_x_ptr), silent_aim_y_ptr(silent_aim_y_ptr)
	{
		m_level = level;
		m_localPlayer = localPlayer;
		m_players = players;
	}
	void update()
	{
		if (!m_level->isPlayable())
		{
			m_lockedOnPlayer = nullptr;
			*lockedOnPlayer_flag_ptr = false;
			return;
		}
		if (m_localPlayer->isDead())
		{
			m_lockedOnPlayer = nullptr;
			*lockedOnPlayer_flag_ptr = false;
			return;
		}
		if (m_localPlayer->isKnocked())
		{
			m_lockedOnPlayer = nullptr;
			*lockedOnPlayer_flag_ptr = false;
			return;
		}
		//if (!m_localPlayer->isInAttack())
		//{
		//    m_lockedOnPlayer = nullptr;
		//    *lockedOnPlayer_flag_ptr = false;
		//    return;
		//}

		if (m_level->isTrainingArea())
		{
			int target_index = findClosestEnemyIndex_TrainingArea();
			//printf("target_index = %d\n", target_index);
			if (target_index == -1) {
				*lockedOnPlayer_flag_ptr = false;
				return;
			}
			Player target_npc(target_index);

			execute_all(&target_npc);
		}
		else
		{
			m_lockedOnPlayer = findClosestEnemy();
			if (m_lockedOnPlayer == nullptr) {
				*lockedOnPlayer_flag_ptr = false;
				return;
			}

			execute_all(m_lockedOnPlayer);
		}

		*lockedOnPlayer_flag_ptr = true;
	}

	void execute_all(Player* target) {
		my_vector localHead;
		m_localPlayer->get_camera_pos(&localHead);
		my_vector targetHead;
		//target->get_bone_pos(8, &targetHead);
		target->get_bone_pos(3, &targetHead);

		m_localPlayer->PredictPos(target->getBasePointer(), &targetHead);

		my_vector oldAngle;
		m_localPlayer->get_viewangles(&oldAngle);

		my_vector newAngle = CalcAngle(localHead, targetHead);

		my_vector punchangle;
		my_vector breathangle;
		m_localPlayer->get_punchangles(&punchangle);

		newAngle -= punchangle;

		newAngle.Normalize();

		//m_localPlayer->set_viewangles(&newAngle);//normal aimbot
		
		*silent_aim_x_ptr = newAngle.x;
		*silent_aim_y_ptr = newAngle.y;
		//m_localPlayer->setPitch(newAngle.x);
		//m_localPlayer->setYaw(newAngle.y);
	}
	double calculatePitchAngleDelta(double oldAngle, double newAngle)
	{
		double wayA = newAngle - oldAngle;
		return wayA;
	}
	double calculateAngleDelta(double oldAngle, double newAngle)
	{
		double wayA = newAngle - oldAngle;
		double wayB = 360 - abs(wayA);
		if (wayA > 0 && wayB > 0)
			wayB *= -1 + (double)rand()/(RAND_MAX)+(rand()%4);
		if (abs(wayA) < abs(wayB))
			return wayA;
		return wayB;
	}
	double calculateDesiredYaw(
		double localPlayerLocationX,
		double localPlayerLocationY,
		double enemyPlayerLocationX,
		double enemyPlayerLocationY)
	{
		const double locationDeltaX = enemyPlayerLocationX - localPlayerLocationX;
		const double locationDeltaY = enemyPlayerLocationY - localPlayerLocationY;
		const double yawInRadians = atan2(locationDeltaY, locationDeltaX);
		const double yawInDegrees = yawInRadians * (180 / M_PI);
		return yawInDegrees;
	}
	double calculateDesiredPitch(
		double localPlayerLocationX,
		double localPlayerLocationY,
		double localPlayerLocationZ,
		double enemyPlayerLocationX,
		double enemyPlayerLocationY,
		double enemyPlayerLocationZ)
	{
		const double locationDeltaZ = enemyPlayerLocationZ - localPlayerLocationZ;
		const double distanceBetweenPlayers = math::calculateDistance2D(enemyPlayerLocationX, enemyPlayerLocationY, localPlayerLocationX, localPlayerLocationY);
		const double pitchInRadians = atan2(-locationDeltaZ, distanceBetweenPlayers);
		const double pitchInDegrees = pitchInRadians * (180 / M_PI);
		return pitchInDegrees;
	}
	my_vector CalcAngle(my_vector src, my_vector dst)
	{
		my_vector origin = dst - src;
		float dist = sqrt(origin.x * origin.x + origin.y * origin.y + origin.z * origin.z);
		my_vector angles = { 0.f, 0.f, 0.f };
		angles.x = -asinf(origin.z / dist) * (180 / M_PI);
		angles.y = atan2f(origin.y, origin.x) * (180 / M_PI);
		return angles;
	}
	Player *findClosestEnemy()
	{
		Player *closestPlayer = nullptr;
		float closest2DPlayerDistance = std::numeric_limits<float>::max();
		for (int i = 0; i < m_players->size(); i++)
		{
			Player *player = m_players->at(i);
			if (!player->isValid())
				continue;
			if (player->isKnocked())
				continue;
			if (player->getTeamNumber() == m_localPlayer->getTeamNumber())
				continue;
			//if (!player->isVisible()) /*TODO update*/
			//    continue;
			my_vector lp_head;
			m_localPlayer->get_camera_pos(&lp_head);
			my_vector enem_head;
			player->get_bone_pos(8, &enem_head);
			float dist_lp_enem = lp_head.DistTo(enem_head);
			if (math::distanceToMeters(dist_lp_enem) > AIMBOT_DISTANCE) {
				continue;
			}

			double desiredViewAngleYaw = calculateDesiredYaw(m_localPlayer->getLocationX(),
													  m_localPlayer->getLocationY(),
													  player->getLocationX(),
													  player->getLocationY());
			double desiredViewAnglePitch = calculateDesiredPitch(m_localPlayer->getLocationX(),
														  m_localPlayer->getLocationY(),
														  m_localPlayer->getLocationZ(),
														  player->getLocationX(),
														  player->getLocationY(),
														  player->getLocationZ());
			double pitchAngleDelta = calculatePitchAngleDelta(m_localPlayer->getPitch(), desiredViewAnglePitch);
			double angleDelta = calculateAngleDelta(m_localPlayer->getYaw(), desiredViewAngleYaw);
			double enemy_scr_dist_2d = (pitchAngleDelta * pitchAngleDelta) + (angleDelta * angleDelta);
			if (enemy_scr_dist_2d > AIMBOT_FOV * AIMBOT_FOV) {
				continue;
			}
			if (closestPlayer == nullptr)
			{
				closestPlayer = player;
				closest2DPlayerDistance = enemy_scr_dist_2d;
			}
			else if (enemy_scr_dist_2d < closest2DPlayerDistance)
			{
				closestPlayer = player;
				closest2DPlayerDistance = enemy_scr_dist_2d;
			}
		}
		return closestPlayer;
	}

	int findClosestEnemyIndex_TrainingArea()
	{
		std::vector<Player> npc;

		for (size_t i = 0; i < 20000; i++) {
			long ent_base = mem::ReadLong(offsets::REGION + offsets::ENTITY_LIST + ((i + 1) << 5));
			if (ent_base == 0) {
				continue;
			}
			
			/*long ptrLong = ent_base + offsets::NAME;
			std::string result = mem::ReadString(ptrLong);
			if (result.empty()) {
				continue;
			}*/
			std::string class_name = mem::get_client_class_name(ent_base);
			//printf("[%d]class_name = %s\n", i, class_name.c_str());
			if (class_name != "CAI_BaseNPC") {
				continue;
			}

			npc.push_back(Player(i));
		}

		Player *closestPlayer = nullptr;
		float closestPlayerDistance = std::numeric_limits<float>::max();
		for (int i = 0; i < npc.size(); i++)
		{
			//printf("i = %d\n", i);
			Player* c_npc = &(npc.at(i));
			//if (!c_npc->isValid())
				//continue;
			//if (c_npc->isDead())
				//continue;
			//if (c_npc->isKnocked())
				//continue;
			//if (c_npc->getTeamNumber() == c_npc->getTeamNumber())
				//continue;
			if (!c_npc->isVisible())
				continue;

			my_vector lp_head;
			m_localPlayer->get_camera_pos(&lp_head);
			my_vector npc_head;
			c_npc->get_bone_pos(8, &npc_head);

			float dist_lp_npc = lp_head.DistTo(npc_head);
			if (math::distanceToMeters(dist_lp_npc) > AIMBOT_DISTANCE) {
				continue;
			}

			double desiredViewAngleYaw = calculateDesiredYaw(m_localPlayer->getLocationX(),
													  m_localPlayer->getLocationY(),
													  c_npc->getLocationX(),
													  c_npc->getLocationY());
			double desiredViewAnglePitch = calculateDesiredPitch(m_localPlayer->getLocationX(),
														  m_localPlayer->getLocationY(),
														  m_localPlayer->getLocationZ(),
														  c_npc->getLocationX(),
														  c_npc->getLocationY(),
														  c_npc->getLocationZ());
			double pitchAngleDelta = calculatePitchAngleDelta(m_localPlayer->getPitch(), desiredViewAnglePitch);
			double angleDelta = calculateAngleDelta(m_localPlayer->getYaw(), desiredViewAngleYaw);
			double npc_scr_dist_2d = (pitchAngleDelta * pitchAngleDelta) + (angleDelta * angleDelta);
			if (npc_scr_dist_2d > AIMBOT_FOV * AIMBOT_FOV) {
				continue;
			}
			if (closestPlayer == nullptr)
			{
				closestPlayer = c_npc;
				closestPlayerDistance = npc_scr_dist_2d;
			}
			else if (npc_scr_dist_2d < closestPlayerDistance)
			{
				closestPlayer = c_npc;
				closestPlayerDistance = npc_scr_dist_2d;
			}
		}
		return (closestPlayer != nullptr) ? closestPlayer->get_m_entityListIndex() : -1;
	}
};
