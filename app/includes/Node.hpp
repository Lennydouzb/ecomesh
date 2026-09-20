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
		long long			R;
		Node();

	public:
		Node(size_t id, Matrix<float> coords) : coords(coords)
		{
			this->id = id;
		};
		size_t					getId() const { return(id); };
		std::vector<t_data>&	getData() { return(this->data); };

		void					pushData(long long tstamp, long long dCpu, long long dLpm, long long dRx, long long dTx)
		{
			data.push_back((t_data){dCpu, dLpm, dRx, dTx});
		}
};
