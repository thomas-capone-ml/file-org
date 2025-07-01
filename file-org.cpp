#include <iostream>
#include <string>
#include <filesystem>
#include <dirent.h>
#include <magic.h>
#include "csv.h"
#include <cstdlib>
#include <fstream>

std::string getFileMimeSubtype(const std::string& filePath) {
    magic_t magic = magic_open(MAGIC_MIME_TYPE);
    if (!magic) {
        return "ERROR: Failed to initialize libmagic";
    }

    if (magic_load(magic, nullptr) != 0) {
        std::string error = magic_error(magic);
        magic_close(magic);
        return "ERROR: Failed to load magic database: " + error;
    }

    const char* mime = magic_file(magic, filePath.c_str());
    if (!mime) {
        std::string error = magic_error(magic);
        magic_close(magic);
        return "ERROR: Failed to detect file type: " + error;
    }

    std::string mimeType(mime);
    magic_close(magic);

    size_t slashPos = mimeType.find('/');
    if (slashPos != std::string::npos && slashPos + 1 < mimeType.length()) {
        return mimeType.substr(slashPos + 1);  // Return the subtype
    }

    return "ERROR: Invalid MIME type format";
}


int organise(std::string settings, std::string directory){


    std::cout << "\n\nSettings: " << settings << '\n';
    std::cout << "Directory: " << directory << '\n' << '\n';

    DIR* dir = opendir(directory.c_str());  // Fix 1: opendir expects const char*, convert std::string

    if (dir == nullptr) {
        std::cerr << "Failed to open directory: " << directory << '\n';
        return 1;
    }


    dirent* entry;
    for (entry = readdir(dir); entry != nullptr; entry = readdir(dir)) {
        char firstChar = entry->d_name[0];
        //ignores hidden files and hidden directories
        if (firstChar == '.'){
            continue;
        }

        else{ 
            std::string filepath;
            filepath = directory + '/' + entry->d_name;
			std::string result = getFileMimeSubtype(filepath);

            io::CSVReader<2> in(settings);
            in.read_header(io::ignore_extra_column, "file-type", "org-dir");
            
            std::string file_type, org_dir;
            
            for(int row = 0; in.read_row(file_type, org_dir); row++) {
                //std::cout << "Row " << row << ": " << file_type << ", " << org_dir << std::endl;
                

                if (file_type == result) {
                    std::cout << "Match found! moving a file to" << org_dir << std::endl;
                    std::string mkdir_command;
                    mkdir_command = "mkdir " + org_dir;
                    std::string organise_command;
                    organise_command = "mv '" + filepath + "' " + org_dir;
                    std::cout << organise_command << std::endl;
                    system(mkdir_command.c_str());
                    system(organise_command.c_str());



                }
            }

        
        }
    }  

    closedir(dir);
    return 0;
}


void logo(){ 
    std::string filename;
    filename = "logo.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    
    // Close file
    file.close();

}


int main(){
    logo();
    std::cout << std::endl;
	int option;
	std::cout << "SELECT AN OPTION\n1)Organise a Directory\n2)EXIT\n\n>:";
	std::cin >> option;

	if (option == 1){
	    //takes in the values to be used
	    std::string settings;
	    std::string directory;

	    std::cout << "Input the file you want to use for settings (EG: default-settings.csv): ";
	    std::cin >> settings;
	    std::cout << "\nInput the directory that you want to organise: ";
	    std::cin >> directory;

	    //runs the organisation script
	    organise(settings, directory);
	    std::cout << std::endl << "REMAINING CONTENT:\n" << system("ls -a");
	}
	else if (option == 2){
		return 0;
	}
	else{
		std::cout << "INVALID INPUT!!!\n";
        main();
	}

	return 0;
}
