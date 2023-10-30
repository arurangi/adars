#include "Utils.hpp"

std::string
ft::to_str(int num) {
    std::stringstream ss;
    ss << num;
    std::string numstr = ss.str();
    return numstr;
}

bool ft::startswith(const std::string& str, const std::string& keyword)
{
    if (str.size() == 0 || keyword.size() == 0)
        return false;
    size_t found = str.find(keyword);
    if (found == std::string::npos )
        return false;
    if (found != 0)
        return false;
    return true;
}

bool ft::endswith(const std::string& str, const std::string& keyword)
{
    if (str.size() == 0 || keyword.size() == 0)
        return false;
    size_t found = str.find(keyword);
    if (found == std::string::npos )
        return false;
    if (found + keyword.size() != str.size())
        return false;
    return true;

}

std::string
ft::get_dir(std::string file_path) {
    int pos = file_path.find_last_of("/");
    if (pos == 0)
        return "/";
    return file_path.substr(0, pos);
}

std::deque<std::string>
ft::list_files_in(std::string directory) {
    const char* directoryPath = directory.c_str();

    DIR *dir;
    struct dirent *ent;
    std::deque<std::string> filesList;

    if ((dir = opendir(directoryPath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG)
                filesList.push_back(ent->d_name);
        }
        closedir(dir);
    }
    else {
        Log::error("Could not open directory: " + directory);
        return filesList;
    }
    return filesList;
}