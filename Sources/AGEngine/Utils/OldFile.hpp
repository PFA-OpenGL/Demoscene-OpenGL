#pragma once

#include <string>

class OldFile
{
public:
	OldFile(const std::string &name = "unknownFile");
	OldFile(const char *name);
	bool exists() const;
	const std::string getFullName() const;
	std::string	getFileName() const;
	std::string getFolder() const;
	std::string getShortFileName() const;
	std::string getExtension() const;
	std::string getFileContent() const;
private:
	std::string				fullPath_;
};