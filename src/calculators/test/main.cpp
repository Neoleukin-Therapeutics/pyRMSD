#include "tests.h"
#include "../factory/RMSDCalculatorTypes.h"
#include <vector>
#include <cstdlib>
using std::vector;


enum TestType{
	TEST_SUPERPOSITION_WITH_FIT,
	TEST_SUPERPOSITION_WITH_FIT_AND_CALC,
	TEST_SUPERPOSITION_WITH_FIT_DIFF,
	TEST_SUPERPOSITION_WITH_FIT_AND_CALC_DIFF,
	TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT,
	TEST_ITERATIVE_SUPERPOSITION_WITH_FIT,
	TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION,
	TEST_MATRIX_WITH_FIT_COORDINATES,
	TEST_MATRIX_WITH_FIT_AND_CALCULATION_COORDINATES
};

double get_itsuper_iterations(RMSDCalculatorType calctype, TestType test){
	if(test == TEST_ITERATIVE_SUPERPOSITION_WITH_FIT){
		return 9;
	}

	if(test == TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION){
		switch(calctype){
			case QCP_CUDA_CALCULATOR:
			case QCP_CUDA_MEM_CALCULATOR:
			case QCP_SERIAL_FLOAT_CALCULATOR:
				return 12;
				break;
			default:
				return 11;
		}
	}

	return 0;
}

// Having checked it manually, this function returns the maximum
// difference allowed for one result VS its prody golden data
double get_precision(RMSDCalculatorType calctype, TestType test){
	switch(calctype){

		case KABSCH_SERIAL_CALCULATOR:
		case KABSCH_OMP_CALCULATOR:
			switch (test){
				case TEST_SUPERPOSITION_WITH_FIT_DIFF:
				case TEST_SUPERPOSITION_WITH_FIT_AND_CALC:
				case TEST_SUPERPOSITION_WITH_FIT_AND_CALC_DIFF:
					return 1e-11;
					break;
				case TEST_MATRIX_WITH_FIT_AND_CALCULATION_COORDINATES:
					return 1e-10;
					break;
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT:
					return 1e-9;
					break;
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION:
					return 1e-8;
					break;
				default:
					return 1e-12;
			}
			break;

		case QTRFIT_SERIAL_CALCULATOR:
		case QTRFIT_OMP_CALCULATOR:
			switch (test){
				case TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT:
					return 1e-10;
					break;
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION:
					return 1e-9;
					break;
				default:
					return 1e-12;
			}
			break;

		case QCP_SERIAL_CALCULATOR:
		case QCP_OMP_CALCULATOR:
			switch (test){
				case TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION:
					return 1e-11;
					break;
				default:
					return 1e-12;
			}
			break;

		case QCP_CUDA_CALCULATOR:
		case QCP_CUDA_MEM_CALCULATOR:
#ifdef CUDA_PRECISION_SINGLE
			// Precision os lower, as floats have lower precision
			switch (test){
				case TEST_SUPERPOSITION_WITH_FIT:
				case TEST_SUPERPOSITION_WITH_FIT_AND_CALC_DIFF:
					return 1e-4;
					break;
				case TEST_SUPERPOSITION_WITH_FIT_DIFF:
					return 1e-3; // Really low precision for this one!
					break;
				case TEST_SUPERPOSITION_WITH_FIT_AND_CALC:
					return 1e-5;
					break;
				case TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT:
					return 1e-2; // Really low precision for this one!
					break;
				case TEST_MATRIX_WITH_FIT_AND_CALCULATION_COORDINATES:
					return 1e-4;
					break;
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_MATRIX_WITH_FIT_COORDINATES:
					return 1e-3; // Really low precision for this one!
					break;
				default:
					return 1e-6;
			}
			break;
#else
			switch (test){
				case TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT:
				case TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION:
						return 1e-11;
						break;
				default:
						return 1e-12;
			}
			break;
#endif
		default:
			return 1e-12;
	}
	return 1e-12;
}

int main(int argc, char **argv){

	RMSDCalculatorType available_calculators_d [] =  {
			KABSCH_SERIAL_CALCULATOR,
			KABSCH_OMP_CALCULATOR,
			QTRFIT_SERIAL_CALCULATOR,
			QTRFIT_OMP_CALCULATOR,
			QCP_SERIAL_CALCULATOR,
			QCP_OMP_CALCULATOR,
#ifdef USE_CUDA
			QCP_CUDA_CALCULATOR,
			QCP_CUDA_MEM_CALCULATOR
#endif
	};

	vector<RMSDCalculatorType> available_calculators( available_calculators_d,
			available_calculators_d + sizeof(available_calculators_d)/sizeof(RMSDCalculatorType));

	test_initialize();

	test_copy_array();

	test_mean_coordinates();

	test_translations();

	test_center_coordinates();

	test_swap_atoms();

	test_apply_symm_group();

	test_apply_all_symmetries();

	test_QCP_Kernel();

	test_KABSCH_Kernel();

	for(unsigned int i = 0; i < available_calculators.size();++i){

		test_superposition_with_fit(available_calculators[i],
				"data/Models/prot_plus_ligand_similar/prot_plus_ligand_offset.CA.coords",
				"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.coords",
				"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.rmsd",
				get_precision(available_calculators[i], TEST_SUPERPOSITION_WITH_FIT));

		test_superposition_with_fit(available_calculators[i],
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.CA.coords",
				"data/Superpose_Fit_CA_very_diff/prot_plus_ligand.aligned_CA.coords",
				"data/Superpose_Fit_CA_very_diff/prot_plus_ligand.aligned_CA.rmsd",
				get_precision(available_calculators[i], TEST_SUPERPOSITION_WITH_FIT_DIFF));

		test_superposition_with_fit_and_calc(available_calculators[i],
				"data/Models/prot_plus_ligand_similar/prot_plus_ligand_offset.CA.coords",
				"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_CA.coords",
				"data/Models/prot_plus_ligand_similar/prot_plus_ligand_offset.ligand.coords",
				"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.coords",
				"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.rmsd",
				get_precision(available_calculators[i], TEST_SUPERPOSITION_WITH_FIT_AND_CALC));

		test_superposition_with_fit_and_calc(available_calculators[i],
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.CA.coords",
				"data/Superpose_Fit_CA_Calc_BEN_very_different/prot_plus_ligand_similar.aligned_CA.coords",
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.ligand.coords",
				"data/Superpose_Fit_CA_Calc_BEN_very_different/prot_plus_ligand_similar.aligned_BEN.coords",
				"data/Superpose_Fit_CA_Calc_BEN_very_different/prot_plus_ligand_similar.aligned_BEN.rmsd",
				get_precision(available_calculators[i], TEST_SUPERPOSITION_WITH_FIT_AND_CALC_DIFF));

		test_step_by_step_iterative_superposition_with_fit(available_calculators[i],
				"data/Iterpose_Fit_CA/steps",
				"data/Iterpose_Fit_CA/mean",
				"data/Iterpose_Fit_CA/stretching_trajectory_offset_ligand.initial_CA.coords",
				get_precision(available_calculators[i], TEST_STEP_BY_STEP_ITERATIVE_SUPERPOSITION_WITH_FIT),
				9);

		test_iterative_superposition_with_fit(available_calculators[i],
				"data/Iterpose_Fit_CA/stretching_trajectory_offset_ligand.initial_CA.coords",
				"data/Iterpose_Fit_CA/stretching_trajectory_offset_ligand.iterposed_CA.coords",
				"data/Iterpose_Fit_CA/step_rmsd_diff.rmsd",
				get_precision(available_calculators[i], TEST_ITERATIVE_SUPERPOSITION_WITH_FIT),
				get_itsuper_iterations(available_calculators[i], TEST_ITERATIVE_SUPERPOSITION_WITH_FIT));
#ifdef CUDA_PRECISION_SINGLE
		if (available_calculators[i] != QCP_CUDA_CALCULATOR and
				available_calculators[i] != QCP_CUDA_MEM_CALCULATOR)
#endif
			test_iterative_superposition_with_fit_and_calc_rotation(available_calculators[i],
					"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.initial_all.coords",
					"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.initial_BEN.coords",
					"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.iterposed_all.coords",
					"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.iterposed_BEN.coords",
					"data/Iterpose_Fit_CA_Rot_BEN/step_rmsd_diff.rmsd",
					get_precision(available_calculators[i], TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION),
					get_itsuper_iterations(available_calculators[i], TEST_ITERATIVE_SUPERPOSITION_WITH_FIT_AND_CALC_ROTATION));

		test_matrix_with_fit_coordinates(available_calculators[i],
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.CA.coords",
				"data/Matrix_Fit_CA/prot_plus_ligand_offset_very_different.CA.rmsd_matrix",
				get_precision(available_calculators[i], TEST_MATRIX_WITH_FIT_COORDINATES));

		test_matrix_with_fit_and_calculation_coordinates(available_calculators[i],
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.CA.coords",
				"data/Models/prot_plus_ligand_very_different/not_aligned_offset_prot_plus_ligand.ligand.coords",
				"data/Matrix_Fit_CA_Calc_BEN/prot_plus_ligand_offset_very_different.CA.rmsd_matrix",
				get_precision(available_calculators[i], TEST_MATRIX_WITH_FIT_AND_CALCULATION_COORDINATES));

		test_rmsd_calculation_fit_and_calc_with_symmetry(available_calculators[i]);
	}

#ifdef CUDA_PRECISION_SINGLE
	// Test CUDA (float version) vs serial (float version). Discrepancies are too large and that's the only
	// way of ensuring that errors are due to code and not to precision.
	test_iterative_superposition_with_fit_and_calc_rotation_comparing_QCP_serial_and_QCP_CUDA(
			"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.initial_all.coords",
			"data/Iterpose_Fit_CA_Rot_BEN/stretching_trajectory_offset_ligand.initial_BEN.coords");
#endif

	test_calculator_with_no_superposition_fit(NOSUP_SERIAL_CALCULATOR,
			"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.coords",
			"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.rmsd");

	test_calculator_with_no_superposition_fit(NOSUP_SERIAL_CALCULATOR,
			"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.coords",
			"data/Superpose_Fit_CA_similar/prot_plus_ligand_similar.aligned_CA.rmsd");

	test_calculator_with_no_superposition_fit_and_calc(NOSUP_OMP_CALCULATOR,
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_CA.coords",
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.coords",
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.rmsd");


	test_calculator_with_no_superposition_fit_and_calc(NOSUP_OMP_CALCULATOR,
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_CA.coords",
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.coords",
			"data/Superpose_Fit_CA_Calc_BEN_similar/prot_plus_ligand_similar.aligned_BEN.rmsd");

	return 0;
}

