
///#include <iostream>
#include <fstream>
#include <time.h> 
#include <boost/filesystem.hpp>
//#include "CLI11.hpp"
#include "spdlog/fmt/fmt.h"
#include "yijinjing/paged/PageEngine.h"
#include "yijinjing/utils/json.hpp"

YJJ_NAMESPACE_START

class PstTimeCtrl: public PstBase
{
public:
    PstTimeCtrl(PageEngine* pe, const std::string & stopTimePt) 
            : engine(pe)
            , stop_time(stopTimePt)
            {}
            
    void go(){
        time_t rawtime = 0;
        ::time (&rawtime);

        struct tm * dt = ::localtime(&rawtime);
        
        char buffer[10];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", dt);
        
        if( stop_time.compare(buffer) < 0){
            fmt::print("Auto stop with stop time {}.\n", stop_time);
            engine->stop();
        }        
    }
    
    std::string getName() const { return "TimeCtrl"; }
    
private:
    PageEngine* engine;
    const std::string stop_time;
};
DECLARE_PTR(PstTimeCtrl);

YJJ_NAMESPACE_END

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

	std::string log_folder, journal_folder;
    std::string stopTimePt = "";
	int freq = 1, cpu_id = -1;
	{
	    std::string filename = "page_engine.json";
		if (argc > 1){
			filename = argv[1];
		}

	//	CLI::App app{"Page engine service"};
	//  app.add_option("-c,--conf", filename, "Page engine service configuration json file, default: page_engine.json");
	//  CLI11_PARSE(app, argc, argv);

		if(!boost::filesystem::exists(filename)){
			fmt::print(">>> Page engine configuration file {} does not exist!\n", filename);
			return -1;
		}

		std::ifstream ifs(filename.c_str());
		nlohmann::json conf_j;
		ifs >> conf_j;
		ifs.close();

		
		try{
			nlohmann::json page_engine_j = conf_j["page_engine"];
			log_folder = page_engine_j["log_folder"].get<std::string>();
			journal_folder = page_engine_j["journal_folder"].get<std::string>();
			
			if (page_engine_j.find("frequency") != page_engine_j.end()) {
				freq = page_engine_j["frequency"].get<int>();
				if(freq <= 0) freq = 1;
			}

			if (page_engine_j.find("cpu_id") != page_engine_j.end()) {
				cpu_id = page_engine_j["cpu_id"].get<int>();
			}
            
            if (page_engine_j.find("stop_time") != page_engine_j.end()) {
				stopTimePt = page_engine_j["stop_time"].get<std::string>();
			}
		}catch(...) {
			fmt::print(">>> The json file {} must contain {'page_engine': dict(log_folder='', journal_folder='')}.\n");
			return -1;
		}
	}

	fmt::print(">>> PageEngine run with log_folder {}, journal_folder {}, frequency {}.\n", log_folder, journal_folder, freq);
	if(log_folder.size() > 0 && journal_folder.size() > 0 
		&& ensure_dir_exists(log_folder) && ensure_dir_exists(journal_folder)){

	    yijinjing::PageEngine engine(journal_folder + "/" + "PAGE_ENGINE_COMM", journal_folder + "/" + "TEMP_PAGE", log_folder);
        
        if(stopTimePt.size() > 0){
        	fmt::print(">>> Add AutoTimeCtrl with stop time {}.\n", stopTimePt);
        	yijinjing::PstTimeCtrlPtr task_control(new yijinjing::PstTimeCtrl(&engine, stopTimePt));
        	engine.add_task(task_control);
    	}
        
        
	    engine.set_task_freq(freq);
	    engine.start(cpu_id);
	}else{
		fmt::print(">>> Failed to create log_folder {} or journal_folder {}, please check.\n", log_folder, journal_folder);
		return -1;
	}

    return 0;
}
