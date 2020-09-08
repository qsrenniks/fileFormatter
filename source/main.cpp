#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <sstream>

static bool isValidFilePath(const std::filesystem::path& filepath)
{
	std::string exten = filepath.extension().string();
	if(exten == ".cpp" || exten == ".h")
	{
		return true;
	}
	else return false;
}

static bool isSourceFile(const std::filesystem::path& filepath)
{
	//std::cout << "filestem: " << filepath.extension().string() << std::endl;
	if(filepath.extension().string() == ".cpp")
	{
		return true;
	}
	else return false;
}

static bool isHeaderFile(const std::filesystem::path& filepath)
{
	return !isSourceFile(filepath);
}

static std::string buildFileHeaderComment(const std::filesystem::path& filepath)
{
	//this should later become a command arg
	
	time_t t = time(nullptr);
	tm* timePtr = localtime(&t);
	
	std::stringstream headerComment;
	headerComment << "/*!" << std::endl;
	headerComment << " * \\file " << filepath.filename().string() << std::endl;
	headerComment << " * \\author Quinton Skinner" << std::endl;
	headerComment << " * \\par Email: qsrenniks\\@gmail.com" << std::endl;
	headerComment << " * \\par " << timePtr->tm_year + 1900 << "/" << timePtr->tm_mon << "/" << timePtr->tm_mday << std::endl;
	headerComment << " */" << std::endl;
	return headerComment.str();
}

static std::string buildHeaderGuard(const std::filesystem::path& filepath)
{
	std::string headerTag = filepath.stem().string();
	for(unsigned i = 0; i < headerTag.size(); ++i)
	{
		headerTag[i] = std::toupper(headerTag[i]);
	}
	
	std::stringstream headerGuard;
	headerGuard << "#ifndef " << headerTag << "_H" << std::endl;
	headerGuard << "#define " << headerTag << "_H" << std::endl << std::endl;
	headerGuard << "#endif " << std::endl;
	return headerGuard.str();
}

int main(int argc, const char** argv)
{
	if(argc < 2)
	{
		std::cout << "============ USAGE ============" << std::endl;
		std::cout << "ff.exe <FILENAME>" << std::endl;
		std::cout << "===============================" << std::endl;
	}
	
	std::filesystem::path currentPath = std::filesystem::current_path();
	std::filesystem::path filePath = currentPath / argv[1];
	
	if(!isValidFilePath(filePath))
	{
		std::cout << "file extension is not supported, only .cpp and .h are supported" << std::endl;
		return 0;
	}
	
	if(!std::filesystem::exists(filePath)) // program ditches on existing files
	{
		std::ofstream file;
		file.open(filePath);
		
		if(file)
		{
			std::stringstream fileData;
			fileData << buildFileHeaderComment(filePath);
			fileData << std::endl;
			
			if (!isSourceFile(filePath))
			{
				fileData << buildHeaderGuard(filePath);
			}
			file << fileData.str();
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