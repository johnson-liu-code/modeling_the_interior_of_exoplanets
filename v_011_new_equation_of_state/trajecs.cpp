#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <typeinfo>
#include <iostream>
#include <complex>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

#include "euler.h"
#include "rk4.h"



vector<double> run_trajectory(double number_of_layers, bool print_meas, double pressure, double mass, string integrator, double rho, double r, double delta_r, vector<double> radii_list, vector<double> rho_list, vector<double> K_list, double K0p, double thresh, double G, string eos, double mass_core)
{
    int section = 0;
    double prev_pressure = pressure, prevprev_pressure, prev_mass = mass, prevprev_mass;

    vector<double> quantities;
    quantities.reserve(3);

    vector<double> all_quantities;
    all_quantities.reserve(9);
    
    double mass_iron = 0;

    vector<double> temporary_rho;
    temporary_rho.reserve(3);

    for (int i = 0; i < 3; i++)
    {
        quantities = euler_update(pressure, mass, rho, r, delta_r, K_list[section], rho_list[section], thresh, G);
        temporary_rho.push_back(quantities[2]);
        pressure = quantities[0];
        mass = quantities[1];
        rho = quantities[2];
        
        r += delta_r;
    }

    while (pressure > 0)
    {        
        prevprev_pressure = prev_pressure;
        prev_pressure = pressure;
        
        prevprev_mass = prev_mass;
        prev_mass = mass;

        if (integrator == "e")
        {
            quantities = euler_update(pressure, mass, rho, r, delta_r, K_list[section], rho_list[section], thresh, G);
        }
        else if (integrator == "c")
        {
            quantities = euler_cromer_update(pressure, mass, rho, r, delta_r, K_list[section], rho_list[section], thresh, G);
        }
        else if (integrator == "r")
        {
            quantities = RK4_update(pressure, mass, rho, r, delta_r, K_list[section], K0p, rho_list[section], thresh, G, eos, temporary_rho);
        }
        
        pressure = quantities[0];
        mass = quantities[1];
        rho = quantities[2];
        
        temporary_rho[0] = temporary_rho[1];
        temporary_rho[1] = temporary_rho[2];
        temporary_rho[2] = rho;
        
        if (print_meas == true)
        {
//            frac_radius = r / Rp;
//            frac_mass = mass / Mp;
//            printf("frac_r: %f   density: %f     pressure: %f    frac_mass: %f\n", frac_radius, rho, pressure, frac_mass);
            printf("r: %.6g     density: %.6g     pressure: %.6g     mass: %.6g\n", r, rho, pressure, mass);
        }
        
        r += delta_r;
        
        if (number_of_layers == 3)
        {
            double interface = radii_list[0];
            if (section == 0)
            {
                if (pressure < interface)
                {
                    section++;
                }
            }
            else if (section < radii_list.size())
            {
//                if (r > radii_list[section]*Rp)
                if (r > radii_list[section])
                {
                    section++;
                }
            }
        }
        
        else if (number_of_layers == 2)
        {
            if (section < radii_list.size())
            {
//                printf("hello\n");
//                if (r > radii_list[section]*Rp)
//                printf("r: %f\n", r);
//                printf("radii_list[0]: %f\n", radii_list[0]);
                mass_iron = mass;

                if (r > radii_list[section])
//                if (mass > mass_core)
                {
                    section++;
//                    printf("hello\n");
                    mass_iron = mass;
//                    printf("mass_iron: %f\n", mass_iron);
                }
            }
        }
    }
    
    all_quantities[0] = pressure;
    all_quantities[1] = mass;
    all_quantities[2] = rho;
    all_quantities[3] = prev_pressure;
    all_quantities[4] = prevprev_pressure;
    all_quantities[5] = prev_mass;
    all_quantities[6] = prevprev_mass;
    all_quantities[7] = r;
    all_quantities[8] = mass_iron;
    
    return all_quantities;
}