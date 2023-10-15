#pragma once
#include "../plugin_sdk/plugin_sdk.hpp"
namespace leblanc
{
	void load();
	void unload();
	inline float getPing();
	inline void on_update();
	inline void on_draw();
	inline void on_env_draw();
	inline void chainOnCCLoop();
	inline void updateUltimateSpell();
	inline void harass_target();
	inline void flee();
	inline bool two_chains(game_object_script& target);
	inline void on_cast_spell(spellslot spellSlot, game_object_script target, vector& pos, vector& pos2, bool isCharge, bool* process);
	inline void on_process_spell_cast(game_object_script sender, spell_instance_script spell);

};
