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
#include <iostream>

#define SECONDS_PER_YEAR 31557600.0
#define CPU_ACTIVE_MW      1.8f
#define CPU_SLEEP_MW       0.0006f
#define RADIO_TX_MW        52.0f
#define RADIO_RX_MW        59.0f
#define RADIO_LISTEN_MW    59.0f
#define VOLTAGE            3.0f
#define RTIMER_SECOND      1000000UL

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
		Node				gateway;
		std::vector<t_elec> elecs;
		size_t				range;
		size_t				interference_range;
		t_stats				stats;
		double				simlen;
		Environment();
	public:
		
		Environment(Node gateway, std::vector<t_elec> elecs, size_t range, size_t interference, t_stats stats, std::vector<Node> nodes, double simlen)
			: gateway(gateway)
		{
			this->nodes = nodes;
			this->elecs = elecs;
			this->range = range;
			this->interference_range = interference;
			this->stats = stats;
			this->simlen = simlen;
		}
		void pushData(size_t id, long long tstamp, long long dCpu, long long dLpm, long long dRx, long long dTx)
		{
			std::vector<Node>::iterator it = this->nodes.begin();
			while (it != nodes.end())
			{
				if (it->getId() == id)
				{
					it->pushData(tstamp, dCpu, dLpm, dRx, dTx);
					return; 
				}
				++it;
			}
			std::cout << "pushdata dans rien ???" << std::endl;
		}
		std::vector<Node>&	getNodes() {return nodes;};
		Node&				getGateway() {return gateway;};
		int getelecconsfromday(size_t i) const
		{
			if (this->elecs.empty())
				return 0;
			size_t index = i % this->elecs.size(); 
			return this->elecs[index].data;
		}
};
std::map<std::string, t_compdata> loadCSV(const std::string &path);
Environment parseJson(std::ifstream& grid, std::ifstream& topo, std::map<std::string, t_compdata> costmap);
void    parseCoojaLogs(Environment& envir, std::ifstream& filename);
