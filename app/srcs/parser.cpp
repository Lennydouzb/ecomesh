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
    unsigned int gw_id = topoData["gateway"]["id"].get<unsigned int>(); //+ 1;
	Nodes.push_back(Node(gw_id, Matrix<float>(1, 2 , NO_TYPE, {topoData["gateway"]["x"], topoData["gateway"]["y"]}, -1, -1), costmap["gateway_sbc"].embodied, costmap["gateway_sbc"].Drate, costmap["gateway_sbc"].Drate * simtime));
	double BasicNodeEmbodied = costmap["microcontroller"].embodied + costmap["radio_transceiver"].embodied + costmap["battery_small"].embodied +costmap["serial_flash"].embodied +costmap["pcb_enclosure"].embodied;
	double BasicNodeDrate = costmap["microcontroller"].Drate + costmap["radio_transceiver"].Drate + costmap["battery_small"].Drate +costmap["serial_flash"].Drate +costmap["pcb_enclosure"].Drate;
	for (auto it = topoData["sensors"].begin(); it != topoData["sensors"].end(); ++it)
	{
        unsigned int se_id = (*it)["id"].get<unsigned int>();// + 1;
		Nodes.push_back(Node(se_id , Matrix<float>(1, 2, NO_TYPE, {(*it)["x"], (*it)["y"]}, -1, -1), BasicNodeEmbodied, BasicNodeDrate, BasicNodeDrate * simtime));
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

void parseCoojaLogs(Environment& envir, std::ifstream& file)
{
    if (!file.is_open())
    {
        std::cerr << "Error : cannot open file COOJA.testlog" << std::endl;
        return;
    }

    std::string line;
    uint64_t total_R = 0;
    double operational_carbon_gco2 = 0.0;

    while (std::getline(file, line))
    {
        if (line.find("[INFO: ecomesh-gw] RX") != std::string::npos)
        {
            total_R++;
        }

        size_t e_pos = line.find("] E ");
        if (e_pos != std::string::npos)
        {
            long long timestamp;
            unsigned int node_id;

            int res1 = std::sscanf(line.c_str(), "%lld %u", &timestamp, &node_id);
            
            if (res1 == 2)
            {
                unsigned int seq;
                unsigned long long last_clock_ms, ticks_cpu, ticks_lpm, ticks_tx, ticks_rx;
                const char* energy_data_str = line.c_str() + e_pos + 4;
                
                int res2 = std::sscanf(energy_data_str, "%u %llu %llu %llu %llu %llu",
                            &seq, &last_clock_ms, &ticks_cpu, &ticks_lpm, &ticks_tx, &ticks_rx);

                if (res2 == 6)
                {
                    if (node_id == envir.getGateway().getId())
                    {
                        double mj = (static_cast<double>(ticks_cpu) / RTIMER_SECOND) * CPU_ACTIVE_MW
                                + (static_cast<double>(ticks_lpm) / RTIMER_SECOND) * CPU_SLEEP_MW
                                + (static_cast<double>(ticks_tx)  / RTIMER_SECOND) * RADIO_TX_MW
                                + (static_cast<double>(ticks_rx)  / RTIMER_SECOND) * RADIO_LISTEN_MW;

                        double wh = mj / 3600.0 / 1000.0;
                        size_t current_day = static_cast<size_t>(timestamp / 86400000LL);
                        double grid_intensity = envir.getelecconsfromday(current_day);
                        operational_carbon_gco2 += wh * grid_intensity;
                    }
                    else
                    {
                        envir.pushData(node_id, timestamp, ticks_cpu, ticks_lpm, ticks_rx, ticks_tx);
                    }
                }
                else
                {
                    std::cerr << "[DIAGNOSTIC] res2 failed (" << res2 << "/6)  : " << energy_data_str << std::endl;
                }
            }
            else
            {
                std::cerr << "[DIAGNOSTIC] res1 failed(" << res1 << "/2)  : " << line << std::endl;
            }
        }
    }
    std::cout << "Nombre de RX (paquets livres R) : " << total_R << std::endl;
    std::cout << "Carbone Operationnel Gateway (E * I) : " << operational_carbon_gco2 << " gCO2" << std::endl;
}