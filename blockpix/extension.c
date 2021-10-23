#include <clibasic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <blockpix.h>

#define P "BP."

char cbext_name[] = "blockpix";

int main(void) {
    return 0;
}

cb_extargs cb;

bool cbext_init(cb_extargs argv) {
    cb = argv;
    return true;
}

int cbext_runcmd(int argct, char** tmpargs, char** arg, uint8_t* argt, int32_t* argl) {
    if (cb.chkCmd(1, P"QUIT")) {
        if (argct) return 3;
        bp_quit();
        return 0;
    }
    if (cb.chkCmd(1, P"SILENT_QUIT")) {
        if (argct) return 3;
        bp_silent_quit();
        return 0;
    }
    if (cb.chkCmd(1, P"SET")) {
        if (argct != 3) return 3;
        if (!cb.solvearg(1) || !cb.solvearg(2) || !cb.solvearg(3)) return *cb.cerr;
        bp_set(atoi(arg[1]), atoi(arg[2]), atoi(arg[3]));
        return 0;
    }
    if (cb.chkCmd(1, P"IMMEDIATE_SET")) {
        if (argct != 3) return 3;
        if (!cb.solvearg(1) || !cb.solvearg(2) || !cb.solvearg(3)) return *cb.cerr;
        if (argt[1] != 2 || argt[2] != 2 || argt[3] != 2) return 2;
        bp_immediate_set(atoi(arg[1]), atoi(arg[2]), atoi(arg[3]));
        return 0;
    }
    if (cb.chkCmd(1, P"RESIZE")) {
        if (argct) return 3;
        bp_resize();
        return 0;
    }
    if (cb.chkCmd(1, P"RENDER")) {
        if (argct) return 3;
        bp_render();
        return 0;
    }
    if (cb.chkCmd(1, P"SMART_RENDER")) {
        if (argct) return 3;
        bp_smart_render();
        return 0;
    }
    if (cb.chkCmd(1, P"CLEAR")) {
        if (argct) return 3;
        bp_clear();
        return 0;
    }
    if (cb.chkCmd(1, P"FILL")) {
        if (argct != 1) return 3;
        if (!cb.solvearg(1)) return *cb.cerr;
        if (argt[1] != 2) return 2;
        bp_fill(atoi(arg[1]));
        return 0;
    }
    return 255;
}

cb_funcret cbext_runfunc(int argct, char** tmpargs, char** arg, uint8_t* argt, int32_t* argl, char* outbuf) {
    if (cb.chkCmd(1, P"INIT")) {
        if (argct) return (cb_funcret){3, 0};
        outbuf[0] = '0' + bp_init();
        outbuf[1] = 0;
        return (cb_funcret){0, 2};
    }
    if (cb.chkCmd(1, P"GET")) {
        if (argct != 2) return (cb_funcret){3, 0};
        if (argt[1] != 2 || argt[2] != 2) return (cb_funcret){2, 0};
        sprintf(outbuf, "%lu", (long unsigned)bp_get(atoi(arg[1]), atoi(arg[2])));
        return (cb_funcret){0, 2};
    }
    if (cb.chkCmd(1, P"BUILD")) {
        if (argct) return (cb_funcret){3, 0};
        sprintf(outbuf, "%llu", (long long unsigned)BLOCKPIX_LINKED_BUILD);
        return (cb_funcret){0, 2};
    }
    if (cb.chkCmd(1, P"WIDTH")) {
        if (argct) return (cb_funcret){3, 0};
        sprintf(outbuf, "%u", (unsigned)bp_width);
        return (cb_funcret){0, 2};
    }
    if (cb.chkCmd(1, P"HEIGHT")) {
        if (argct) return (cb_funcret){3, 0};
        sprintf(outbuf, "%u", (unsigned)bp_height);
        return (cb_funcret){0, 2};
    }
    return (cb_funcret){127, 0};
}

void cbext_deinit() {
    bp_silent_quit();
    cb.getCurPos();
    if (*cb.curx != 1) putchar('\n');
}

