#include "Helpers.h"
#include "Permashow.hpp"
#include "Others.h"
#include "Leblanc.h"
#include <cstring>
#include <string>

namespace leblanc
{
	struct State {
		float lastCast;
		float lastECastTime;
		float lastRCastTime;
		bool hasCasted;
	};
	State state = { 0, false };


	/*****************************************
					VARIABLES
	****************************************/

	script_spell* q = nullptr;
	script_spell* w = nullptr;
	script_spell* e = nullptr;
	script_spell* r = nullptr;
	TreeTab* main_tab = nullptr;

	/*****************************************
					MENU STUFF
	****************************************/

	namespace combo
	{
		// Q Settings
		TreeEntry* use_q = nullptr;
		TreeEntry* min_q = nullptr;

		// W Settings
		TreeEntry* use_w = nullptr;
		TreeEntry* w_mana = nullptr;

		// E Settings
		TreeEntry* use_e = nullptr;

		// R Settings
		TreeEntry* hit_r = nullptr;
	}

	namespace misc_menu
	{
		//Two Chains Settings
		TreeEntry* wait_for_root = nullptr;
	}

	namespace harass
	{
		// Q Settings
		TreeEntry* use_q = nullptr;
		TreeEntry* min_q = nullptr;

		// W Settings
		TreeEntry* use_w = nullptr;
		TreeEntry* w_mana = nullptr;

		// E Settings
		TreeEntry* use_e = nullptr;
	}

	namespace farm_menu
	{
		TreeEntry* toggle = nullptr;
	}

	namespace lane_clear_menu
	{
		// W Settings
		TreeEntry* use_w = nullptr;
		TreeEntry* hit_w = nullptr;
		TreeEntry* w_mana = nullptr;

		// E Settings
		TreeEntry* use_e = nullptr;
		TreeEntry* hit_e = nullptr;
	}

	namespace jungle_clear_menu
	{
		// Q Settings
		TreeEntry* use_q = nullptr;

		// W Settings
		TreeEntry* use_w = nullptr;
		TreeEntry* w_mana = nullptr;

		// E Settings
		TreeEntry* use_e = nullptr;
	}

	namespace ac
	{
		// E Settings
		TreeEntry* auto_e_kill = nullptr;
	}


	namespace q_pred
	{
		TreeEntry* hc = nullptr;
		TreeEntry* semi_q_hc = nullptr;
	}

	namespace r_pred
	{
		TreeEntry* range_slider = nullptr;
	}

	namespace draws
	{
		TreeEntry* draw_q = nullptr;
		TreeEntry* draw_w = nullptr;
		TreeEntry* draw_e = nullptr;
		TreeEntry* draw_r = nullptr;
	}

	namespace dmg_draws
	{
		TreeEntry* draw_damage = nullptr;
		TreeEntry* draw_q = nullptr;
		TreeEntry* draw_w = nullptr;
		TreeEntry* draw_e = nullptr;
		TreeEntry* draw_r = nullptr;
		TreeEntry* w_ticks = nullptr;
	}

	namespace mainMenu
	{
		TreeEntry* semiq = nullptr;
	}
	float getPing()
	{
		return ping->get_ping() / 1000;
	}
	void on_cast_spell(spellslot spellSlot, game_object_script target, vector& pos, vector& pos2, bool isCharge, bool* process)
	{
		state.lastCast = gametime->get_time() + 0.133 + getPing();
	}

	void on_process_spell_cast(game_object_script sender, spell_instance_script spell)
	{
		if (sender->get_handle() == myhero->get_handle()) state.lastCast = 0;
	}

	void setState() {
		// reset hasCasted on every update
		state.hasCasted = false;
	}
	/*****************************************
					LOAD/UNLOAD
	****************************************/

	void load()
	{
		// Registering a spells
		q = plugin_sdk->register_spell(spellslot::q, 699);
		w = plugin_sdk->register_spell(spellslot::w, 600);
		e = plugin_sdk->register_spell(spellslot::e, 950);
		r = plugin_sdk->register_spell(spellslot::r, 600);
		q->set_spell_lock(false);
		w->set_spell_lock(false);
		e->set_spell_lock(false);
		r->set_spell_lock(false);


		w->set_skillshot(0, 240, 1450, {}, skillshot_type::skillshot_circle);
		e->set_skillshot(0.25f, 80.f, 2000.f, { collisionable_objects::minions, collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);

		// Create menu
		main_tab = menu->create_tab(MENU_KEY + myhero->get_model(), AIO_NAME " - " + myhero->get_model());
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());

		const auto draw_settings = main_tab->add_tab("draw", "Drawings");
		{
			draw_settings->add_separator("ranges", "-- Ranges --");
			draws::draw_q = draw_settings->add_checkbox("draw_q", "Draw Q Range", false);
			draws::draw_w = draw_settings->add_checkbox("draw_w", "Draw W Range", false);
			draws::draw_e = draw_settings->add_checkbox("draw_e", "Draw E Range", false);
			draws::draw_r = draw_settings->add_checkbox("draw_r", "Draw R Range", false);
			draw_settings->add_separator("other", "-- Other --");
			const auto damage_settings = draw_settings->add_tab("damage", "Damage Drawings");
			{
				dmg_draws::draw_damage = damage_settings->add_checkbox("draw_damage", "Draw Damage", true);
				damage_settings->add_separator("empty", "");
				dmg_draws::draw_q = damage_settings->add_checkbox("draw_q", "Draw Q Damage", true);
				dmg_draws::draw_w = damage_settings->add_checkbox("draw_w", "Draw W Damage", true);
				dmg_draws::draw_e = damage_settings->add_checkbox("draw_e", "Draw E Damage", true);
				dmg_draws::draw_r = damage_settings->add_checkbox("draw_r", "Draw R Damage", true);
			}
		}

		const auto misc_settings = main_tab->add_tab("misc", "Misc");
		{
			misc_settings->add_separator("two_chains_settings", "-- Two Chains Combo --");
			misc_menu::wait_for_root = misc_settings->add_checkbox("wait_for_root", "Wait for root before E Mimic", true);

		}

		misc().init();
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_env_draw);
		event_handler<events::on_draw>::add_callback(on_draw);
		event_handler<events::on_cast_spell>::add_callback(on_cast_spell, event_prority::high);
		event_handler<events::on_process_spell_cast>::add_callback(on_process_spell_cast, event_prority::high);

	}

	void unload()
	{
		// Unregister spells
		plugin_sdk->remove_spell(q);
		plugin_sdk->remove_spell(w);
		plugin_sdk->remove_spell(e);
		plugin_sdk->remove_spell(r);

		// Delete menu
		menu->delete_tab(MENU_KEY + myhero->get_model());

		// Unregister callbacks
		event_handler<events::on_preupdate>::remove_handler(on_update);
		event_handler<events::on_preupdate>::remove_handler(on_env_draw);
		event_handler<events::on_preupdate>::remove_handler(on_draw);
		event_handler<events::on_cast_spell >::remove_handler(on_cast_spell);
		event_handler<events::on_process_spell_cast >::remove_handler(on_process_spell_cast);


		// Destroy permashow
		misc().destroy();
	}

	/*Damages*/

	float q_damages[] = { 65.f , 90.f , 115.f , 140.f , 165.f };

	float q_damage(const game_object_script& target)
	{
		if (q->level() == 0) return 0.f;

		damage_input input;
		input.raw_magical_damage = q_damages[q->level() - 1] + myhero->get_total_ability_power() * 0.40f;

		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);
		return damage;
	}

	float w_damages[] = { 75.f, 115.f, 155.f, 195.f, 235.f };

	float w_damage(const game_object_script& target)
	{
		if (w->level() == 0) return 0.f;

		damage_input input;
		input.raw_magical_damage = w_damages[w->level() - 1] + myhero->get_total_ability_power() * 0.65f;
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);

		return damage;
	}


	float e_damages[] = { 50.f, 70.f, 90.f, 110.f, 130.f };

	float e_damage(const game_object_script& target)
	{
		if (w->level() == 0) return 0.f;

		damage_input input;
		input.raw_magical_damage = e_damages[e->level() - 1] + myhero->get_total_ability_power() * 0.35f;
		const float damage = damagelib->calculate_damage_on_unit(myhero, target, &input);

		return damage;
	}


	void qe_combo(game_object_script& target, bool targetIsMarked) {
		if (!targetIsMarked && q->is_ready()) {
			q->cast(target);
		}
		else if (targetIsMarked && e->is_ready()) {
			e->cast(target, hit_chance::high);
		}
	}
	void qw_combo(game_object_script& target, bool targetIsMarked, float health, bool canJumpBack) {
		if (!canJumpBack) {
			if (!targetIsMarked && q->is_ready()) {
				q->cast(target);
			}
			w->is_ready() && w->cast(target, hit_chance::high);

			if (health >= 20 && canJumpBack) {
				w->is_ready() && w->cast();
			}
		}
	}

	bool two_chains(game_object_script& target) {
		bool waitForRoot = misc_menu::wait_for_root->get_bool();
		auto chainMimicAvailable = myhero->get_spell(spellslot::r)->get_name_hash() == spell_hash("LeblancRE");
		auto ultAvailableNoMimicChosen = myhero->get_spell(spellslot::r)->get_name_hash() == spell_hash("LeblancR");
		auto timeRemainingOnE = target->get_buff_time_left(buff_hash("LeblancE"));
		auto timeRemainingOnRE = target->get_buff_time_left(buff_hash("LeblancRE"));
		bool targetIsChained = target->has_buff(buff_hash("LeblancE"));


		if (waitForRoot) {
			double distanceToTarget = myhero->get_distance(target->get_position());
			double baseCastingTime = e->delay + 0.133 + getPing();

			double curveFactor;

			if (distanceToTarget < 400) {
				double exponent = 6.0;  // Adjust this exponent to fine-tune the curve
				curveFactor = 5.0 - pow(distanceToTarget / 400.0, exponent);
			}
			else if (distanceToTarget < 800) {
				double exponent = 5.0;  // Adjust this exponent to fine-tune the curve
				curveFactor = 0.5 - pow((distanceToTarget - 400) / 400.0, exponent);
			}
			else {
				double exponent = 5.0;  // Adjust this exponent to fine-tune the curve
				curveFactor = 0.5 - pow((distanceToTarget - 800) / 150.0, exponent);
			}

			baseCastingTime += curveFactor * (distanceToTarget / e->speed);

			if (state.lastECastTime == 0) {
				e->cast(target, hit_chance::high);
				if (targetIsChained) {
					state.lastECastTime = gametime->get_time() + baseCastingTime;
				}
			}
			else if (gametime->get_time() - state.lastECastTime > 0.15) {
				if (r->is_ready() && ultAvailableNoMimicChosen) {
					targetIsChained&& r->cast(target, hit_chance::high);
					state.lastECastTime = 0;
					state.hasCasted = true;
				}
				else if (r->is_ready() && chainMimicAvailable) {
					targetIsChained&& r->cast(target, hit_chance::high);
					state.lastECastTime = 0;
					state.hasCasted = true;
				}
				else if (r->is_ready() && (!chainMimicAvailable || !ultAvailableNoMimicChosen)) {
					state.lastECastTime = 0;
				}
			}
		}
		return false;
	}




	void harass_target() {
		if (state.hasCasted || state.lastCast > gametime->get_time()) return;
		auto target = target_selector->get_target(e->range(), damage_type::magical);
		if (target == nullptr || target_selector->has_spellshield(target)) {
			return;
		}
		const auto health = target->get_health_percent();
		const auto targetDistance = target->get_distance(myhero);
		const bool targetIsMarked = target->has_buff(buff_hash("LeblancQMark"));
		const bool canJumpBack = myhero->has_buff(buff_hash("LeblancW"));
		two_chains(target);
		return;
	}

	void flee() {
		auto mouse_position = hud->get_hud_input_logic()->get_game_cursor_position();
		bool canJumpBack = myhero->has_buff(buff_hash("LeblancW"));
		bool canJumpBackMimic = myhero->has_buff(buff_hash("LeblancRW"));


		if (w->is_ready() || r->is_ready()) {
			auto pathfindingDirection = myhero->get_position().extend(mouse_position, 600.f);
			if (!canJumpBack) {
				w->is_ready() && w->cast(pathfindingDirection);
				canJumpBack = true;
			}
			if (!canJumpBackMimic) {
				r->is_ready() && r->cast(pathfindingDirection);
				canJumpBackMimic = true;
			}
		}
	}

	void updateUltimateSpell() {
		if (r->is_ready()) {
			auto spell = myhero->get_spell(spellslot::r);
			switch (spell->get_name_hash()) {
			case spell_hash("LeblancRQ"):
				r->set_range(q->range());
				break;
			case spell_hash("LeblancRW"):
				r->set_range(w->range());
				r->set_skillshot(0, 240, 1450, {}, skillshot_type::skillshot_circle);
				break;
			case spell_hash("LeblancRE"):
				r->set_range(e->range());
				r->set_skillshot(0.25f, 80.f, 2000.f, { collisionable_objects::minions, collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);
				break;
			}
		}
	}

	void chainOnCCLoop() {
		for (auto&& target : entitylist->get_enemy_heroes())
		{
			if (target != nullptr && myhero->get_distance(target->get_position()) <= e->range()) {
				auto timeLeft = target->get_immovibility_time();
				auto isStunnedByMe = !target->has_buff(buff_hash("LeblancE")) || !target->has_buff(buff_hash("LeblancRE"));
				if (timeLeft > 0 && !isStunnedByMe) {
					e->is_ready() && e->cast(target, hit_chance::high);
				}
			}
		}
	}
	void on_update()
	{
		if (myhero->is_dead()) return;
		setState();
		updateUltimateSpell();
		/*	chainOnCCLoop();*/
		if (orbwalker->mixed_mode()) {
			harass_target();
		}


		if (orbwalker->flee_mode()) {
			flee();
			return;
		}
	}

	void on_draw()
	{
		if (myhero->is_dead()) return;

		for (auto&& enemy : entitylist->get_enemy_heroes())
		{
			if (enemy == nullptr || !enemy->is_valid_target()) continue;

			if (dmg_draws::draw_damage->get_bool() && enemy->is_visible_on_screen())
			{
				float q_dmg = 0.f;
				if (q->is_ready() && (w->is_ready() || e->is_ready() || r->is_ready())) {
					q_dmg = q_damage(enemy) * 2;
				}
				else {
					q_dmg = q->is_ready() ? q_damage(enemy) : 0.f;
				}
				float w_dmg = w->is_ready() ? w_damage(enemy) : 0.f;
				float e_dmg = e->is_ready() ? e_damage(enemy) : 0.f;
				float r_dmg = 0.f;

				if (!dmg_draws::draw_q->get_bool()) q_dmg = 0.f;
				if (!dmg_draws::draw_e->get_bool()) e_dmg = 0.f;
				if (!dmg_draws::draw_w->get_bool()) w_dmg = 0.f;
				if (!dmg_draws::draw_r->get_bool()) r_dmg = 0.f;

				const float total = q_dmg + e_dmg + w_dmg + r_dmg;
				helpers::draw_damage(enemy, total);
			}
		}
	}

	void on_env_draw() {

		if (draws::draw_q->get_bool())
		{
			helpers::draw_range(myhero->get_position(), misc().color_q->get_color(), misc().color_q2->get_color(), q->range(), misc().thicc->get_int(), helpers::get_glow());
		}
		if (draws::draw_w->get_bool())
		{
			helpers::draw_range(myhero->get_position(), misc().color_w->get_color(), misc().color_w2->get_color(), w->range(), misc().thicc->get_int(), helpers::get_glow());
		}
		if (draws::draw_e->get_bool())
		{
			helpers::draw_range(myhero->get_position(), misc().color_e->get_color(), misc().color_e2->get_color(), e->range(), misc().thicc->get_int(), helpers::get_glow());
		}

	}

}
