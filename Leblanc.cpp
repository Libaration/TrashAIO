#include "../plugin_sdk/plugin_sdk.hpp"
#include "Helpers.h"
#include "Permashow.hpp"
#include "Others.h"
#include "Leblanc.h"

namespace leblanc
{
	void on_update();
	void on_draw();
	void on_env_draw();

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


	/*****************************************
					LOAD/UNLOAD
	****************************************/

	void load()
	{
		// Registering a spells
		q = plugin_sdk->register_spell(spellslot::q, 699);
		w = plugin_sdk->register_spell(spellslot::w, 600);
		e = plugin_sdk->register_spell(spellslot::e, 950);
		r = plugin_sdk->register_spell(spellslot::r, 300);
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

	void two_chains(game_object_script& target) {
		bool wait_for_root = misc_menu::wait_for_root->get_bool();
		auto chainMimicAvailable = myhero->get_spell(spellslot::r)->get_name() == "LeblancRE";
		bool targetIsChained = target->has_buff(buff_hash("LeblancE")) || target->has_buff(buff_hash("LeblancRE"));
		bool targetIsRooted = target->can_move();

		if (!wait_for_root) {
			console->print("%s", "wait for root is false beginning combo");
			if (chainMimicAvailable && !e->is_ready()) {
				r->cast(target, hit_chance::high);
				targetIsChained&& e->cast(target, hit_chance::high);
			}
			else {
				console->print("%s", "chaimMimic Not Available casting E as normal");
				e->cast(target, hit_chance::high);
				targetIsChained&& r->cast(target, hit_chance::high);
			}
		}



	}

	void harass_target(game_object_script& target) {
		if (target == nullptr || target_selector->has_spellshield(target)) {
			return;
		}
		const auto health = target->get_health_percent();
		const auto targetDistance = target->get_distance(myhero);
		const bool targetIsMarked = target->has_buff(buff_hash("LeblancQMark"));
		const bool canJumpBack = myhero->has_buff(buff_hash("LeblancW"));
		two_chains(target);
	}

	void flee() {
		auto mouse_position = hud->get_hud_input_logic()->get_game_cursor_position();
		const bool canJumpBack = myhero->has_buff(buff_hash("LeblancW"));
		const bool canJumpBackMimic = myhero->has_buff(buff_hash("LeblancRW"));


		if (w->is_ready() || r->is_ready()) {
			auto pathfindingDirection = myhero->get_position().extend(mouse_position, 600.f);
			if (!canJumpBack) {
				w->cast(pathfindingDirection);
			}
			if (!canJumpBackMimic) {
				r->cast(pathfindingDirection);
			}
		}
	}

	void on_update()
	{
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
		if (myhero->is_dead()) return;
		if (orbwalker->mixed_mode()) {
			auto target = target_selector->get_target(q->range(), damage_type::magical);
			harass_target(target);
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
