#pragma once

#include <string>

class SrcFile {
public:
	SrcFile(std::string name);
	~SrcFile();
	std::string GetContents() const;
private:
	std::string contents;
};

class FileTree {
public:
	FileTree(std::string dirPath);
	~FileTree();
private:
	
};
