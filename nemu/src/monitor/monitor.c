/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
#include <elf.h>

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
//  Log("Exercise: Please remove me in the source code and compile NEMU again.");
//  assert(0);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>

void sdb_set_batch_mode();

static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;

static int difftest_port = 1234;

struct fuction_unit{
  char function_name[256];
  word_t in_addr;
  word_t out_addr;
}func[2000];
static char *elf_file = NULL;

 static char strtab_info[20000];
uint32_t func_cnt = 0;
/*
type = 0:none
type = 1: call
type = 2: ret

*/
void ftrace_display(uint32_t addr)
{
  for (int i = 0 ; i < func_cnt ; i++)
{
    if(addr == func[i].in_addr)
    {
      Log("<%x> call [%s @%x]\n", addr, func[i].function_name, func[i].in_addr);
      return;
    }
    if(addr == func[i].out_addr)
    {
      Log("<%x> ret [%s]\n", addr, func[i].function_name);
      return;      
    }
}
  return;
}


static bool check_elf_info(FILE * fp)
{
    fseek(fp, 0, SEEK_SET);//将fp设置到开头
 	  Elf32_Ehdr ehdr;   
    int ret = fread(&ehdr, sizeof(ehdr), 1, fp);
    if(ret != 1)
    {
        Log("Load Error in Elf Header File!!\n");
        assert(0);
        return false;
    }

    if(!((ehdr.e_ident[0] == 0x7f) && (ehdr.e_ident[1] == 'E') && (ehdr.e_ident[2] == 'L') && (ehdr.e_ident[3] == 'F')))
    {
      Log("The Elf Header's magic number is wrong!\n");
      assert(0);
      return false;
    }

    if(ehdr.e_ident[4] != ELFCLASS32)
    {
      Log("The Elf ISA doesn't support riscv32-nemu!\n");
      assert(0);
      return false;
    }
    
    if(ehdr.e_ident[5] != ELFDATA2LSB)
    {
      Log("The Elf data doesn't support Little Endian storage!");
      assert(0);
      return false;
    }
  return true;
}

static void load_elf()
{
    if(elf_file == NULL)
    {
        Log("There is no elf file to be read. \n ");
        return;
    }
    else
      Log("elf_file is not NULL!\n");
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp , "Invalid Elf File! Unable to open the file!\n ");
    fseek(fp, 0, SEEK_SET);
    if(check_elf_info(fp) == false) return; 
    if(check_elf_info(fp)) Log("elf_file check successfully!!! \n");
    
    Elf32_Ehdr ehdr;
    fseek(fp, 0, SEEK_SET);
    assert(fread(&ehdr, sizeof(ehdr), 1, fp) == 1);
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    Log("Elf size = %d\n",size);
    Elf32_Half sh_cnt = ehdr.e_shnum, sh_sz = ehdr.e_shentsize;
    Elf32_Off sh_off = ehdr.e_shoff;
    //next, we are going to find the strtab and symtab to collect the information of fuctions!
    fseek(fp, sh_off, SEEK_SET);
    for (int i = 0 ; i < sh_cnt ; i++)
    {
      Elf32_Shdr shdr;
      //move the section header pointer to the now section!
      fseek(fp, sh_off + i * sh_sz, SEEK_SET);
      int ret = fread(&shdr, sizeof(Elf32_Shdr), 1, fp);
      assert(ret == 1);
      if(shdr.sh_type == SHT_SYMTAB)
      {
        Elf32_Shdr symtab = shdr;
        Elf32_Half symnum = shdr.sh_size / shdr.sh_entsize;
        Elf32_Shdr strtab;
        assert(fread(&strtab, sh_sz, 1, fp) == 1);
        fseek(fp, strtab.sh_offset, SEEK_SET);
        //store the strtab strings to memory the function name
        assert(fread(&strtab_info, strtab.sh_size, 1, fp) == 1);
        fseek(fp, symtab.sh_offset, SEEK_SET);
        for (Elf32_Half num = 0 ; num < symnum ; num++)
        {
          Elf32_Sym sym;
          assert(fread(&sym, sizeof(Elf32_Sym), 1, fp) == 1);
          if(ELF32_ST_TYPE(sym.st_info) == STT_FUNC)
          {
              func[func_cnt].in_addr = sym.st_value;
              func[func_cnt].out_addr = sym.st_value + sym.st_size;
              strcpy(func[func_cnt].function_name, &(strtab_info[sym.st_name]));
              func_cnt++;
          }
        }
      break;
      }
    }
    
   fclose(fp);
    Log("The Elf file loaded successfully!\n");
}






static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}
//parse the args from a long instruction, like a "decoder"

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"elf"      , required_argument, NULL, 'e'},
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 }, 
  };//defined the option parameters and their behaviours
  int o;//fetch the argv and parse 
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) {
    switch (o) {//generate different control messages
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'e': elf_file = optarg; break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-e,--elf=FILE           load the elf file to the nemu\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* Perform ISA dependent initialization. */
  init_isa();

  /*load the elf file to support the ftrace*/
  load_elf();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));

  /* Display welcome message. */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();//init the random seed
  init_mem();//init the memory
  init_isa();//init the ISA
  load_img();//load the img file
  IFDEF(CONFIG_DEVICE, init_device());//init the device
  welcome();
}
#endif
