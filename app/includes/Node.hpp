/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldesboui <ldesboui@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 14:45:44 by ldesboui          #+#    #+#             */
/*   Updated: 2026/09/20 18:21:55 by ldesboui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <vector>
#include <iostream>
#include "Matrix.hpp"

typedef struct s_data
{
	long long timestamp;
	// delta is the difference between the last and this one
	// for example last one was 10, this one show 12 so its gonna be only 2 more
	// active cpu
	long long delta_cpu;
	// sleeping cpu
	long long delta_lpm;
	// transmitting
	long long delta_rx;
	// receiving
	long long delta_tx;
}	t_data;

class Node
{
	private:
		size_t				id;
		std::vector<t_data> data;
		Matrix<float>		coords;
		double				initialembodiedCo2;
		double				drate; //Depreciation rate mais ratio je fait une ref a peak gate
		double				Mnode;
		double				lastconso;
		Node();

	public:
		Node(size_t id, Matrix<float> coords, double embodied, double drate, double Mnode) : coords(coords)
		{
			this->id = id;
			this->initialembodiedCo2 = embodied;
			this->drate = drate; //pas encore calculee car besoin de l'environement <- jsuis un menteur en fait
			this->Mnode = Mnode;
			this->lastconso = 0;
		};
		size_t					getId() const { return(id); };
		std::vector<t_data>&	getData() { return(this->data); };
		double					getInitialEmbodied() const { return(initialembodiedCo2); };
		double					getDrate() const { return(drate); };
		double					getMnode() const { return(Mnode); };
		double					getlastconso() const { return(lastconso); };
		void					setlastconso(double val) {this->lastconso = val;};
		void					pushData(long long tstamp, long long dCpu, long long dLpm, long long dRx, long long dTx)
		{
			data.push_back((t_data){tstamp, dCpu, dLpm, dRx, dTx});
		}
};
