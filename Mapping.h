#ifndef MAPPING_H
#define MAPPING_H

#include <string>
#include <vector>
#include <map>
#include "picojson/picojson.h"

class Mapping
{
	std::map< std::wstring, std::map< std::wstring, std::wstring > > mapping;
	std::vector< std::wstring > columns;
	std::wstring name;

public:
	Mapping(std::wstring name) : name(name)
	{

	}

	void AddColumn(std::wstring col)
	{
		columns.push_back(col);
	}

	void AddRow(std::wstring rowname, std::vector< std::wstring > row)
	{
		std::map< std::wstring, std::wstring > map;

		if (row.size() != columns.size())
		{
			std::wcerr << "The number of columns in row " << rowname << " of table " << name << " do not match" << std::endl;
			exit(1);
		}

		for (int i=0; i<row.size(); i++)
		{
			map[ columns[i] ] = row[i];
		}

		mapping[rowname] = map;
	}

	std::wstring getName () const
	{
		return name;
	}

	picojson::value asJson()
	{
		std::map< std::wstring, picojson::value > outerMap;
		for (auto outerkv : mapping)
		{
			std::map< std::wstring, picojson::value > innerMap;
			for (auto innerkv : outerkv.second)
			{
				innerMap[innerkv.first] = picojson::value(innerkv.second);
			}
			outerMap[outerkv.first] = picojson::value(innerMap);
		}

		return picojson::value(outerMap);
	}
};

#endif // MAPPING_H
