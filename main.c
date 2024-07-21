#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#ifdef __GNUC__
#include <getopt.h>
#endif

struct meminfo {
    int32_t screaming;
    void *address;
    uint64_t allocated;
    uint8_t *content;
};

void usage(const char *progname);
void print_bits(uint8_t byte);

int bflag = 0, hflag = 0;

int main(int argc, char **argv)
{
    char ch;

    while ((ch = getopt(argc, argv, "bh")) != -1) {
        switch (ch) {
        case 'b':
            bflag = 1;
            break;
        case 'h':
            hflag = 1;
            break;
        default:
            usage(strrchr(*argv, '/') + 1);
        }
    }

    argc -= optind;

    if (argc == 0)
        usage(strrchr(*argv, '/') + 1);

    argv += optind;

    FILE *dump_file = fopen(*argv, "r");
    if (!dump_file) {
        fprintf(stderr, "Invalid memory dump '%s'\n", argv[1]);
        return 1;
    }

    int magic = 0;
    fread(&magic, sizeof(int), 1, dump_file);

    printf("Memory dump:\n");

    while (1) {
        struct meminfo info = {0};
        int res = 0;

        res = fread(&info.screaming, sizeof(int32_t), 1, dump_file);
        if (!res)
            break;

        res = fread(&info.address, sizeof(void*), 1, dump_file);
        if (!res)
            break;

        res = fread(&info.allocated, sizeof(uint64_t), 1, dump_file);
        if (!res)
            break;

        if (magic) {
            info.content = malloc(info.allocated);
            fread(info.content, sizeof(uint8_t), info.allocated, dump_file);

            if (!res)
                break;
        }

        printf("Pointer\t\t= %p\n", info.address);
        printf("Allocated\t= %ld bytes\n", info.allocated);
        printf("Unused\t\t= %s\n", info.screaming ? "YES" : "NO");

        if (magic) {
            if (bflag) {
                for (uint64_t i = 0; i < info.allocated; i++) {
                    print_bits(info.content[i]);
                    putchar(' ');
                }
                putchar('\n');
                free(info.content);
                continue;
            }

            if (hflag) {
                for (uint64_t i = 0; i < info.allocated; i++)
                    printf("%X ", info.content[i]);
                putchar('\n');
                free(info.content);
                continue;
            }

            for (uint64_t i = 0; i < info.allocated; i++)
                printf("%d ", info.content[i]);
            putchar('\n');
            free(info.content);
        }
    }

    fclose(dump_file);
    return 0;
}

void usage(const char *progname)
{
    fprintf(stderr, "usage: %s [-bh] memdump\n", progname);
    exit(1);
}

void print_bits(uint8_t byte)
{
    for (int i = 7; i >= 0; i--)
        printf("%d", (byte >> i) & 1);
}
