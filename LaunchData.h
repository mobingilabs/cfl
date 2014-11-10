#ifndef LAUNCHDATA_H
#define LAUNCHDATA_H

#include <string>
#include <vector>
#include <map>

struct Service
{
	std::wstring name;
};

struct Command
{
	std::wstring command;
	std::wstring cwd;
};

struct File
{
	std::wstring path;
	std::wstring content;
	std::wstring owner;
	std::wstring group;
	std::wstring mode;
};

class LaunchData
{
	bool containsData;
	std::vector< std::wstring > yum;
	std::vector< std::wstring > rpm;
	std::vector< std::wstring > rubygems;

	std::vector< Service > services;
	std::vector< Command > commands;
	std::vector< File > files;

public:
	LaunchData() : containsData(false)
	{

	}

	void addYum(std::wstring package)
	{
		containsData = true;
		yum.push_back(package);
	}

	const std::vector< std::wstring >& getYum() const
	{
		return yum;
	}

	void addRPM(std::wstring repo)
	{
		containsData = true;
		rpm.push_back(repo);
	}

	const std::vector< std::wstring >& getRPMs() const
	{
		return rpm;
	}

	void addGem(std::wstring package)
	{
		containsData = true;
		rubygems.push_back(package);
	}

	const std::vector< std::wstring >& getGems() const
	{
		return rubygems;
	}

	void addCommand(Command command)
	{
		containsData = true;
		commands.push_back(command);
	}

	const std::vector< Command >& getCommands() const
	{
		return commands;
	}

	void addService(Service service)
	{
		containsData = true;
		services.push_back(service);
	}

	const std::vector< Service >& getServices() const
	{
		return services;
	}

	void addFile(File file)
	{
		containsData = true;
		files.push_back(file);
	}

	const std::vector< File >& getFiles() const
	{
		return files;
	}

	bool hasData() const
	{
		return containsData;
	}
};

#endif // LAUNCHDATA_H
