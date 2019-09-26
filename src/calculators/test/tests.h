/*
 * tests.h
 *
 *  Created on: 04/03/2013
 *      Author: victor
 */

#ifndef TESTS_H_
#define TESTS_H_
#include "../factory/RMSDCalculatorTypes.h"
	void test_initialize();
	void test_copy_array();
	void test_mean_coordinates();
	void test_translations();
	void test_center_coordinates();

	void test_swap_atoms();
	void test_apply_symm_group();
	void test_apply_all_symmetries();

	void test_QCP_Kernel();
	void test_KABSCH_Kernel();

	void test_superposition_with_fit(RMSDCalculatorType type,
			const char* initial_coords_file,
			const char* final_coords_file,
			const char* rmsd_results_file,
			double precision_of_check);

	void test_superposition_with_fit_and_calc(RMSDCalculatorType type,
			const char* initial_prot_coords_file,
			const char* final_prot_coords_file,
			const char* initial_lig_coords_file,
			const char* final_lig_coords_file,
			const char* rmsd_results_file,
			double precision_of_check);

	void test_step_by_step_iterative_superposition_with_fit(RMSDCalculatorType type,
			const char* step_directory,
			const char* mean_directory,
			const char* initial_prot_coords_file,
			double precision_of_check,
			int expected_number_of_iterations);

	void test_iterative_superposition_with_fit(RMSDCalculatorType type,
			const char* initial_prot_coords_file,
			const char* final_prot_coords_file,
			const char* iteration_rmsd_results_file,
			double precision_of_check,
			int expected_number_of_iterations);

	void test_iterative_superposition_with_fit_and_calc_rotation(RMSDCalculatorType type,
			const char* initial_prot_coords_file,
			const char* initial_lig_coords_file,
			const char* final_prot_coords_file,
			const char* final_lig_coords_file,
			const char* iteration_rmsd_results_file,
			double precision_of_check,
			int expected_number_of_iterations);

	void test_matrix_with_fit_coordinates(RMSDCalculatorType type,
			const char* initial_prot_coords_file,
			const char* rmsd_results_file,
			double precision_of_check);

	void test_matrix_with_fit_and_calculation_coordinates(RMSDCalculatorType type,
			const char* initial_prot_coords_file,
			const char* initial_lig_coords_file,
			const char* rmsd_results_file,
			double precision_of_check);

	void test_iterative_superposition_with_fit_and_calc_rotation_comparing_QCP_serial_and_QCP_CUDA(
			const char* initial_prot_coords_file,
			const char* initial_lig_coords_file);

	void test_rmsd_calculation_fit_and_calc_with_symmetry(RMSDCalculatorType type);

	void test_calculator_with_no_superposition_fit(RMSDCalculatorType type,
			const char* final_coords_file,
			const char* rmsd_results_file);

	void test_calculator_with_no_superposition_fit_and_calc(RMSDCalculatorType type,
			const char* final_prot_coords_file,
			const char* final_lig_coords_file,
			const char* rmsd_results_file);

#endif /* TESTS_H_ */
