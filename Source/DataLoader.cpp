#include "DataLoader.h"
#include "boost/range/iterator_range.hpp"
#include "boost/scoped_array.hpp"

using namespace boost;

DataLoader::DataLoader() :
	DataFolderPath(filesystem::current_path() / "Data")
{
}

void DataLoader::GetWeaponConfigNames(boost::shared_array<std::string>& OutArray)
{

}
