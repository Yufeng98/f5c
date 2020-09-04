// #include "src/align.c"
#include "src/f5c.h"
#include "src/f5cmisc.h"
#include <assert.h>
#include <getopt.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "src/logsum.h"

// int main(int argc, char* argv[]) {

    // const char* optstring = "r:b:g:";
    // static struct option long_options[] = {
    //     {"reads", required_argument, 0, 'r'},          //0 fastq/fasta read file
    //     {"bam", required_argument, 0, 'b'},            //1 sorted bam file
    //     {"genome", required_argument, 0, 'g'}          //2 reference genome
    // };

    // int32_t c = -1;
    // int longindex = 0;
    // char* bamfilename = NULL;
    // char* fastafile = NULL;
    // char* fastqfile = NULL;
    // char *tmpfile = NULL;
    // char *eventalignsummary = NULL;

    // while ((c = getopt_long(argc, argv, optstring, long_options, &longindex)) >= 0) {
    //     if (c == 'r') {
    //         fastqfile = optarg;
    //     } else if (c == 'g') {
    //         fastafile = optarg;
    //     } else if (c == 'b') {
    //         bamfilename = optarg;
    //     } 
    // }

    // opt_t opt;
    // init_opt(&opt); //initialise options to defaults

    // //initialise the core data structure
    // core_t* core = init_core(bamfilename, fastafile, fastqfile, tmpfile, opt, realtime(), 0, eventalignsummary);
	// db_t* db = init_db(core);
    
int main(int argc, char* argv[]) {
    
	FILE *fp;
	fp = fopen("align_parameter", "r+");
    if(fp==NULL) {
        printf("File cannot open! " ); 
        exit(0);
    }
        
    int32_t i, result, sequence_len, original_result;
    event_table events;
    scalings_t scaling;
    float sample_rate;

    // fread(&original_result, sizeof(int32_t), 1, fp);
    fread(&i, sizeof(int32_t), 1, fp);
    fread(&events.n, sizeof(size_t), 1, fp);
    fread(&sequence_len, sizeof(int32_t), 1, fp);

    AlignedPair out_2[events.n];
    char sequence[sequence_len];
    model_t models[4096];
    event_t event[events.n];

    fread(&out_2, sizeof(AlignedPair), events.n, fp);
    fread(&sequence, sizeof(char), sequence_len, fp);
    fread(&event, sizeof(event_t), events.n, fp);
    fread(&models, sizeof(model_t), 4096, fp);
    fread(&scaling, sizeof(scalings_t), 1, fp);
    fread(&sample_rate, sizeof(float), 1, fp);

    fclose(fp);

    FILE *fp1;
    fp1 = fopen("align_result", "r+");
    if(fp1==NULL) {
        printf("File cannot open! " ); 
        exit(0);
    }
    fread(&original_result, sizeof(int32_t), 1, fp1);
    fclose(fp1);

    events.event = event;

	result = align(out_2, sequence, sequence_len, events, models, scaling, sample_rate);

    printf("result = %d\n", result);
    printf("original_result = %d\n", original_result);
    if (result == original_result) printf("Correct!\n");
    else printf("Wrong!\n");
    return 0;
}

