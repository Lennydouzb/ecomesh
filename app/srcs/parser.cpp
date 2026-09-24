/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldesboui <ldesboui@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 18:06:06 by ldesboui          #+#    #+#             */
/*   Updated: 2026/09/20 18:39:10 by ldesboui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Environment.hpp"
#include "../includes/json.hpp"
#include <fstream>
using json = nlohmann::json;

Environment parseJson(std::ifstream& grid, std::ifstream& topo, std::map<std::string, t_compdata> costmap)
{
	nlohmann::json gridData = json::parse(grid);
	nlohmann::json topoData = json::parse(topo);
	std::vector<Node> Nodes;
	double simtime = topoData["emulated_minutes"].get<double>() * 60.0;
	Nodes.push_back(Node(topoData["gateway"]["id"], Matrix<float>(1, 2 , NO_TYPE, {topoData["gateway"]["x"], topoData["gateway"]["y"]}, -1, -1), costmap["gateway_sbc"].embodied, costmap["gateway_sbc"].Drate, costmap["gateway_sbc"].Drate * simtime));
	double BasicNodeEmbodied = costmap["microcontroller"].embodied + costmap["radio_transceiver"].embodied + costmap["battery_small"].embodied +costmap["serial_flash"].embodied +costmap["pcb_enclosure"].embodied;
	double BasicNodeDrate = costmap["microcontroller"].Drate + costmap["radio_transceiver"].Drate + costmap["battery_small"].Drate +costmap["serial_flash"].Drate +costmap["pcb_enclosure"].Drate;
	for (auto it = topoData["sensors"].begin(); it != topoData["sensors"].end(); ++it)
	{
		Nodes.push_back(Node((*it)["id"], Matrix<float>(1, 2, NO_TYPE, {(*it)["x"], (*it)["y"]}, -1, -1), BasicNodeEmbodied, BasicNodeDrate, BasicNodeDrate * simtime));
	}
	std::vector<t_elec> stats;
	size_t i = 0;
	for (auto it = gridData["intensity_gco2e_per_kwh"].begin(); it != gridData["intensity_gco2e_per_kwh"].end(); ++it)
	{
		stats.push_back((t_elec){i, *it});	
		++i;
	}
	t_stats temptocompile;
	Environment results((Nodes[0]), stats, 0, 0, temptocompile, Nodes, simtime);
	return (results);
}

std::map<std::string, t_compdata> loadCSV(const std::string &path)
{
    std::map<std::string, t_compdata> components;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error : cannot open file " << path << std::endl;
        return components;
    }
    std::string line;
    if (!std::getline(file, line))
        return components;
    int line_num = 1;
    while (std::getline(file, line))
    {
        line_num++;
        if (line.empty()) continue;
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;
        for (char c : line)
        {
            if (c == '"')
            {
                inQuotes = !inQuotes;
            }
            else if (c == ',' && !inQuotes)
            {
                fields.push_back(field);
                field.clear();
            }
            else
            {
                field += c;
            }
        }
        fields.push_back(field);
        if (fields.size() < 5 || fields[0].empty())
            continue;
        try
        {
            t_compdata data;
            data.embodied = std::stod(fields[2]);
            data.lifetime = std::stod(fields[3]);
            data.margin = std::stod(fields[4]);
			data.Drate = data.embodied / (data.lifetime * SECONDS_PER_YEAR); 

            components[fields[0]] = data;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Conversion error at line " << line_num 
                      << " (" << e.what() << ") : " << line << std::endl;
        }
    }
    return components;
}

void    parseCoojaLogs(Environment& envir, std::ifstream& file)
{
    (void) envir;
    if (!file.is_open())
    {
        std::cerr << "Error : cannot open file COOJA.testlog" << std::endl;
        return ;
    }
    std::string line;
    uint64_t total_R = 0;
    if (!std::getline(file, line))
        return ;
    while(std::getline(file, line))
    {
        if (line.find("[INFO: ecomesh-gw] RX") != std::string::npos)
            total_R++;
        else if (line.find(" ] E ") != std::string::npos)
        {
            long long timestamp;
            int node_id, seq;
            uint64_t ticks_tx, ticks_rx, ticks_deep_lpm, ticks_cpu, ticks_lpm;
            int parsed = std::sscanf(line.c_str(),
                "%lld %d [INFO: %*s ] E %d %llu %llu %llu %llu %llu",
                &timestamp,
                &node_id,
                &seq,
                &ticks_tx,        // 1er nombre après E  -> TX
                &ticks_rx,        // 2e nombre après E   -> RX
                &ticks_deep_lpm,  // 3e nombre après E   -> Deep LPM
                &ticks_cpu,       // 4e nombre après E   -> CPU
                &ticks_lpm        // 5e nombre après E   -> LPM
            );

            if (parsed == 8) {
                // Les variables contiennent maintenant la bonne catégorie de ticks !
            }
        }
    }
    std::cout << "nombre de RX (paquet livree)" << total_R << std::endl;
}