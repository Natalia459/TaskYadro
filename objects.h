#pragma once
#include <string>
#include <optional>
#include <memory>

enum class InputId {
	ID1,
	ID2,
	ID3,
	ID4
};

enum class OutputId {
	ID11,
	ID12,
	ID13
};

struct ClubEvent {
	int time = 0;
	int id = 0;
	std::string client_name = "";
	std::optional<int> table;

	bool operator==(const ClubEvent& other) const {
		bool first = client_name == other.client_name && time == other.time;
		bool second = id == other.id && table == other.table;
		return first && second; 
	}
};

struct ClubEventHash {
	size_t operator()(const ClubEvent& client) const {
		return std::hash<int>{}(client.time) ^ std::hash<int>{}(client.id) + std::hash<std::optional<int>>{}(client.table);
	}
};

struct ComputerClub {
	int common_table_count = 0;
	int open_time = 0;
	int close_time = 0;
	double cost = 0;
	int busy_tables = 0;
};

struct TableInfo {
	bool is_busy = 0;
	double gain = 0;
	int time_occupied = 0;
	int time_last_coming = 0;
};