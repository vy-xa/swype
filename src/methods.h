#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "offsets.h"
#include "../GGhost/include/ggh.hpp"
inline uint32_t g_pid = 0;

namespace methods {
    template<typename T>
    inline T read(uint64_t address) {
        T buffer{};
        if (address == 0) return buffer;
        ggh::read(g_pid, address, &buffer, sizeof(T));
        return buffer;
    }
    template<typename T>
    inline void write(uint64_t address, T value) {
        if (address == 0) return;
        ggh::write(g_pid, address, &value, sizeof(T));
    }
    struct vector3_t {
        float x, y, z;
        float dist(const vector3_t& o) const {
            return sqrtf(powf(x - o.x, 2) + powf(y - o.y, 2) + powf(z - o.z, 2));
        }
    };
    struct vector2_t {
        float x, y;
    };
    struct matrix4_t {
        float data[16];
    };
    struct CFrame {
        vector3_t rotation[3];
        vector3_t position;
    };
    inline std::string read_string(uint64_t address) {
        std::string result;
        char buf[128];
        ggh::read(g_pid, address, buf, sizeof(buf));
        for (int i = 0; i < sizeof(buf); i++) {
            if (buf[i] == 0) break;
            result.push_back(buf[i]);
        }
        return result;
    }
    inline std::string read_roblox_string(uint64_t address) {
        int len = read<int>(address + 0x18);
        if (len >= 16) {
            uint64_t ptr = read<uint64_t>(address);
            return read_string(ptr);
        }
        return read_string(address);
    }
    inline vector2_t world_to_screen(const vector3_t& world_pos, const vector2_t& screen_size, const matrix4_t& view_matrix) {
        /*使用行优先矩阵乘法*/
                
        float x = (world_pos.x * view_matrix.data[0]) + (world_pos.y * view_matrix.data[1]) + (world_pos.z * view_matrix.data[2]) + view_matrix.data[3];
        float y = (world_pos.x * view_matrix.data[4]) + (world_pos.y * view_matrix.data[5]) + (world_pos.z * view_matrix.data[6]) + view_matrix.data[7];
        float z = (world_pos.x * view_matrix.data[8]) + (world_pos.y * view_matrix.data[9]) + (world_pos.z * view_matrix.data[10]) + view_matrix.data[11];
        float w = (world_pos.x * view_matrix.data[12]) + (world_pos.y * view_matrix.data[13]) + (world_pos.z * view_matrix.data[14]) + view_matrix.data[15];

        if (w < 0.1f) return { -1, -1 };

        vector3_t ndc;
        ndc.x = x / w;
        ndc.y = y / w;
        ndc.z = z / w;

        vector2_t screen_pos = {
            (screen_size.x / 2 * ndc.x) + (screen_size.x / 2),
            -(screen_size.y / 2 * ndc.y) + (screen_size.y / 2)
        };

        /* 检查边界*/
        if (screen_pos.x < 0 || screen_pos.x > screen_size.x || screen_pos.y < 0 || screen_pos.y > screen_size.y)
            return { -1, -1 };

        return screen_pos;
    }
    class instance_t {
    public:
        uint64_t self;
        instance_t(uint64_t addr = 0) : self(addr) {}
        std::string name() {
            uint64_t ptr = read<uint64_t>(self + offsets::Name);
            return ptr ? read_roblox_string(ptr) : "???";
        }
        std::string class_name() {
            uint64_t desc = read<uint64_t>(self + offsets::ClassDescriptor);
            uint64_t name_ptr = read<uint64_t>(desc + offsets::ClassDescriptorToClassName);
            return name_ptr ? read_roblox_string(name_ptr) : "???";
        }
        std::vector<instance_t> get_children() {
            std::vector<instance_t> res;
            uint64_t children_ptr = read<uint64_t>(self + offsets::Children);
            if (!children_ptr) return res;
            uint64_t start = read<uint64_t>(children_ptr);
            uint64_t end = read<uint64_t>(children_ptr + offsets::ChildrenEnd);
            for (uint64_t ptr = start; ptr < end; ptr += 16) {
                res.emplace_back(read<uint64_t>(ptr));
            }
            return res;
        }
        instance_t find_first_child_of_class(std::string classname) {
            for (auto& child : get_children()) {
                if (child.class_name() == classname) return child;
            }
            return instance_t(0);
        }
        vector3_t get_position() {
            uint64_t primitive = read<uint64_t>(self + offsets::Primitive);
            return primitive ? read<vector3_t>(primitive + offsets::Position) : vector3_t{};
        }
        instance_t get_character() {
            return instance_t(read<uint64_t>(self + offsets::ModelInstance));
        }
        float get_health() {
            return read<float>(self + offsets::Health);
        }
        float get_max_health() {
            return read<float>(self + offsets::MaxHealth);
        }
        instance_t get_local_player() {
            return instance_t(read<uint64_t>(self + offsets::LocalPlayer));
        }
        instance_t get_camera() {
            return instance_t(read<uint64_t>(self + offsets::Camera));
        }
        vector3_t get_camera_pos() {
            return read<vector3_t>(self + offsets::CameraPos);
        }
        matrix4_t get_view_matrix() {
            return read<matrix4_t>(self + offsets::viewmatrix);
        }
        vector2_t get_dimensions() {
            return read<vector2_t>(self + offsets::Dimensions);
        }
    };
}