/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Environment.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldesboui <ldesboui@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 16:18:47 by ldesboui          #+#    #+#             */
/*   Updated: 2026/09/20 16:36:10 by ldesboui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <vector>
#include "Node.hpp"

typedef struct s_elec
{
	long long	timestamp;
	long long	data;
} t_elec;

typedef struct s_stats
{
	size_t	cpu;
	size_t	tx;
	size_t	rx;
	size_t	battery_capacity;
	size_t	embodied;	
} t_stats;

class Environment
{
	private:
		std::vector<Node>	nodes;
		Node&				gateway;
		std::vector<t_elec> elecs;
		size_t				range;
		size_t				interference_range;
		t_stats				stats;
	public:
};
