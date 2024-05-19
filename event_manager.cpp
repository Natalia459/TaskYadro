#include "event_manager.h"

#include <iostream>
#include <algorithm>
#include <vector>

EventManager::EventManager(ClubEvent&& client_action) {
	queue_.push_back(client_action.client_name);
	waiting_clients_.insert(client_action.client_name);
	clients_.insert({ client_action.client_name, std::move(client_action) });
}
EventManager::EventManager(const ComputerClub& info)
	:club_info_(info)
{}

void EventManager::StartWorking() const {
	std::cout << ConvertTime(club_info_.open_time) << std::endl;
}

void EventManager::FinishWorking() {
	CheckIfTablesOccupied();
	SendGainMessage();
}

void EventManager::SetEvent(ClubEvent&& client_action) {

	switch (client_action.id) {
	case 1:
		SendMessage(client_action.time, InputId::ID1, client_action.client_name);
		ProcessId1(client_action);
		break;
	case 2:
		SendMessage(client_action.time, InputId::ID2, client_action.client_name + " " + std::to_string(client_action.table.value()));
		ProcessId2(client_action);
		break;
	case 3:
		SendMessage(client_action.time, InputId::ID3, client_action.client_name);
		ProcessId3(client_action);
		break;
	case 4:
		SendMessage(client_action.time, InputId::ID4, client_action.client_name);
		ProcessId4(client_action);
		break;
	default:
		std::cout << "Unknown id" << std::endl;
	}
}


void EventManager::LeaveTableSaveGain(int table_num, int leave_time) {
	tables_[table_num].is_busy = false;

	int time_occupied = leave_time - tables_[table_num].time_last_coming;
	tables_[table_num].gain += club_info_.cost * (static_cast<int>(time_occupied / 60) + ((time_occupied % 60 == 0) ? 0 : 1));
	tables_[table_num].time_occupied += time_occupied;
	tables_[table_num].time_last_coming = leave_time;
	--club_info_.busy_tables;
}

void EventManager::OccupyTable(int table_num, int coming_time) {
	tables_[table_num].is_busy = true;
	tables_[table_num].time_last_coming = coming_time;
	++club_info_.busy_tables;
}

void EventManager::LeaveQueue(std::string_view client_name) {
	if (!queue_.empty()) {
		if (queue_.front() != client_name) {
			auto pos = std::find(queue_.begin(), queue_.end(), static_cast<std::string>(client_name));
			queue_.erase(pos);
		}
		else {
			queue_.pop_front();
		}
	}
	waiting_clients_.erase(client_name);
}

void EventManager::CheckIfTablesOccupied() {
	if (club_info_.busy_tables != 0) {
		std::vector<std::string> stayed_clients;
		stayed_clients.reserve(clients_.size());
		for (auto& [client_name, action] : clients_) {
			stayed_clients.push_back(client_name);
		}
		std::sort(stayed_clients.begin(), stayed_clients.end());
		for (auto& client_name : stayed_clients) {
			if (clients_[client_name].table.has_value()) {
				LeaveTableSaveGain(clients_[client_name].table.value(), club_info_.close_time);
				SendMessage(club_info_.close_time, OutputId::ID11, client_name);
			}
		}
	}
}

std::string EventManager::ConvertTime(int time) const {
	int hours = time / 60;
	int minutes = time - hours * 60;
	std::string h = ((hours < 10) ? "0" : "") + std::to_string(hours);
	std::string m = ((minutes < 10) ? "0" : "") + std::to_string(minutes);
	return h + ":" + m;
}

void EventManager::GenerateError(int time, std::string_view message) const {
	std::cout << ConvertTime(time);
	OutputIdMessage(OutputId::ID13);
	std::cout << message << std::endl;
}

void EventManager::OutputIdMessage(OutputId ch) const { // можно переписать с возвращаемым инт
	switch (ch) {
	case OutputId::ID11:
		std::cout << " 11 ";
		break;
	case OutputId::ID12:
		std::cout << " 12 ";
		break;
	case OutputId::ID13:
		std::cout << " 13 ";
		break;
	}
}

void EventManager::SendGainMessage() {
	std::cout << ConvertTime(club_info_.close_time) << std::endl;
	//выручка для каждого стола
	std::vector<int> table_ids;
	table_ids.reserve(tables_.size());
	for (auto& [table_id, info] : tables_) {
		table_ids.push_back(table_id);
	}
	std::sort(table_ids.begin(), table_ids.end());
	for (auto id : table_ids) {
		std::cout << id << " " << tables_[id].gain << " " << ConvertTime(tables_[id].time_occupied) << std::endl;
	}
}

void EventManager::InputIdMessage(InputId ch) const { 
	switch (ch) {
	case InputId::ID1:
		std::cout << " 1 ";
		break;
	case InputId::ID2:
		std::cout << " 2 ";
		break;
	case InputId::ID3:
		std::cout << " 3 ";
		break;
	case InputId::ID4:
		std::cout << " 4 ";
		break;
	}
}

void EventManager::SendMessage(int time, OutputId ch, std::string_view message) const {
	std::cout << ConvertTime(time);
	OutputIdMessage(ch);
	std::cout << message << std::endl;
}

void EventManager::SendMessage(int time, InputId ch, std::string_view message) const {
	std::cout << ConvertTime(time);
	InputIdMessage(ch);
	std::cout << message << std::endl;
}

void EventManager::ProcessId1(const ClubEvent& client_action) {
	if (clients_.find(client_action.client_name) != clients_.end()) {
		GenerateError(client_action.time, "YouShallNotPass");
		return;
	}
	if (client_action.time < club_info_.open_time) {
		GenerateError(client_action.time, "NotOpenYet");
		return;
	}

	clients_.insert({ client_action.client_name, client_action });
	queue_.push_back(client_action.client_name);
	waiting_clients_.insert(queue_.back());
}

void EventManager::ProcessId2(const ClubEvent& client_action) {
	if (clients_.find(client_action.client_name) == clients_.end()) {
		GenerateError(client_action.time, "ClientUnknown");
		return;
	}
	if (client_action.table.has_value()) {
		if (tables_.find(client_action.table.value()) != tables_.end()) {
			if (tables_.at(client_action.table.value()).is_busy == true) {
				GenerateError(client_action.time, "PlaceIsBusy");
				return;
			}
		}
		if (waiting_clients_.find(client_action.client_name) == waiting_clients_.end()) {
			//tables - save gain+time_occupied from this client 
			LeaveTableSaveGain(clients_[client_action.client_name].table.value(), client_action.time);
			// and change this table fo unbusy, other - busy+time_last_coming
			OccupyTable(client_action.table.value(), client_action.time);
			//clients - change table and time for this client
			clients_[client_action.client_name] = client_action;
			return;
		}

		LeaveQueue(client_action.client_name);
		clients_[client_action.client_name] = client_action;
		OccupyTable(client_action.table.value(), client_action.time);
		++club_info_.busy_tables;
	}
}

void EventManager::ProcessId3(const ClubEvent& client_action) {
	if (club_info_.busy_tables < club_info_.common_table_count) {
		GenerateError(client_action.time, "ICanWaitNoLonger!");
		return;
	}
	if (queue_.size() > club_info_.common_table_count) {
		std::cout << ConvertTime(client_action.time);
		OutputIdMessage(OutputId::ID11);
		std::cout << client_action.client_name << std::endl;

		LeaveQueue(client_action.client_name);
	}
}

void EventManager::ProcessId4(const ClubEvent& client_action) {
	if (clients_.find(client_action.client_name) == clients_.end()) {
		GenerateError(client_action.time, "ClientUnknown");
		return;
	}
	if (waiting_clients_.find(client_action.client_name) != waiting_clients_.end()) {
		LeaveQueue(client_action.client_name);
		return;
	}
	if (clients_[client_action.client_name].table.has_value()) {
		LeaveTableSaveGain(clients_[client_action.client_name].table.value(), client_action.time);
		int free_table = clients_[client_action.client_name].table.value();
		clients_.erase(client_action.client_name);

		if (!queue_.empty()) {
			std::string client_name = queue_.front();
			LeaveQueue(queue_.front());
			clients_[client_name].table = free_table;
			clients_[client_name].time = client_action.time;

			OccupyTable(free_table, client_action.time);
			SendMessage(client_action.time, OutputId::ID12, client_name + " " + std::to_string(free_table));
		}
	}
}
