/*
 * QCPCUDAMemKernel.cpp
 *
 *  Created on: Apr 13, 2013
 *      Author: victor
 */

#include "QCPCUDAMemKernel.h"
#include "kernel_functions_cuda.h"
#include "../RMSDCalculationData.h"

#define OFFSET_FOR(conformation_number, number_of_conformations) (((number_of_conformations-1)* conformation_number) - (conformation_number*(conformation_number-1))/2)

inline void checkCudaError(char* message, cudaError error_code){
	if (error_code != 0){
		std::cout<<"Error in "<<message<<". Error code: "<<error_code<<". Exiting..."<<std::flush<<std::endl;
		exit(-1);
	}
}

QCPCUDAMemKernel::QCPCUDAMemKernel(
				RMSDCalculationData* data,
				int threads_per_block,
				int blocks_per_grid):QCPCUDAKernel( data,
													threads_per_block,
													blocks_per_grid) {
	allDeviceRMSDs = NULL;

}

QCPCUDAMemKernel::~QCPCUDAMemKernel(){}

void QCPCUDAMemKernel::matrixInit(RMSDCalculationData* data){
	
	QCPCUDAKernel::matrixInit(data);
						
	//Allocate space to store all rmsds
	checkCudaError("Malloc allDeviceRMSDs", 
			cudaMalloc(
					(void **) &this->allDeviceRMSDs, 
					((data->numberOfConformations*(data->numberOfConformations-1)) / 2) * sizeof(floating_point_type)));
	
}

void QCPCUDAMemKernel::matrixEnd(int rmsds_len,
									std::vector<double>& rmsds){
	rmsds.clear();
	rmsds.resize(rmsds_len);
	
	#ifdef CUDA_PRECISION_SINGLE
	
		float* buffer = new float[rmsds_len];
		
		checkCudaError("allDeviceRMSDs copy to host",
			cudaMemcpy(	buffer,
					this->allDeviceRMSDs,
					rmsds_len * sizeof(float),
					cudaMemcpyDeviceToHost));
		
		for(int i = 0; i < rmsds_len; ++i){
			rmsds[i] = static_cast<double>( buffer[i] );
		}
		
		delete [] buffer;
		
	#else	
		checkCudaError("allDeviceRMSDs copy to host",
			cudaMemcpy(	&(rmsds[0]),
					this->allDeviceRMSDs,
					rmsds_len * sizeof(double),
					cudaMemcpyDeviceToHost));
	#endif
	
	checkCudaError("allDeviceRMSDs cudaFree",
				cudaFree(this->allDeviceRMSDs));
}


void QCPCUDAMemKernel::matrixOneVsFollowingFitEqualCalc(
									double* reference, 
									int reference_conformation_number, 
									double* rmsd,
									RMSDCalculationData* data){
	
	floating_point_type* tmpDeviceReference = &(this->deviceCoords[reference_conformation_number*
	                                                               data->fittingConformationLength]);
	
	// Do the calculations
	calcRMSDOfOneVsFollowing CUDA_KERNEL_DIM(this->blocks_per_grid, this->threads_per_block)(
			tmpDeviceReference,
			reference_conformation_number,
			this->deviceCoords,
			data->numberOfConformations,
			data->atomsPerFittingConformation,
			data->fittingConformationLength,
			&(this->allDeviceRMSDs[OFFSET_FOR(reference_conformation_number, data->numberOfConformations)]));
}

void QCPCUDAMemKernel::matrixOneVsFollowingFitDiffersCalc(
											double* fitReference, 
											double* calcReference,
											int reference_conformation_number, 
											double* rmsd,
											RMSDCalculationData* data){
		
	floating_point_type* tmpFitDeviceReference = &(this->deviceCoords[reference_conformation_number*
	                                                                  data->fittingConformationLength]);
	floating_point_type* tmpCalcDeviceReference = &(this->deviceCalcCoords[reference_conformation_number*
	                                                                       data->calculationConformationLength]);
	
	// Do the calculations
	calcRMSDOfOneVsFollowingFitDiffersCalc CUDA_KERNEL_DIM(this->blocks_per_grid, this->threads_per_block)(
			tmpFitDeviceReference,
			tmpCalcDeviceReference,
			reference_conformation_number,
			&(this->allDeviceRMSDs[OFFSET_FOR(reference_conformation_number, data->numberOfConformations)]),
			data->numberOfConformations,
			data->fittingConformationLength,
			data->atomsPerFittingConformation,
			deviceCoords,
			data->calculationConformationLength,
			data->atomsPerCalculationConformation,
			deviceCalcCoords);
}
