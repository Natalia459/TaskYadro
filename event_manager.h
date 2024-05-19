#pragma once
#include "parser.h"
#include "objects.h"

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <deque>


class EventManager {
public:
	EventManager() = default;
	EventManager(ClubEvent&& client_action);
	EventManager(const ComputerClub& info);

	void StartWorking() const;

	void FinishWorking();

	void SetEvent(ClubEvent&& client_action);

private:
	std::unordered_map<int, TableInfo> tables_;
	std::unordered_map<std::string, ClubEvent> clients_;

	std::unordered_set<std::string_view> waiting_clients_;
	std::deque<std::string> queue_;

	ComputerClub club_info_;

	void LeaveTableSaveGain(int table_num, int leave_time);

	void OccupyTable(int table_num, int coming_time);

	void LeaveQueue(std::string_view client_name);

	void CheckIfTablesOccupied();

	std::string ConvertTime(int time) const;

	void GenerateError(int time, std::string_view message) const;

	void OutputIdMessage(OutputId ch) const;

	void InputIdMessage(InputId ch) const;

	void SendGainMessage();
	
	void SendMessage(int time, OutputId ch, std::string_view message) const;

	void SendMessage(int time, InputId ch, std::string_view message) const;

	void ProcessId1(const ClubEvent& client_action);

	void ProcessId2(const ClubEvent& client_action);

	void ProcessId3(const ClubEvent& client_action);

	void ProcessId4(const ClubEvent& client_action);
};