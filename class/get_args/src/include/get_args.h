
/*********************************************************
 **************    Function Declaration    ***************
 *********************************************************/
typedef enum ret_value_type{
    RET_CHAR   = 0,     /* return char    */
    RET_INT    = 1,     /* return int    */
    RET_STRING = 2      /* return string */
} ret_value_type;

struct options {
    const char *short_name;  /* Like -h, -t */
    const char *long_name;   /* Like --help, --time*/
    const int  has_args;     /* This option has how many returm parameters */
    const int  value_type;   /* Parameters type, like RET_CHAR*/
    void       **value;      /* Return value */
    int        been_get;     
};

/**
 * @brief parser agruement from  command line
 * 
 * Usage:
 *
 *    int  help_flag = 0;
      char *ip       = NULL;
      int  port      = 0;
      struct options opts[] = {
          {"-h", "--help", 0, RET_INT   , (void **)&help_flag },
          {"-i", "--ip"  , 1, RET_STRING, (void **)&ip },
          {"-p", "--port", 1, RET_INT   , (void **)&port },
          {"-t", "--t"   , 1, RET_INT   , NULL },
          {NULL, NULL    , 0, 0         , NULL}
      };

      parser_args(agrc, agrv, opts);
      printf("help_flag: %d\n", help_flag);
      if (ip != NULL) printf("ip: %s\n", ip);
      printf("port: %d\n", port);
 *
 * @param agrc   [in]  agruement count
 * @param agrv[] [in]  pointer to agruement
 * @param opt    [out] options struct
 */
void get_args(int agrc, char *agrv[], struct options *opt);

