/*
 * tester.c
 * 
 * A simple tester for the hdbscan library using C. This file shows how
 * call the different methods. It shows how to run the initial clustering
 * as well as how to recluster using the same data but different minPts.
 * There is examples of how to create the clustering table as well as 
 * statistical values and the cluster confidences and sorting the 
 *
 *  Created on: 17 Sep 2017
 *      Author: junior
 */

//#include "config.h"
#include "hdbscan/hdbscan.h"
#include "dataset.h"
#include <time.h>

int main(int argc, char** argv){
	clock_t begin, end;
	int err;
	double time_spent;
	hdbscan* scan = hdbscan_init(NULL, atoi(argv[1]), DATATYPE_DOUBLE);
	bool rerun_ = false;
	if(scan == NULL){
		printf("ERROR: Could not initialise hdbscan\n");
		exit(0);
	} 
	printf("SUCCESS: hdbscan fully initialised\n");
	
	// This lil loop demonstrates how to use the rerun function
	for(int i = 0; i < 10; i++){
		if(!rerun_){
			begin = clock();
			err = hdbscan_run(scan, dataset, rows, cols, TRUE);
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("hdbscan run Process took %f\n", time_spent);
			rerun_ = true;
		} else{
			begin = clock();
			hdbscan_rerun(scan, atoi(argv[1]) + i);
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			printf("hdbscan rerun Process took %f\n", time_spent);
		}
		
		printf("%d ------- ***********************************************************************************\n", scan->minPoints);
		
		if(err == HDBSCAN_ERROR){
			printf("ERROR: Could not run hdbscan\n");
		} else{
			printf("SUCCESS: hdbscan clustering completed\n");
			IntIntListMap* clusterTable = hdbscan_create_cluster_table(scan->clusterLabels, 0, scan->numPoints);
			hdbscan_print_cluster_table(clusterTable);
			
			IntArrayList* sorted  = int_array_list_init(g_hash_table_size(clusterTable));
			sorted = hdbscan_sort_by_length(clusterTable, sorted);
			printf("\n\nSorted by length = [");
			
			int32_t *data = (int32_t *)sorted->data;
			for(size_t i = 0; i < sorted->size; i++){
				printf("%d ", data[i]);
			}
			printf("]\n\n");
				
			IntDistancesMap* dMap = hdbscan_get_min_max_distances(scan, clusterTable);
			clustering_stats stats;
			hdbscan_calculate_stats(dMap, &stats);
			
			sorted = hdbscan_sort_by_similarity(dMap, sorted, INTRA_DISTANCE_TYPE); // There is choice to use CORE_DISTANCE_TYPE
			printf("Sorted by similarity = [");
			
			data = (int32_t *)sorted->data;
			for(size_t i = 0; i < sorted->size; i++){
				printf("%d ", data[i]);
			}
			printf("]\n\n");
			
			hdbscan_print_distance_map_table(dMap);
			hdbscan_print_stats(&stats);								
			printf("Clustering validity : %d\n", hdbscan_analyse_stats(&stats));
				
			printf("\n\nCluster labels = [");
			for(uint i = 0; i < scan->numPoints; i++){
				printf("%d ", scan->clusterLabels[i]);
			}
			printf("]\n\n");
			int_array_list_delete(sorted);
			hdbscan_destroy_distance_map_table(dMap);
			hdbscan_destroy_cluster_table(clusterTable);
		}
		
		printf("***********************************************************************************\n\n");
		break;
	}
	
	hdbscan_destroy(scan);
	
	return 0;
}
