
///#include <iostream>
#include <boost/filesystem.hpp>
#include "CLI11.hpp"
#include "spdlog/fmt/fmt.h"
#include "paged/PageEngine.h"
#include "utils/json.hpp"




bool ensure_dir_exists(const std::string & path){
	if (!boost::filesystem::is_directory(path)){
		if (!boost::filesystem::create_directory(path)){
			///std::cout<<">>> Failed to create directory " << path <<"." <<std::endl;
			fmt::print(">>> Failed to create directory {}.\n", path);
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv){

    std::string filename = "page_engine.json";

	CLI::App app{"Page engine service"};
    app.add_option("-c,--conf", filename, "Page engine service configuration json file, default: page_engine.json");

    CLI11_PARSE(app, argc, argv);

	if(!boost::filesystem::exists(filename)){
		fmt::print(">>> Page engine configuration file {} does not exist!\n", filename);
		return -1;
	}

	std::ifstream ifs(filename);
	nlohmann::json conf;
	ifs >> conf;
	ifs.close();

	std::string log_folder, journal_folder;
	int freq = 1;
	try{
		nlohmann::json page_engine_j = conf["page_engine"];
		log_folder = page_engine_j["log_folder"].get<std::string>();
		journal_folder = page_engine_j["journal_folder"].get<std::string>();
		
		if (page_engine_j.find("frequency") != page_engine_j.end()) {
		  freq = page_engine_j["frequency"].get<int>();
		  if(freq <= 0) freq = 1;
		}
	}catch(...) {
		fmt::print(">>> The json file {} must contain {'page_engine': dict(log_folder='', journal_folder='')}.\n");
		return -1;
	}
	

	fmt::print(">>> PageEngine will run with log_folder {}, journal_folder {}, and frequency {}.\n", log_folder, journal_folder, freq);
	if(log_folder.size() > 0 && journal_folder.size() > 0 
		&& ensure_dir_exists(log_folder) && ensure_dir_exists(journal_folder)){

	    yijinjing::PageEngine engine(journal_folder + "/" + "PAGE_ENGINE_COMM", journal_folder + "/" + "TEMP_PAGE", log_folder);

	    engine.set_freq(1);
	    engine.start();
	}else{
		fmt::print(">>> Failed to create log_folder {} or journal_folder {}, please check.\n", log_folder, journal_folder);
		return -1;
	}

    return 0;
}
