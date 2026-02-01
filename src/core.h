#pragma once
#include "methods.h"
#include "console.h"
#include <thread>

namespace core {
    using namespace methods;
    std::vector<instance_t> cached_npcs;
    void scan_for_rigs(instance_t& parent, std::vector<instance_t>& rigs, uint64_t local_char_addr, int depth = 0) {
        if (depth > 6) return;
        for (auto& child : parent.get_children()) {
            if (!child.self) continue;
            if (child.self == local_char_addr) continue;
            instance_t hum = child.find_first_child_of_class("Humanoid");
            if (hum.self && hum.get_health() > 0) {
                bool has_head = false, has_root = false;
                for (auto& part : child.get_children()) {
                    std::string n = part.name();
                    if (n == "Head") has_head = true;
                    else if (n == "HumanoidRootPart" || n == "Torso") has_root = true;
                    if (has_head && has_root) break;
                }
                if (has_head && has_root) {
                    rigs.push_back(child);
                    continue;
                }
            }
            std::string cn = child.class_name();
            if (cn != "Part" && cn != "MeshPart" && cn != "UnionOperation" &&
                cn != "Decal" && cn != "Texture" && cn != "Script" &&
                cn != "LocalScript" && cn != "ModuleScript" && cn != "Sound" &&
                cn != "ParticleEmitter" && cn != "PointLight" && cn != "SpotLight" &&
                cn != "SurfaceLight" && cn != "Fire" && cn != "Smoke" && cn != "Sparkles") {
                scan_for_rigs(child, rigs, local_char_addr, depth + 1);
            }
        }
    }
    void init() {
        setup_console();
        ggh::init();
        std::cout << "waiting roblox....." << std::endl;
        while (true) {
            g_pid = ggh::get_pid(L"RobloxPlayerBeta.exe");
            if (g_pid) break;
            ggh::sleep(1000);
        }
        std::cout << "  found: " << g_pid << std::endl;

        uint64_t fake_dm = read<uint64_t>(ggh::get_module_base(g_pid) + offsets::FakeDataModelPointer);
        uint64_t datamodel_addr = read<uint64_t>(fake_dm + offsets::FakeDataModelToDataModel);
        instance_t datamodel(datamodel_addr);
        instance_t workspace = datamodel.find_first_child_of_class("Workspace");
        instance_t players = datamodel.find_first_child_of_class("Players");


        std::cout << "thread started" << std::endl;

        bool active = false;
        instance_t locked_target(0);
        bool last_key_state = false;
        uint64_t last_scan_start = 0;
        std::vector<instance_t> scan_queue;
        size_t scan_index = 0;

        while (true) {
            if (!ggh::get_pid(L"RobloxPlayerBeta.exe")) {
                system("cls");
                std::cout << "waiting roblox ..." << std::endl;
                while (true) {
                    g_pid = ggh::get_pid(L"RobloxPlayerBeta.exe");
                    if (g_pid) break;
                    ggh::sleep(1000);
                }
                std::cout << "roblox found: " << g_pid << std::endl;
                fake_dm = read<uint64_t>(ggh::get_module_base(g_pid) + offsets::FakeDataModelPointer);
                datamodel_addr = read<uint64_t>(fake_dm + offsets::FakeDataModelToDataModel);
                datamodel.self = datamodel_addr;
                workspace = datamodel.find_first_child_of_class("Workspace");
                players = datamodel.find_first_child_of_class("Players");
                std::cout << "thread started" << std::endl;
                locked_target = instance_t(0);
                active = false;
                last_scan_start = 0;
                scan_queue.clear();
                scan_index = 0;
            }
            uint64_t now_tick = GetTickCount64();
            if (now_tick - last_scan_start > 2000) { // for the every second 2s start new pass
                scan_queue.clear();
                if (workspace.self) scan_queue.push_back(workspace);
                scan_index = 0;
                cached_npcs.clear();
                last_scan_start = now_tick;
            }
            size_t budget = 64;
            while (budget-- && scan_index < scan_queue.size()) {
                instance_t node = scan_queue[scan_index++];
                instance_t hum = node.find_first_child_of_class("Humanoid");
                if (hum.self && hum.get_health() > 0) {
                    bool has_head = false, has_root = false;
                    for (auto& part : node.get_children()) {
                        std::string n = part.name();
                        if (n == "Head") has_head = true;
                        else if (n == "HumanoidRootPart" || n == "Torso") has_root = true;
                        if (has_head && has_root) break;
                    }
                    if (has_head && has_root) {
                        instance_t local_player = players.get_local_player();
                        instance_t lp_char = local_player.self ? local_player.get_character() : instance_t(0);
                        if (node.self != lp_char.self) cached_npcs.push_back(node);
                    }
                }
                for (auto& ch : node.get_children()) {
                    scan_queue.push_back(ch);
                }
            }
            bool key_down = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
            if (key_down && !last_key_state) {
                active = !active;
                if (!active) {
                    locked_target = instance_t(0);
                    std::cout << "[-] Disabled" << std::endl;
                } else {
                    std::cout << "[+] Enabled - Searching for lock..." << std::endl;
                }
                std::cout.flush();
            }
            last_key_state = key_down;

            if (active) {
                instance_t local_player = players.get_local_player();
                instance_t lp_char = local_player.self ? local_player.get_character() : instance_t(0);
                instance_t camera = workspace.get_camera();

                if (local_player.self && camera.self && lp_char.self) {
                    uint64_t ve = read<uint64_t>(ggh::get_module_base(g_pid) + offsets::VisualEnginePointer);
                    matrix4_t view_matrix = read<matrix4_t>(ve + offsets::viewmatrix);
                    vector2_t screen_res = read<vector2_t>(ve + offsets::Dimensions);

                    POINT mouse;
                    GetCursorPos(&mouse);
                    HWND game_hwnd = FindWindowA(NULL, "Roblox");
                    if (game_hwnd) ScreenToClient(game_hwnd, &mouse);
                    if (!locked_target.self) {
                        float min_dist = 999999.0f;
                        instance_t best_candidate(0);
                        //players
                        for (auto& plr : players.get_children()) {
                            if (plr.self == local_player.self) continue;
                            instance_t character = plr.get_character();
                            if (!character.self) continue;
                            instance_t hum = character.find_first_child_of_class("Humanoid");
                            if (!hum.self || hum.get_health() <= 0) continue;
                            instance_t root(0);
                            for (auto& ch : character.get_children()) {
                                std::string n = ch.name();
                                if (n == "HumanoidRootPart" || n == "Torso") { root = ch; break; }
                            }
                            if (!root.self) continue;
                            vector3_t pos = root.get_position();
                            vector2_t w2s = world_to_screen(pos, screen_res, view_matrix);
                            if (w2s.x != -1) {
                                float dist = sqrtf(powf(w2s.x - mouse.x, 2) + powf(w2s.y - mouse.y, 2));
                                if (dist < min_dist && dist < 300) {
                                    min_dist = dist;
                                    best_candidate = character;
                                }
                            }
                        }
                        //npcs
                        std::vector<instance_t> npcs_copy = cached_npcs;
                        for (auto& npc : npcs_copy) {
                            instance_t hum = npc.find_first_child_of_class("Humanoid");
                            if (!hum.self || hum.get_health() <= 0) continue;
                            instance_t root(0);
                            for (auto& ch : npc.get_children()) {
                                std::string n = ch.name();
                                if (n == "HumanoidRootPart" || n == "Torso") { root = ch; break; }
                            }
                            if (!root.self) continue;
                            vector3_t pos = root.get_position();
                            vector2_t w2s = world_to_screen(pos, screen_res, view_matrix);
                            if (w2s.x != -1) {
                                float dist = sqrtf(powf(w2s.x - mouse.x, 2) + powf(w2s.y - mouse.y, 2));
                                if (dist < min_dist && dist < 300) {
                                    min_dist = dist;
                                    best_candidate = npc;
                                }
                            }
                        }
                        if (best_candidate.self) {
                            locked_target = best_candidate;
                            std::cout << "[+] LOCKED onto: " << locked_target.name() << std::endl;
                        }
                    }
                    if (locked_target.self) {
                        instance_t hum = locked_target.find_first_child_of_class("Humanoid");
                        if (!hum.self || hum.get_health() <= 0) {
                            std::cout << "[-] Target dead/lost" << std::endl;
                            locked_target = instance_t(0);
                        } else {
                            bool on_target = false;
                            bool is_r15 = false;
                            std::vector<instance_t> body_parts;

                            for (auto& ch : locked_target.get_children()) {
                                std::string n = ch.name();
                                if (n == "UpperTorso" || n == "LowerTorso") is_r15 = true;
                            }
                            for (auto& ch : locked_target.get_children()) {
                                std::string n = ch.name();
                                if (is_r15) {
                                    if (n == "Head" || n == "UpperTorso" || n == "LowerTorso" ||
                                        n == "LeftUpperArm" || n == "LeftLowerArm" || n == "LeftHand" ||
                                        n == "RightUpperArm" || n == "RightLowerArm" || n == "RightHand" ||
                                        n == "LeftUpperLeg" || n == "LeftLowerLeg" || n == "LeftFoot" ||
                                        n == "RightUpperLeg" || n == "RightLowerLeg" || n == "RightFoot") {
                                        body_parts.push_back(ch);
                                    }
                                } else {
                                    if (n == "Head" || n == "Torso" ||
                                        n == "Left Arm" || n == "Right Arm" ||
                                        n == "Left Leg" || n == "Right Leg") {
                                        body_parts.push_back(ch);
                                    }
                                }
                            }
                            for (auto& part : body_parts) {
                                vector3_t pos = part.get_position();
                                vector2_t screen_pos = world_to_screen(pos, screen_res, view_matrix);
                                if (screen_pos.x == -1) continue;
                                std::string pn = part.name();
                                float radius = 12.0f;
                                if (pn == "Head") radius = 14.0f;
                                else if (pn == "Torso" || pn == "UpperTorso" || pn == "LowerTorso") radius = 18.0f;
                                else if (pn.find("Arm") != std::string::npos || pn.find("Hand") != std::string::npos) radius = 10.0f;
                                else if (pn.find("Leg") != std::string::npos || pn.find("Foot") != std::string::npos) radius = 12.0f;

                                float dist = sqrtf(powf(screen_pos.x - mouse.x, 2) + powf(screen_pos.y - mouse.y, 2));
                                if (dist <= radius) {
                                    on_target = true;
                                    break;
                                }
                            }
                            if (on_target) {
                                static uint64_t last_fire = 0;
                                uint64_t now = GetTickCount64();
                                if (now - last_fire > 30) {
                                    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                                    last_fire = now;
                                }
                            }
                        }
                    }
                }
            }
            ggh::sleep(1);
        }
    }
}
