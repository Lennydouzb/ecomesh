/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Environment.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldesboui <ldesboui@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 16:18:47 by ldesboui          #+#    #+#             */
/*   Updated: 2026/09/20 18:28:45 by ldesboui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <vector>
#include "Node.hpp"
#include <map>

#define SECONDS_PER_YEAR 31557600.0

typedef struct s_elec
{
	size_t		day;
	float		data;
} t_elec;

typedef struct s_stats
{
	float	cpu;
	float	tx;
	float	rx;
	float	battery_capacity;
	float	embodied;	
} t_stats;

typedef struct s_compdata {
    double embodied;
    double lifetime;
    double margin;
	double Drate;
} t_compdata;

class Environment
{
	private:
		std::vector<Node>	nodes;
		Node&				gateway;
		std::vector<t_elec> elecs;
		size_t				range;
		size_t				interference_range;
		t_stats				stats;
		double				simlen;
		Environment();
	public:
		
		Environment(Node& gateway, std::vector<t_elec> elecs, size_t range, size_t interference, t_stats stats, std::vector<Node> nodes, double simlen)
			: gateway(gateway)
		{
			this->nodes = nodes;
			this->elecs = elecs;
			this->range = range;
			this->interference_range = interference;
			this->stats = stats;
			this->simlen = simlen;
		}
		void	pushData(size_t id, long long tstamp, long long dCpu, long long dLpm, long long dRx, long long dTx)
		{
			bool found = 0;
			std::vector<Node>::iterator it = this->nodes.begin();

			while (!found && it != nodes.end())
			{
				if (it->getId() == id)
					found = 1;
				else
					++it;
			}
			it->pushData(tstamp, dCpu, dLpm, dRx, dTx);
		}
		std::vector<Node>&	getNodes() {return nodes;};
		Node&				getGateway() {return gateway;};
};
std::map<std::string, t_compdata> loadCSV(const std::string &path);
Environment parseJson(std::ifstream& grid, std::ifstream& topo, std::map<std::string, t_compdata> costmap);
void    parseCoojaLogs(Environment& envir, std::ifstream& filename);
