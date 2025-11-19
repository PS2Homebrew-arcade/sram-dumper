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

#include "acsram_rpc.h"
#include <errno.h>

int acsram_dumper_init(void);
int dump_sram(void* buf, uint32_t readsize, uint32_t off);