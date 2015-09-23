/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
#include <time.h>
#include <pthread.h>
/*
detect multicore systems
Im fairly sure this is obsolete with the MPI additions
MPI uses nprocs and myid for task identification
*/
// int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
#include "ChainWalkContext.h"
#include "mpi.h"
void* RunMulticore();
FILE* file;
CChainWalkContext cwc;
int nRainbowChainLen;

void Usage()
{
	rtgen_logo();
	Logo();
	// printf("Total Tasks = %d My TaskID = %d\n", nprocs,myid);
	printf("usage: rtgen hash_algorithm \\\n");
	printf("             plain_charset plain_len_min plain_len_max \\\n");
	printf("             rainbow_table_index \\\n");
	printf("             rainbow_chain_length rainbow_chain_count \\\n");
	printf("             file_title_suffix\n");
	printf("       rtgen hash_algorithm \\\n");
	printf("             plain_charset plain_len_min plain_len_max \\\n");
	printf("             rainbow_table_index \\\n");
	printf("             -bench\n");
	printf("\n");

	CHashRoutine hr;
	printf("hash_algorithm:       available: %s\n", hr.GetAllHashRoutineName().c_str());
	printf("plain_charset:        use any charset name in charset.txt here\n");
	printf("                      use \"byte\" to specify all 256 characters as the charset of the plaintext\n");
	printf("plain_len_min:        min length of the plaintext\n");
	printf("plain_len_max:        max length of the plaintext\n");
	printf("rainbow_table_index:  index of the rainbow table\n");
	printf("rainbow_chain_length: length of the rainbow chain\n");
	printf("rainbow_chain_count:  count of the rainbow chain to generate\n");
	printf("file_title_suffix:    the string appended to the file title\n");
	printf("                      add your comment of the generated rainbow table here\n");
	printf("-bench:               do some benchmark\n");
	printf("-bench-csv:           do some benchmark save output in CSV (usefull for graph data)\n");

	printf("\n");
	printf("example: rtgen lm alpha 1 7 0 100 16 test\n");
	printf("         rtgen md5 byte 4 4 0 100 16 test\n");
	printf("         rtgen sha1 numeric 1 10 0 100 16 test\n");
	printf("         rtgen lm alpha 1 7 0 -bench\n");
	printf("         rtgen lm alpha 1 7 0 -bench-csv\n");
}

void Bench(string sHashRoutineName, string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int id, int tpids)
{
	// Setup CChainWalkContext
	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		printf("hash routine %s not supported\n", sHashRoutineName.c_str());
		return;
	}
	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
		return;
	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	{
		printf("invalid rainbow table index %d\n", nRainbowTableIndex);
		return;
	}

	// Bench hash
	{
	CChainWalkContext cwc;
	cwc.GenerateRandomIndex();
	cwc.IndexToPlain();

	clock_t t1 = clock();
	int nLoop = 2500000;
	int i;
	for (i = 0; i < nLoop; i++)
		cwc.PlainToHash();
	clock_t t2 = clock();
	float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;

	/*
	I think this area here might be the easest to work with in order to migrate over to an MPI compatible environment
	as this only calculates hash speeds im sure we can increase that by paralizing the hashing algorithm
	*/
	printf("Process: %d of %d on node %s reports: %s hash speed: %d / s\n", id, tpids, GetNodeName(), sHashRoutineName.c_str(), int(nLoop / fTime));
	}

	// Bench step
	{
	CChainWalkContext cwc;
	cwc.GenerateRandomIndex();

	clock_t t1 = clock();
	int nLoop = 2500000;
	int i;
	for (i = 0; i < nLoop; i++)
	{
		cwc.IndexToPlain();
		cwc.PlainToHash();
		cwc.HashToIndex(i);
	}
	clock_t t2 = clock();
	float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;

	printf("Process: %d of %d on node %s reports: %s step speed: %d / s\n", id, tpids, GetNodeName(), sHashRoutineName.c_str(), int(nLoop / fTime));
	}
}

void BenchCSV(string sHashRoutineName, string sCharsetName, int nPlainLenMin, int nPlainLenMax, int nRainbowTableIndex, int id, int tpids)
{
	// Setup CChainWalkContext
	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))
	{
		printf("hash routine %s not supported\n", sHashRoutineName.c_str());
		return;
	}
	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))
		return;
	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))
	{
		printf("invalid rainbow table index %d\n", nRainbowTableIndex);
		return;
	}

	// Bench hash
	{
	CChainWalkContext cwc;
	cwc.GenerateRandomIndex();
	cwc.IndexToPlain();

	clock_t t1 = clock();
	int nLoop = 2500000;
	int i;
	for (i = 0; i < nLoop; i++)
		cwc.PlainToHash();
	clock_t t2 = clock();
	float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;

	/*
	I think this area here might be the easest to work with in order to migrate over to an MPI compatible environment
	as this only calculates hash speeds im sure we can increase that by paralizing the hashing algorithm
	*/
	if (id == 0)
		printf("id of total on name,hash_type,function,speed\n");
	printf("%d of %d on %s,%s,hash,%d\n", id, tpids, GetNodeName(), sHashRoutineName.c_str(), int(nLoop / fTime));
	}

	// Bench step
	{
	CChainWalkContext cwc;
	cwc.GenerateRandomIndex();

	clock_t t1 = clock();
	int nLoop = 2500000;
	int i;
	for (i = 0; i < nLoop; i++)
	{
		cwc.IndexToPlain();
		cwc.PlainToHash();
		cwc.HashToIndex(i);
	}
	clock_t t2 = clock();
	float fTime = 1.0f * (t2 - t1) / CLOCKS_PER_SEC;
	
	// printf("id,total,name,hash_type,function,speed\n");
	printf("%d of %d on %s,%s,step,%d\n", id, tpids, GetNodeName(), sHashRoutineName.c_str(), int(nLoop / fTime));
	}
}



int main(int argc, char* argv[])
{
	string sHashRoutineName,sCharsetName,sFileTitleSuffix;
	int nPlainLenMin,nPlainLenMax,nRainbowTableIndex,nRainbowChainCount;
	//printf("argc:%d\n",argc);

	//Rocks Cluster Support
	int nprocs, myid, merror;
	merror = MPI_Init(&argc,&argv);
	if (merror != 0) {
		printf("Error initializing MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD,merror);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	//printf("Number of processors or tasks = %d My rank= %d\n", nprocs,myid);
	//End Cluster additions
	
	if (argc == 7)
	{
		if (strcmp(argv[6], "-bench") == 0)
		{
			Bench(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), myid, nprocs);
			MPI_Finalize();
			return 0;
		}
		else if (strcmp(argv[6], "-bench-csv") == 0)
		{
			BenchCSV(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), myid, nprocs);
			MPI_Finalize();
			return 0;
		}
	}

	if (argc != 9 && argc != 2)
	{
		if (myid == 0)
			Usage();
		return 0;
	}
	pthread_t thread1, thread2, thread3, thread4;
	int iret1,iret2,iret3,iret4;
	char * pch;
	if(argc==2){
		pch = strtok (argv[1],"_");
		//printf("Routinename:%s\n",pch);
		sHashRoutineName  = pch;					//Algorith type e.g md5
		
		pch = strtok (NULL, "#");
		//printf("charset:%s\n",pch);
		sCharsetName      = pch;					//charset name
		
		pch = strtok(NULL,"-");
		//printf("min:%s\n",pch);
		nPlainLenMin         = atoi(pch);			//minimum length
		
		pch = strtok(NULL,"_");
		//printf("max:%s\n",pch);
		nPlainLenMax         = atoi(pch);			//maximum length
		
		pch = strtok(NULL,"_");
		//printf("index:%s\n",pch);
		nRainbowTableIndex   = atoi(pch);			//table index
		
		pch = strtok(NULL,"x");
		//printf("chainlen:%s\n",pch);
		nRainbowChainLen     = atoi(pch);	 			//chain length
		
		pch = strtok(NULL,"_");
		//printf("chaincount:%s\n",pch);
		nRainbowChainCount   = atoi(pch);			//chain count
		
		pch = strtok(NULL,"_.");
		//printf("suffix:%s\n",pch);
		sFileTitleSuffix  = pch;			//suffix
		//return 0;

	}else{
		sHashRoutineName  = argv[1];			//Algorith type e.g md5
		sCharsetName      = argv[2];			//charset name
		nPlainLenMin         = atoi(argv[3]);	//minimum length
		nPlainLenMax         = atoi(argv[4]);	//maximum length
		nRainbowTableIndex   = atoi(argv[5]);	//table index

		nRainbowChainLen     = atoi(argv[6]);	//chain length
		nRainbowChainCount   = atoi(argv[7]);	//chain count
		sFileTitleSuffix  = argv[8];			//suffix
	}
	// nRainbowChainCount check
	if (nRainbowChainCount >= 134217728)		//make sure that chain count is correct to avoid big rt's
	{
		printf("this will generate a table larger than 2GB, which is not supported\n");
		printf("please use a smaller rainbow_chain_count(less than 134217728)\n");
		return 0;
	}

	// Setup CChainWalkContext
	if (!CChainWalkContext::SetHashRoutine(sHashRoutineName))		//check to see if we support the hash
	{
		printf("hash routine %s not supported\n", sHashRoutineName.c_str());
		return 0;
	}
	if (!CChainWalkContext::SetPlainCharset(sCharsetName, nPlainLenMin, nPlainLenMax))		//check charset
		return 0;
	if (!CChainWalkContext::SetRainbowTableIndex(nRainbowTableIndex))		//make sure we have correct index
	{
		printf("invalid rainbow table index %d\n", nRainbowTableIndex);
		return 0;
	}
	CChainWalkContext::Dump();

	// Low priority. We need to be at low priority in order to not interfere with the system's functions
	//and to provide the user with clean usage of the OS
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
#else
	nice(1);
#endif

	// FileName
	char szFileName[256];
	sprintf(szFileName, "%s_%s#%d-%d_%d_%dx%d_%s.rt", sHashRoutineName.c_str(),
													  sCharsetName.c_str(),
													  nPlainLenMin,
													  nPlainLenMax,
													  nRainbowTableIndex,
													  nRainbowChainLen,
													  nRainbowChainCount,
													  sFileTitleSuffix.c_str());	//define filename

	// Open file
	fclose(fopen(szFileName, "a"));				//create the file
	file = fopen(szFileName, "r+b");		//open the file for reading in binary
	if (file == NULL)	//check to see if we opened the file correctly
	{
		printf("failed to create %s\n", szFileName);
		return 0;
	}

	// Check existing chains
	unsigned int nDataLen = GetFileLen(file);
	nDataLen = nDataLen / 16 * 16;					//??????????? Don't get it either
	if (nDataLen == nRainbowChainCount * 16)
	{
		printf("precomputation of this rainbow table already finished\n");
		fclose(file);
		return 0;
	}
	if (nDataLen > 0)
		printf("continuing from interrupted precomputation %d...\n",nDataLen/16);
	fseek(file, nDataLen, SEEK_SET);

	// Generate rainbow table
	printf("generating...\n");
	
	clock_t t1 = clock();
	int i;
	for (i = nDataLen / 16; i < nRainbowChainCount; i++)
	{
		//start to try and run a multithreaded
		RunMulticore();
		/*
		switch(numCPU){
			case 1:
				iret1 = pthread_create(&thread1,NULL,&RunMulticore,0);
			case 2:
				iret1 = pthread_create(&thread1,NULL,&RunMulticore,NULL);
				iret2 = pthread_create(&thread2,NULL,&RunMulticore,NULL);
			case 3:
				iret1 = pthread_create(&thread1,NULL,&RunMulticore,NULL);
				iret2 = pthread_create(&thread2,NULL,&RunMulticore,NULL);
				iret3 = pthread_create(&thread3,NULL,&RunMulticore,NULL);
			case 4:
				iret1 = pthread_create(&thread1,NULL,&RunMulticore,NULL);
				iret2 = pthread_create(&thread2,NULL,&RunMulticore,NULL);
				iret3 = pthread_create(&thread3,NULL,&RunMulticore,NULL);
				iret4 = pthread_create(&thread4,NULL,&RunMulticore,NULL);
			}
		*/
		//end
		//Display status
		if ((i + 1) % 100000 == 0 || i + 1 == nRainbowChainCount)
		{
			clock_t t2 = clock();
			int nSecond = (t2 - t1) / CLOCKS_PER_SEC;					//87
			int chainsleft = nRainbowChainCount - (i+1);				//33454432
			int cl = chainsleft/100000 ;								//334
			int secondsleft=cl*nSecond;									//29058
			//printf("Chains left : %d\nCL : %ld\n",chainsleft,cl);
			//eg 51437
			int hrleft = secondsleft/3600; 								//8
			
			int dleft = hrleft/24;										//0
			
			hrleft=hrleft-(dleft*24);									//8
			
			int mleft = (secondsleft/60)-(hrleft*60)-(dleft*24*60);		//4
			
			int sleft = secondsleft-(mleft*60)-(hrleft*3600)-(dleft*24*3600); //18
			
			//printf("dhms: %d,%d,%d,%d - %d\n",dleft,hrleft,mleft,sleft,secondsleft);
			printf("%d of %d rainbow chains generated (%d m %d s) -> %d%%  ETA: %d days %d hrs %d mins %d sec\n", i + 1,
																	  nRainbowChainCount,
																	  nSecond / 60,
																	  nSecond % 60,
																	  (((i+1)*100)/nRainbowChainCount),
																	  dleft,hrleft,mleft,sleft);
			t1 = clock();
		}
		
	}

	// Close
	MPI_Finalize();
	fclose(file);

	return 0;
}

/*
Can we make this MPI compatible?
seems to me its just writing the same chain data from each process called
makes the files a mess
*/

void* RunMulticore(){
	cwc.GenerateRandomIndex();
	uint64 nIndex = cwc.GetIndex();							// get index
	if (fwrite(&nIndex, 1, 8, file) != 8)
	{
		printf("disk write fail\n");
		goto ENDTHREAD;
	}

	int nPos;
	for (nPos = 0; nPos < nRainbowChainLen - 1; nPos++)		//generate a chain
	{
		cwc.IndexToPlain();
		cwc.PlainToHash();
		cwc.HashToIndex(nPos);
	}

	nIndex = cwc.GetIndex();								//get new index
	if (fwrite(&nIndex, 1, 8, file) != 8)
	{
		printf("disk write fail\n");
		goto ENDTHREAD;
	}
	ENDTHREAD:;
}