#include "objects.h"
#include "parser.h"
#include "event_manager.h"

#include <iostream>
#include <fstream>


int main(int argc, char* argv[]){
	if (argc == 1)
		std::cout << "The file name is missing" << std::endl;

	if (argc >= 2) {
		std::ifstream fin(argv[1]);

		if (!fin.is_open()) {
			std::cout << "Error while opening file" << std::endl;
		}

		int tables;
		fin >> tables;

		std::string time;
		std::getline(fin, time);
		std::getline(fin, time);

		double cost;
		fin >> cost;

		Parser handler;
		auto status = handler.ParseHeader(tables, time, cost);
		if (!status) {
			exit(1);
		}

		std::string line;

		EventManager manager(status.value());

		manager.StartWorking();
		std::getline(fin, line);
		while (std::getline(fin, line)) {
			auto line_status = handler.ParseLine(line, tables);
			if (!line_status) {
				std::cout << line << std::endl;
				exit(1);
			}
			manager.SetEvent(std::move(line_status.value()));
		}
		manager.FinishWorking();
	}

	return 0;
}