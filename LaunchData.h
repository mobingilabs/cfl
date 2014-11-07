#ifndef LAUNCHDATA_H
#define LAUNCHDATA_H

#include <string>
#include <vector>
#include <map>

class LaunchData
{
	bool containsData;
	std::vector< std::wstring > yum;
	std::vector< std::wstring > rpm;
	std::vector< std::wstring > rubygems;

	std::vector< std::wstring > services;
	std::vector< std::wstring > commands;

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

	void addCommand(std::wstring command)
	{
		containsData = true;
		commands.push_back(command);
	}

	const std::vector< std::wstring >& getCommands() const
	{
		return commands;
	}

	void addService(std::wstring service)
	{
		containsData = true;
		services.push_back(service);
	}

	const std::vector< std::wstring >& getServices() const
	{
		return services;
	}

	bool hasData() const
	{
		return containsData;
	}
};

#endif // LAUNCHDATA_H
