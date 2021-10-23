#include <clibasic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define extname "TEST"

char cbext_name[] = extname;

int main(void) {
    return 0;
}

cb_extargs cb;

bool cbext_init(cb_extargs argv) {
    cb = argv;
    return true;
}

int cbext_runcmd(int argct, char** tmpargs, char** arg, uint8_t* argt, int32_t* argl) {
    if (cb.chkCmd(1, extname".TEST")) {
        if (argct) return 3;
        puts("Test");
        return 0;
    }
    if (cb.chkCmd(1, extname".BRUH")) {
        if (argct) return 3;
        puts("Bruh");
        return 0;
    }
    return 255;
}

cb_funcret cbext_runfunc(int argct, char** tmpargs, char** arg, uint8_t* argt, int32_t* argl, char* outbuf) {
    int ftype = 0;
    if (cb.chkCmd(1, extname".TEST$")) {
        if (argct) return (cb_funcret){3, ftype};
        ftype = 1;
        strcpy(outbuf, "Bruh");
        return (cb_funcret){0, ftype};
    }
    return (cb_funcret){127, ftype};
}
