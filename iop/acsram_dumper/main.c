/**
 *     ___________  ___  ___  ___  ______ _   ____  _________ ___________ 
 *    /  ___| ___ \/ _ \ |  \/  |  |  _  \ | | |  \/  || ___ \  ___| ___ \
 *    \ `--.| |_/ / /_\ \| .  . |  | | | | | | | .  . || |_/ / |__ | |_/ /
 *     `--. \    /|  _  || |\/| |  | | | | | | | |\/| ||  __/|  __||    / 
 *    /\__/ / |\ \| | | || |  | |  | |/ /| |_| | |  | || |   | |___| |\ \ 
 *    \____/\_| \_\_| |_/\_|  |_/  |___/  \___/\_|  |_/\_|   \____/\_| \_|
 *                                                                        
 *  Namco System 2x6 SRAM dumper
 *  Copyright (c) 2025 Matias Israelson - MIT license    
 *                                                                        
 */
#include "irx_imports.h"
#include <errno.h>
#include "acsram_dumper.h"
#include <stdint.h>

#define FUNLOG() DPRINTF("%s\n", __func__)
#define DPRINTF(format, args...) printf(MODNAME ": " format, ##args) // module printf

#define MODNAME "acsram_dumper"
IRX_ID(MODNAME, 1, 1);

static SifRpcDataQueue_t rpcqueue;
static SifRpcServerData_t serverdata;
static int rpcthid;
static unsigned char SifServerBuffer[0x1000];

#define ACRAM_CAPACITY_32MB 0x2000000
#define ACRAM_CAPACITY_64MB 0x4000000
const char ACRAMTESTSTRW[16] = "This Is A Test";
char ACRAMTESTSTRR[16] = {0};

#define SRAM_SIZE 0x8000


static void acram_cb(acRamT ram, void *arg, int result)
{
	int thid = *(int *)arg;
	*(int *)arg = 0;
	// cppcheck-suppress knownConditionTrueFalse
	if (thid)
		WakeupThread(thid);
}
enum OPERATION {
    ACRAM_READ = 0,
    ACRAM_WRITE,
};

static int acram_op(void *acaddr, void *buf, int len, enum OPERATION OP)
{
	acRamData *R;
	acRamData ram_data;
	int ret, thid;

	thid = GetThreadId();
	R = acRamSetup(&ram_data, acram_cb, &thid, 1000000); //ACCD uses 0. ACMEME uses 1000000
    if (OP == ACRAM_WRITE)
	    ret = acRamWrite(R, (acRamAddr)acaddr, buf, len);
    else
	    ret = acRamRead(R, (acRamAddr)acaddr, buf, len);
	if ( ret < 0 )
	{
		return ret;
	}
	while ( thid )
	{
		SleepThread();
	}
	return ret;
}

static void *cmdHandler(int cmd, void *buffer, int nbytes)
{
    printf("%s: CMD %d w buff of %d bytes\n", __FUNCTION__, cmd, nbytes);

    switch (cmd)
    {
    case ACSRAM_DUMP_CHUNK:
        struct DumpSram* Params = buffer;
        printf("acSramRead(0x%x, %p, 0x%x)\n", Params->off, Params->buffer, Params->size);
        Params->result = acSramRead(Params->off, Params->buffer, Params->size);
        break;
    case ACRAM_CHECK_CAPACITY:
        struct CheckAcram* AcCheck = buffer;
        printf("write32\n");
        AcCheck->ram32_write_res = acram_op((void*)ACRAM_CAPACITY_32MB, (void*)ACRAMTESTSTRW, sizeof ACRAMTESTSTRW, ACRAM_WRITE);
        printf("write64\n");
        AcCheck->ram64_write_res = acram_op((void*)ACRAM_CAPACITY_64MB, (void*)ACRAMTESTSTRW, sizeof ACRAMTESTSTRW, ACRAM_WRITE);

        if (AcCheck->ram32_write_res >= 0) {
            AcCheck->ram32_read_res = acram_op((void*)ACRAM_CAPACITY_32MB, (void*)ACRAMTESTSTRR, sizeof ACRAMTESTSTRR, ACRAM_READ);
            AcCheck->ram32_result = !strcmp(ACRAMTESTSTRW, ACRAMTESTSTRR);
            memset(ACRAMTESTSTRR, 0, sizeof(ACRAMTESTSTRR));
        }
        if (AcCheck->ram64_write_res >= 0) {
            AcCheck->ram64_read_res = acram_op((void*)ACRAM_CAPACITY_64MB, (void*)ACRAMTESTSTRR, sizeof ACRAMTESTSTRR, ACRAM_READ);
            AcCheck->ram64_result = !strcmp(ACRAMTESTSTRW, ACRAMTESTSTRR);
        }
        break;
    default:
        DPRINTF("INVALID CMD 0x%X\n");
        break;
    }
    return buffer;
}


static void SifSecrDownloadHeaderThread(void *parameters)
{
    (void)parameters;
    printf("ACSRAM RPC THREAD RUNNING\n");

    if (!sceSifCheckInit()) {
        DPRINTF("sceSifCheckInit 0. initializing SIF\n");
        sceSifInit();
    }

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&rpcqueue, GetThreadId());
    sceSifRegisterRpc(&serverdata, ACSRAM_RPC_IRX, &cmdHandler, SifServerBuffer, NULL, NULL, &rpcqueue);
    sceSifRpcLoop(&rpcqueue);
}

int _start(int argc, char** argv) {
    printf("%s start\n", MODNAME);
    iop_thread_t thread;
    thread.attr      = TH_C;
    thread.priority  = 0x28;
    thread.stacksize = 0x1000;

    thread.thread = &SifSecrDownloadHeaderThread;
    if ((rpcthid = CreateThread(&thread)) == 0) {
        return MODULE_NO_RESIDENT_END;
    }
    StartThread(rpcthid, NULL);


    return MODULE_RESIDENT_END;
}
