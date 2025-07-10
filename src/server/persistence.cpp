#include "/home/shark/code/TinyDB/include/TinyDB.h"

void Database::saveToJSON()
{
	
	nlohmann::json JSONObject;

	JSONObject["strings"] = strings;
	JSONObject["sets"] = sets;
	JSONObject["hashmaps"] = hashmaps;
	JSONObject["arrays"] = arrays;
	JSONObject["key_types"] = key_types;

	std::ofstream JSONWrite("savefiles/tdb.json");
	if (!JSONWrite.is_open())
	{
		throw std::runtime_error("COULD NOT OPEN JSON SAVE FILE");
	}

	JSONWrite << JSONObject.dump(4);
	JSONWrite.close();
}
void Database::periodicalSaveToJSON(){
	while(true){
		std::this_thread::sleep_for(std::chrono::seconds(60));

		nlohmann::json JSONObject;
		JSONObject["strings"] = strings;
		JSONObject["sets"] = sets;
		JSONObject["hashmaps"] = hashmaps;
		JSONObject["arrays"] = arrays;
		JSONObject["key_types"] = key_types;

		std::ofstream JSONWrite("savefiles/tdb.json");
		if (!JSONWrite.is_open())
		{
			throw std::runtime_error("COULD NOT OPEN JSON SAVE FILE");
		}

		JSONWrite << JSONObject.dump(4);
		JSONWrite.close();
	}
} 
void Database::loadFromJSON()
{

	nlohmann::json JSONObject;

	std::ifstream JSONRead("/home/shark/code/TinyDB/src/savefiles/tdb.json");

	if (!JSONRead.is_open())
	{
		throw std::runtime_error("COULD NOT OPEN JSON SAVE FILE");
	}
	JSONRead >> JSONObject;
	JSONRead.close();

	if (JSONObject.contains("strings"))
	{
		strings = JSONObject["strings"];
	}
	if (JSONObject.contains("hashmaps"))
	{
		hashmaps = JSONObject["hashmaps"];
	}
	if (JSONObject.contains("arrays"))
	{
		arrays = JSONObject["arrays"];
	}
	if (JSONObject.contains("key_types"))
	{
		key_types = JSONObject["key_types"];
	}
}
