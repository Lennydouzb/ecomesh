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

Environment parseJson(std::ifstream& grid, std::ifstream& topo)
{
	nlohmann::json gridData = json::parse(grid);
	nlohmann::json topoData = json::parse(topo);
	std::vector<Node> Nodes;
	Nodes.push_back(Node(topoData["gateway"]["id"], Matrix<float>(0, 2 , NO_TYPE, {topoData["gateway"]["x"], topoData["gateway"]["y"]}, -1, -1)));
	
	for (auto it = topoData["sensors"].begin(); it != topoData["sensors"].end(); ++it)
	{
		Nodes.push_back(Node((*it)["id"], Matrix<float>(0, 2, NO_TYPE, {(*it)["x"], (*it)["y"]}, -1, -1)));
	}
	std::vector<t_elec> stats;
	size_t i = 0;
	for (auto it = gridData["intensity_gco2e_per_kwh"].begin(); it != gridData["intensity_gco2e_per_kwh"].end(); ++it)
	{
		stats.push_back((t_elec){i, *it});	
		++i;
	}
}
