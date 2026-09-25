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
		double getEnergyJoules(double cpu_mw, double lpm_mw, double tx_mw, double rx_mw, double rtimer_freq) const
        {
            long long total_cpu = 0, total_lpm = 0, total_rx = 0, total_tx = 0;

            for (const auto& d : data)
            {
                total_cpu += d.delta_cpu;
                total_lpm += d.delta_lpm;
                total_rx  += d.delta_rx;
                total_tx  += d.delta_tx;
            }

            double e_cpu = (cpu_mw / 1000.0) * (static_cast<double>(total_cpu) / rtimer_freq);
            double e_lpm = (lpm_mw / 1000.0) * (static_cast<double>(total_lpm) / rtimer_freq);
            double e_rx  = (rx_mw  / 1000.0) * (static_cast<double>(total_rx)  / rtimer_freq);
            double e_tx  = (tx_mw  / 1000.0) * (static_cast<double>(total_tx)  / rtimer_freq);

            return (e_cpu + e_lpm + e_rx + e_tx);
        }
        double getEnergyWh(double cpu_mw, double lpm_mw, double tx_mw, double rx_mw, double rtimer_freq) const
        {
            return getEnergyJoules(cpu_mw, lpm_mw, tx_mw, rx_mw, rtimer_freq) / 3600.0;
        }

        double getDailyConsomationWh(double simlen_ms, double cpu_mw, double lpm_mw, double tx_mw, double rx_mw, double rtimer_freq) const
        {
            if (simlen_ms <= 0.0) return 0.0;

            double sim_days = simlen_ms / (86400.0 * 1000.0); // ms -> jours
            return getEnergyWh(cpu_mw, lpm_mw, tx_mw, rx_mw, rtimer_freq) / sim_days;
        }

        double getEstimatedLifetimeDays(double battery_wh, double simlen_ms, double cpu_mw, double lpm_mw, double tx_mw, double rx_mw, double rtimer_freq) const
        {
            double daily_wh = getDailyConsomationWh(simlen_ms, cpu_mw, lpm_mw, tx_mw, rx_mw, rtimer_freq);
            if (daily_wh <= 0.0) 
				return 365.0;

            return (battery_wh / daily_wh);
        }
};
