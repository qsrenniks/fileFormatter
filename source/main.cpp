#include "pch.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <sstream>
#include <vector>
#include <algorithm>

struct FileType
{
  std::string m_extension;
  std::vector<std::string> m_lines;
};

struct FileData
{
  std::string m_filenameWithExtension;
  std::string m_filenameWithoutExtension;
};

static FileData m_activeData;

const std::string resolveVariable(const std::string& variableName)
{
  if (variableName == "$FILE_NAME$")
  {
    return m_activeData.m_filenameWithExtension;
  }
  else if (variableName == "$DATE$")
  {
    std::stringstream stream;
    time_t t = time(nullptr);
    tm* timePtr = localtime(&t);
    stream << timePtr->tm_year + 1900 << "/" << timePtr->tm_mon << "/" << timePtr->tm_mday;
    return stream.str();
  }
  else if (variableName == "$FILE_UPPER$")
  {
    std::string name = m_activeData.m_filenameWithoutExtension;
    std::transform(name.begin(), name.end(), name.begin(), [](char a) -> char
    {
      return std::toupper(a);
    });
    return name;
  }
  else
  {
    std::cout << "unknown variable: " << variableName << std::endl;
  }
  return variableName;
}

static std::vector<FileType> m_fileTypes;

static bool isValidFilePath(const std::filesystem::path& filepath)
{
  std::string exten = filepath.extension().string();
  if (exten == ".cpp" || exten == ".h")
  {
    return true;
  }
  else return false;
}

static bool isSourceFile(const std::filesystem::path& filepath)
{
  //std::cout << "filestem: " << filepath.extension().string() << std::endl;
  if (filepath.extension().string() == ".cpp")
  {
    return true;
  }
  else return false;
}

static void createFileTypeFromJsonObject(FileType& type, const json& object)
{
  type.m_extension = object["extension"].get<std::string>();

  for (const auto& lineObject : object["style"])
  {
    std::string line = lineObject.get<std::string>();
    type.m_lines.push_back(line);
  }
}

static void parseFormatFile(std::vector<FileType>& files, std::filesystem::path& formatPath)
{
  std::ifstream file;
  file.open(formatPath);
  if (file.is_open())
  {
    json fileData;
    //     fileData << file;
    file >> fileData;

    for (const auto& object : fileData["fileTypes"])
    {
      FileType type;
      createFileTypeFromJsonObject(type, object);
      files.push_back(type);
    }
  }
  else
  {
    std::cout << "could not open style file" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

static const FileType* matchFileTypeToExtension(const std::filesystem::path& file)
{
  std::string extension = file.extension().string();
  for (const auto& type : m_fileTypes)
  {
    if (type.m_extension == extension)
    {
      return &type;
    }
  }
  return nullptr;
}

static const std::string formatLine(const std::string& line)
{
  std::string formattedLine = line;
  auto startOfVar = line.find('$');
  auto endOfVar = line.find('$', startOfVar + 1);
  if (startOfVar != std::string::npos && endOfVar != std::string::npos)
  {
    unsigned len = endOfVar - startOfVar + 1;
    std::string varName = line.substr(startOfVar, len);
    std::string variable = resolveVariable(varName);
    formattedLine.erase(startOfVar, len);
    formattedLine.insert(startOfVar, variable);
  }
  return formattedLine;
}

static void formatFileWithFileType(const FileType& type, std::ofstream& file)
{
  for (const auto& line : type.m_lines)
  {
    const std::string formattedLine = formatLine(line);
    file << formattedLine << std::endl;
  }
}

static void buildFileData(const std::filesystem::path& filePath)
{
  m_activeData.m_filenameWithExtension = filePath.filename().string();
  m_activeData.m_filenameWithoutExtension = filePath.stem().string();
}

int main(int argc, const char** argv)
{
  if (argc < 3)
  {
    std::cout << "============ USAGE ===========" << std::endl;
    std::cout << "ff.exe <FILEFORMAT> <FILENAME>" << std::endl;
    std::cout << "==============================" << std::endl;
  }

  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path formatPath = currentPath / argv[1];
  std::filesystem::path filePath = argv[2];

  parseFormatFile(m_fileTypes, formatPath);
  buildFileData(filePath);

  if (!std::filesystem::exists(filePath)) // program ditches on existing files
  {
    std::ofstream file;
    file.open(filePath);

    if (file)
    {
      const FileType* type = matchFileTypeToExtension(filePath);
      if (type)
      {
        formatFileWithFileType(*type, file);
      }
    }

    file.close();
  }
  else
  {
    std::cout << "file already exists" << std::endl;
    return 0;
  }

  return 0;
}