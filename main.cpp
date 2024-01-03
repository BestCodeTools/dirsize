// include for managing file system
#include <filesystem>
#include <vector>
#include <iostream>
#include <string>
#include <map>
std::string bytes_to_str(std::uintmax_t bytes);
std::vector<std::string> get_directories(const std::string &path);
void show_progress(int current, int total);
void map_args(int argc, char **argv, std::map<std::string, std::string> &args);
std::uintmax_t calculate_and_show_path_size(const std::filesystem::path &path);
std::string pad_left(const std::string &str, int width, char pad_char = ' ');
std::string pad_right(const std::string &str, int width, char pad_char = ' ');
void print_total_size(const std::string &path, std::uintmax_t total_size);

const int MAX_WIDTH = 60;
const int MAX_WIDTH_BYTES = 20;
// pad left
std::string pad_left(const std::string &str, int width, char pad_char)
{
  // if the string is already wider than the width, return it
  if (str.size() >= width)
  {
    return str;
  }
  // otherwise, pad it
  std::string padded_str;
  for (int i = 0; i < width - str.size(); ++i)
  {
    padded_str += pad_char;
  }
  padded_str += str;
  return padded_str;
}
// pad right
std::string pad_right(const std::string &str, int width, char pad_char)
{
  // if the string is already wider than the width, return it
  if (str.size() >= width)
  {
    return str;
  }
  // otherwise, pad it
  std::string padded_str = str;
  for (int i = 0; i < width - str.size(); ++i)
  {
    padded_str += pad_char;
  }
  return padded_str;
}
std::string bytes_to_str(std::uintmax_t bytes)
{
  // convert bytes to kilobytes
  float kilobytes = bytes / 1024.0f;
  // convert kilobytes to megabytes
  float megabytes = kilobytes / 1024.0f;
  // convert megabytes to gigabytes
  float gigabytes = megabytes / 1024.0f;

  // return string based on size
  if (gigabytes >= 1.0f)
  {
    return std::to_string(gigabytes) + " GB";
  }
  else if (megabytes >= 1.0f)
  {
    return std::to_string(megabytes) + " MB";
  }
  else if (kilobytes >= 1.0f)
  {
    return std::to_string(kilobytes) + " KB";
  }
  else
  {
    return std::to_string(bytes) + " B";
  }
}

std::vector<std::string> get_directories(const std::filesystem::path &path)
{
  std::vector<std::string> directories;
  for (const auto &entry : std::filesystem::directory_iterator(path))
  {
    if (entry.is_directory())
    {
      directories.emplace_back(entry.path().string());
    }
  }
  return directories;
}

void show_progress(int current, int total)
{
  // calculate percentage
  float percent = static_cast<float>(current) / total;
  // calculate number of hashes to print
  int hashes = static_cast<int>(percent * 100);
  // print hashes
  for (int i = 0; i < hashes; ++i)
  {
    std::cout << "#";
  }
  // print spaces
  for (int i = hashes; i < 100; ++i)
  {
    std::cout << " ";
  }
  // print percentage
  std::cout << " " << percent * 100 << "%\r";
  // flush output
  std::cout.flush();
}
// print total size
void print_total_size(const std::string &path, std::uintmax_t total_size)
{
  std::cout << "Total size of " << pad_right("'" + path + "': ", MAX_WIDTH + 3, ' ') << pad_left(bytes_to_str(total_size), MAX_WIDTH_BYTES, ' ').c_str() << std::endl;
}
void map_args(int argc, char **argv, std::map<std::string, std::string> &args)
{
  // iterate over args
  for (int i = 1; i < argc; ++i)
  {
    // if its an option
    if (argv[i][0] == '-')
    {
      // if its a long option
      if (argv[i][1] == '-')
      {
        // get option name
        std::string option = argv[i] + 2;
        // if its the last arg or the next arg is an option, set value to true
        if (i + 1 == argc || argv[i + 1][0] == '-')
        {
          args[option] = "true";
        }
        else
        {
          // otherwise set value to next arg
          args[option] = argv[i + 1];
          // skip next arg
          ++i;
        }
      }
      else
      {
        // if its a short option
        // get option name
        std::string option = argv[i] + 1;
        // if its the last arg or the next arg is an option, set value to true
        if (i + 1 == argc || argv[i + 1][0] == '-')
        {
          args[option] = "true";
        }
        else
        {
          // otherwise set value to next arg
          args[option] = argv[i + 1];
          // skip next arg
          ++i;
        }
      }
    }
  }
}

std::uintmax_t calculate_and_show_path_size(const std::filesystem::path &path)
{
  // get total size of path
  std::uintmax_t total_size = 0;
  for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
  {
    // if its a file, add its size to total
    if (entry.is_regular_file())
    {
      total_size += entry.file_size();
    }
  }
  // print total size
  print_total_size(path.string(), total_size);
  return total_size;
}

int main(int argc, char **argv)
{
  // extract paths from command line args, ignoring options (-- and - options)
  std::vector<std::filesystem::path> pathList;
  // argmap
  std::map<std::string, std::string> args;

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] != '-')
    {
      // if its not a directory, ignore it
      if (!std::filesystem::is_directory(argv[i]))
      {
        continue;
      }
      pathList.emplace_back(argv[i]);
    }
  }

  // if no paths were specified, use current directory
  if (pathList.empty())
  {
    pathList.emplace_back(".");
  }

  // map args
  map_args(argc, argv, args);

  int current = 0;

  // iterate over paths (recursively) to get total size, print total size, and progress
  for (const auto &p_path : pathList)
  {
    show_progress(current++, pathList.size());
    std::uintmax_t total_size = 0;
    // if --children option is specified, iterate over children
    if (args["children"] == "true")
    {
      for (const auto &entry : get_directories(p_path))
      {
        total_size += calculate_and_show_path_size(entry);
      }
      // print total size
      print_total_size(p_path.string(), total_size);
    }
    else
    {
      calculate_and_show_path_size(p_path);
    }
  }
  // print newline
  std::cout << std::endl;
  return 0;
}