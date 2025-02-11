#include "app_cli.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "dev_cdc_acm.h"

__attribute__((weak)) void cli_fr_writer(char ch)
{
    dev_cdc_acm_send(0, &ch, 1, 0);
    //printf("%c",ch);
}

__attribute__((weak)) int cli_fr_read(char *ch)
{
    return fread(ch, 1, 1, stdin);
}

static int cmd_free(int argc, char **argv)
{
    printf("[freertos] total memory size: %d, free size: %d\r\n", 
        configTOTAL_HEAP_SIZE, 
        xPortGetFreeHeapSize());
    return 0;
}

static char task_list_str[512];
static int cmd_ps(int argc, char **argv)
{
    const char *const header = "Task       State     Priority  Stack	#\r\n************************************************\r\n";
    strcpy(task_list_str, header);
    vTaskList(task_list_str + strlen(header));
    printf("%s\r\n", task_list_str);
    return 0;
}

static int cmd_time(int argc, char **argv)
{
    TickType_t tick = xTaskGetTickCount();
    float now = tick * 1.0f / (float)configTICK_RATE_HZ;
    printf("frtos running time: %.4f\r\n", now);
    return 0;
}

static const struct cli_cmdmap_s fr_cmdmap[] =
{
    { "free",     cmd_free,    1, 1, "free: display remain memory size" },
    { "ps",       cmd_ps,      1, 1, "ps: display task list" },
    { "time",     cmd_time,    1, 1, "time: display running timestamp" },
};

void app_fr_cli_task(void *p)
{
    for (;;) {
        cli_task();
    }
}

void app_fr_cli_init()
{
    int i;
    cli_init(cli_fr_read, cli_fr_writer, NULL);

    for (i = 0; i < sizeof(fr_cmdmap) / sizeof(fr_cmdmap[0]) ;i++) {
        cli_register(fr_cmdmap[i].cmd, fr_cmdmap[i].handler, 
            fr_cmdmap[i].minargs, fr_cmdmap[i].maxargs, fr_cmdmap[i].usage);
    }

    xTaskCreate(app_fr_cli_task, "cli", 1024, NULL, 1, NULL);
}

