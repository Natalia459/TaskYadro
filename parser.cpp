#include "parser.h"

#include <iostream>
#include <regex>
#include <charconv>
#include <algorithm>

ComputerClub Parser::GetComputerClubInfo() const {
	return club_info_;
}

std::optional<ClubEvent> Parser::ParseLine(std::string_view line, int max_table_count) {
	ClubEvent client;
	auto words = SplitIntoWords(line);

	if (words.size() < 3) {
		return std::nullopt;
	}

	auto time_status = CheckTimeFormat(words[0]);
	if (!time_status) {
		return std::nullopt;
	}
	if (time_status.value() < last_time_event_) {
		return std::nullopt;
	}

	auto id_status = CheckIdFormat(words[1]);
	if (!id_status) {
		return std::nullopt;
	}

	auto client_status = CheckClientNameFormat(words[2]);
	if (!client_status) {
		return std::nullopt;
	}

	client = { time_status.value(), id_status.value(), client_status.value(), std::nullopt };

	if (words.size() == 4) {
		auto table_status = CheckTablesFormat(words[3], max_table_count);
		if (!table_status) {
			return std::nullopt;
		}
		client.table = table_status;
	}
	last_time_event_ = time_status.value();
	return client;
}

std::optional<ComputerClub> Parser::ParseHeader(int tables, std::string time, double cost) {
	if (tables < 0) {
		std::cout << "The number of tables does not match the input format" << std::endl;
		return std::nullopt;
	}

	if (cost < 0) {
		std::cout << "Cost pro hour format is incorrect" << std::endl;
		return std::nullopt;
	}

	std::string open_time = time.substr(0, 5);
	std::string close_time = time.substr(6, 5);
	auto open_status = CheckTimeFormat(open_time);
	auto close_status = CheckTimeFormat(close_time);

	if (!open_status || !close_status) {
		return std::nullopt;
	}
	int busy_tables = 0;

	return ComputerClub{ tables, open_status.value(), close_status.value(), cost, busy_tables };
}


std::optional<int> Parser::CheckTimeFormat(std::string_view line)  const {
	static std::regex reg(R"/(^([01]?[0-9]|2[0-3]):([0-5]\d)$)/");
	bool ans = std::regex_match(line.begin(), line.end(), reg);

	if (!ans) {
		return std::nullopt;
	}
	return ConvertTime(std::string(line));
}

std::optional<int> Parser::CheckIdFormat(std::string_view line)  const {
	static std::regex reg(R"/(^[1-4]$)/");
	bool ans = std::regex_match(line.begin(), line.end(), reg);

	if (!ans) {
		return std::nullopt;
	}
	int res;
	std::from_chars(line.data(), line.data() + line.size(), res);
	return res;
}

std::optional<std::string> Parser::CheckClientNameFormat(std::string_view line)  const {
	bool is_fine = std::all_of(line.begin(), line.end(), [](char c)
		{
			bool dash_or_underline = c == '-' || c == '_';
			bool number = c >= '0' && c <= '9';
			bool up_letter = c >= 'A' && c <= 'Z';
			bool down_letter = c >= 'a' && c <= 'z';
			return (dash_or_underline || number) || (up_letter || down_letter);
		});

	if (!is_fine) {
		return std::nullopt;
	}
	return std::string(line);
}

std::optional<int> Parser::CheckTablesFormat(std::string_view line, int max_tables_count)  const {
	int table;
	std::from_chars(line.data(), line.data() + line.size(), table);

	if (table > max_tables_count || table <= 0) {
		return std::nullopt;
	}
	return table;
}

int Parser::ConvertTime(std::string&& time) const {
	return std::stoi(time.substr(0, 2)) * 60 + std::stoi(time.substr(3, 2));
}

std::vector<std::string_view> Parser::SplitIntoWords(std::string_view line) const {
	std::vector<std::string_view> words;

	size_t pos = line.find_first_not_of(' ');
	size_t line_end = line.npos;

	while (pos != line_end) {
		size_t space = line.find(' ', pos);
		words.push_back(space == line_end ? line.substr(pos) : line.substr(pos, space - pos));
		pos = line.find_first_not_of(' ', space);
	}
	return words;
}
