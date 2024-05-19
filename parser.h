#pragma once
#include "objects.h"

#include <string>
#include <string_view>
#include <optional>
#include <vector>

class Parser {
public:
	Parser() = default;

	ComputerClub GetComputerClubInfo() const;

	std::optional<ClubEvent> ParseLine(std::string_view line, int max_table_count);

	std::optional<ComputerClub> ParseHeader(int tables, std::string time, double cost);

private:
	ComputerClub club_info_;
	int last_time_event_;

	std::optional<int> CheckTimeFormat(std::string_view line) const;

	std::optional<int> CheckIdFormat(std::string_view line) const;

	std::optional<std::string> CheckClientNameFormat(std::string_view line) const;

	std::optional<int> CheckTablesFormat(std::string_view line, int max_tables_count) const;

	int ConvertTime(std::string&& time) const;

	std::vector<std::string_view> SplitIntoWords(std::string_view line) const;
};
