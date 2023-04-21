#pragma once
#include <vector>
#include "LocalPlayer.cpp"
#include "Player.cpp"
#include "Math.cpp"
#include "Level.cpp"
#include "X11Utils.cpp"
#include "ConfigLoader.cpp"

class Sense
{
private:
    Level *m_level;
    LocalPlayer *m_localPlayer;
    std::vector<Player *> *m_players;
    

public:
    Sense(
          Level *level,
          LocalPlayer *localPlayer,
          std::vector<Player *> *players
          )
    {
        
        m_level = level;
        m_localPlayer = localPlayer;
        m_players = players;
        
    }
    void update(int counter)
    {
        if (!m_level->isPlayable())
            return;
        for (int i = 0; i < m_players->size(); i++)         //esp for health
        {
            Player *player = m_players->at(i);
            if (!player->isValid())
                continue;
            if (player->getTeamNumber() == m_localPlayer->getTeamNumber())
                continue;

            //if (player->isVisible()){

                player->setCustomGlow();
                player->setCustomGlowWhite();

                if(player->getShieldValue() <= 10){
                    player->setCustomGlowGreen();
                } else

                if(player->getShieldValue() <= 50){
                    player->setCustomGlowWhite();
                } else

                if(player->getShieldValue() <= 75){
                    player->setCustomGlowBlue();
                } else

                if(player->getShieldValue() <= 100){
                    player->setCustomGlowPurple();
                } else

                if(player->getShieldValue() <= 125){
                    player->setCustomGlowRed();
                }
                
            
            /*}
            else
            {
                player->setCustomGlow();
                player->setCustomGlowRed();
            } */
        }
        
        for (size_t i = 0; i < 15; i++) {
		int index = (counter * 15) + i;
		long ent_base = mem::ReadLong(offsets::REGION + offsets::ENTITY_LIST + ((index + 1) << 5));
		if (ent_base == 0) {
			continue;
		}
		std::string class_name = mem::get_client_class_name(ent_base);

		if (class_name != "CPropSurvival" && class_name != "CWeaponX") {
			continue;
		}      
		int itemID = mem::ReadInt(ent_base + 0x1648);
		}
    }
};
