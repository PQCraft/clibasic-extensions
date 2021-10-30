#include <clibasic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>

// Safe mode extension
// Disables potentially dangerous commands and functions by overriding them

#define P "SM."

char cbext_name[] = "safemode";

char* startcwd = NULL;

int main(void) {
    return 0;
}

static inline int isFile(char* path) {
    struct stat pathstat;
    if (stat(path, &pathstat)) return -1;
    return !(S_ISDIR(pathstat.st_mode));
}

static inline void strApndChar(char* str, char c) {
    while (*str) {++str;}
    *str = c;
    ++str;
    *str = 0;
}

#define copyStrApnd(b, a) strcat(a, b)

cb_extargs cb;

bool cbext_init(cb_extargs argv) {
    cb = argv;
    startcwd = getcwd(NULL, 0);
    return true;
}

void cbext_deinit() {
    free(startcwd);
}

int slashct(char* dir) {
    int ct = 0;
    for (; *dir; ++dir) {
        #ifndef _WIN32
        if (*dir == '/') ++ct;
        #else
        if (*dir == '\\') ++ct;
        #endif
    }
    return ct;
}

int cbext_runcmd(int argct, char** arg, uint8_t* argt, int32_t* argl) {
    if (cb.chkCmd(6, "$", "SH", "EXEC", "EXECA", "CALL", "CALLA")) {
        return 0;
    }
    if (cb.chkCmd(10, "FWRITE", "FLUSH", "MD", "MKDIR", "RM", "REMOVE", "MV", "MOVE", "REN", "RENAME")) {
        return 0;
    }
    if (cb.chkCmd(2, "LOADEXT", "UNLOADEXT")) {
        return 0;
    }
    if (cb.chkCmd(2, "_SETENV", "_UNSETENV")) {
        return 0;
    }
    if (cb.chkCmd(3, "_AUTOCMDHIST", "_SAVECMDHIST", "_LOADCMDHIST")) {
        return 0;
    }
    if (cb.chkCmd(2, "CHDIR", "CD")) {
        int ret = 0, retval = 0;
        if (argct != 1) return 3;
        if (!cb.solvearg(1)) return *cb.cerr;
        if (argt[1] != 1) return 2;
        if (argl[1] < 1) return 16;
        #ifndef _WIN32
        char* newcwd = realpath(arg[1], NULL);
        #else
        char* newcwd = _fullpath(NULL, arg[1], 256);
        #endif
        if (!newcwd) {cb.seterrstr(arg[1]); retval = 17; goto cde;}
        if (slashct(startcwd) > slashct(newcwd)) {retval = 16; goto cde;}
        if (chdir(newcwd)) {
            cb.seterrstr(newcwd);
            retval = 17;
        }
        cde:;
        free(newcwd);
        return retval;
    }
    if (cb.chkCmd(1, "FILES")) {
        if (argct > 1) return 3;
        char* olddn = NULL;
        if (argct) {
            if (!cb.solvearg(1)) return *cb.cerr;
            if (argt[1] != 1) return 2;
            int tmpret = isFile(arg[1]);
            if (tmpret) {
                if (tmpret == -1) {cb.seterrstr(arg[1]); return 15;}
                else {return 19;}
            }
            #ifndef _WIN32
            char* newcwd = realpath(arg[1], NULL);
            #else
            char* newcwd = _fullpath(NULL, arg[1], 256);
            #endif
            if (!newcwd) {
                free(arg[1]);
                cb.seterrstr(arg[1]);
                return 17;
            }
            if (slashct(newcwd) < slashct(startcwd)) {
                return 16;
            }
            olddn = malloc(256);
            char* bret = getcwd(olddn, 256);
            (void)bret;
            if (chdir(arg[1])) {
                free(olddn);
                cb.seterrstr(arg[1]);
                return 17;
            }
        }
        DIR* cwd = opendir(".");
        int ret;
        if (!cwd) {if (argct) {ret = chdir(olddn); free(olddn);} return 0;}
        struct dirent* dir;
        #ifdef _WIN32
            #define DIRPFS "%s\\\n"
            puts(".\\\n..\\");
        #else
            #define DIRPFS "%s/\n"
            puts("./\n../");
        #endif
        long dbegin = telldir(cwd);
        struct stat pathstat;
        while ((dir = readdir(cwd))) {
            stat(dir->d_name, &pathstat);
            if (S_ISDIR(pathstat.st_mode) && strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) printf(DIRPFS, dir->d_name);
        }
        seekdir(cwd, dbegin);
        while ((dir = readdir(cwd))) {
            stat(dir->d_name, &pathstat);
            if (!(S_ISDIR(pathstat.st_mode))) puts(dir->d_name);
        }
        if (argct) {
            ret = chdir(olddn);
            free(olddn);
        }
        closedir(cwd);
        (void)ret;
        return 0;
    }
    if (cb.chkCmd(2, "RUN", "RUNA")) {
        if (argct < 1) return 3;
        bool execa = false;
        char** tmparg = NULL;
        int tmpargct = 0;
        if (!strcmp(arg[0], "RUNA")) {
            if (argct != 1) return 3;
            execa = true;
            int v = -1;
            for (register int i = 0; i < *cb.varmaxct; ++i) {
                if (cb.vardata[i].inuse && !strcmp(arg[1], cb.vardata[i].name)) {v = i; break;}
            }
            if (v == -1 || cb.vardata[v].size == -1) {cb.seterrstr(arg[1]); return 23;}
            if (cb.vardata[v].type != 1) return 2;
            tmparg = arg;
            tmpargct = argct;
            arg = cb.vardata[v].data - 1;
            argct = cb.vardata[v].size + 1;
        } else {
            if (!cb.solvearg(1)) return *cb.cerr;
            if (argt[1] != 1) return 2;
        }
        #ifndef _WIN32
        #ifndef _WIN32
        char* newcwd = realpath(arg[1], NULL);
        #else
        char* newcwd = _fullpath(NULL, arg[1], CB_BUF_SIZE);
        #endif
        if (!newcwd || slashct(newcwd) < slashct(startcwd) + 1) {
            if (execa) {
                argct = tmpargct;
                arg = tmparg;
            }
            return 16;
        }
        char** runargs = (char**)malloc((argct + 3) * sizeof(char*));
        runargs[0] = cb.startcmd;
        runargs[1] = cb.roptstr;
        runargs[2] = arg[1];
        argct += 2;
        int argno = 3;
        for (; argno < argct; argno++) {
            if (!execa) if (!cb.solvearg(argno - 1)) {free(runargs); return *cb.cerr;}
            runargs[argno] = arg[argno - 1];
        }
        argct -= 2;
        runargs[argno] = NULL;
        pid_t pid = fork();
        if (pid < 0) return -1;
        else if (pid == 0) {
            execvp(cb.startcmd, runargs);
            exit(0);
        } else if (pid > 0) {
            while (wait(cb.retval) != pid) {}
            *cb.retval = WEXITSTATUS(*cb.retval);
        }
        free(runargs);
        #else
        char* tmpcmd = malloc(CB_BUF_SIZE);
        tmpcmd[0] = 0;
        bool nq;
        if ((nq = winArgNeedsQuotes(startcmd))) copyStrApnd(" \"", tmpcmd);
        copyStrApnd(cb.startcmd, tmpcmd);
        if (nq) strApndChar(tmpcmd, '"');
        copyStrApnd(" -x", tmpcmd);
        for (int argno = 1; argno <= argct; ++argno) {
            if (!execa) if (argno > 1) if (!cb.solvearg(argno)) {free(tmpcmd); return *cb.cerr;}
            strApndChar(tmpcmd, ' ');
            bool nq = winArgNeedsQuotes(arg[argno]);
            if (nq) strApndChar(tmpcmd, '"');
            copyStrApnd(arg[argno], tmpcmd);
            if (nq) strApndChar(tmpcmd, '"');
        }
        int ret = system(tmpcmd);
        (void)ret;
        free(tmpcmd);
        #endif
        if (execa) {
            argct = tmpargct;
            arg = tmparg;
        }
        cb.updateTxtAttrib();
        return 0;
    }
    return 255;
}

cb_funcret cbext_runfunc(int argct, char** arg, uint8_t* argt, int32_t* argl, char* outbuf) {
    outbuf[0] = '0';
    outbuf[1] = 0;
    if (cb.chkCmd(6, "SH", "SH$", "EXEC", "EXECA", "EXEC$", "EXECA$")) {
        if (arg[0][argl[0] - 1] == '$') outbuf[0] = 0;
        return (cb_funcret){0, 2 - (arg[0][argl[0] - 1] == '$')};
    }
    if (cb.chkCmd(11, "FWRITE", "FLUSH", "MD", "MKDIR", "RM", "REMOVE", "MV", "MOVE", "REN", "RENAME", "_HOME$")) {
        if (arg[0][argl[0] - 1] == '$') outbuf[0] = 0;
        return (cb_funcret){0, 2 - (arg[0][argl[0] - 1] == '$')};
    }
    if (cb.chkCmd(3, "LOADEXT", "UNLOADEXT", "READEXTNAME$")) {
        if (arg[0][argl[0] - 1] == '$') outbuf[0] = 0;
        return (cb_funcret){0, 2 - (arg[0][argl[0] - 1] == '$')};
    }
    if (cb.chkCmd(2, "CD", "CHDIR")) {
        int ret = 0, retval = 0;
        if (argct != 1) {return (cb_funcret){3, 1};}
        if (argt[1] != 1) {return (cb_funcret){2, 1};}
        if (argl[1] < 1) {return (cb_funcret){16, 1};}
        #ifndef _WIN32
        char* newcwd = realpath(arg[1], NULL);
        #else
        char* newcwd = _fullpath(NULL, arg[1], CB_BUF_SIZE);
        #endif
        outbuf[0] = '0';
        outbuf[1] = 0;
        if (newcwd) {
            if (slashct(startcwd) <= slashct(newcwd)) outbuf[0] = '0' + !chdir(newcwd);
        }
        else {retval = 17;}
        free(newcwd);
        return (cb_funcret){retval, 2};
    }
    if (cb.chkCmd(1, "FOPEN")) {
        int cerr = 0;
        *cb.fileerror = 0;
        errno = 0;
        if (argct != 2) return (cb_funcret){3, 2};
        if (argt[1] != 1 || argt[2] != 1) return (cb_funcret){2, 2};
        #ifndef _WIN32
        char* newcwd = realpath(arg[1], NULL);
        #else
        char* newcwd = _fullpath(NULL, arg[1], CB_BUF_SIZE);
        #endif
        if (!newcwd || slashct(newcwd) < slashct(startcwd) + 1 || !isFile(arg[1])) {
            outbuf[0] = '-';
            outbuf[1] = '1';
            outbuf[2] = 0;
            *cb.fileerror = EINVAL;
            if (newcwd) free(newcwd);
            return (cb_funcret){cerr, 2};
        }
        sprintf(outbuf, "%d", cb.openFile(arg[1], arg[2]));
        free(newcwd);
        return (cb_funcret){cerr, 2};
    }
    if (cb.chkCmd(1, "FILES$")) {
        *cb.fileerror = 0;
        errno = 0;
        if (argct > 1) return (cb_funcret){3, 1};
        char* olddn = NULL;
        char* bret;
        int ret;
        if (argct) {
            if (argt[1] != 1) return (cb_funcret){2, 1};
            olddn = malloc(256);
            bret = getcwd(olddn, 256);
            #ifndef _WIN32
            char* newcwd = realpath(arg[1], NULL);
            #else
            char* newcwd = _fullpath(NULL, arg[1], 256);
            #endif
            errno = EINVAL;
            if (!newcwd || slashct(newcwd) < slashct(startcwd) || isFile(arg[1]) || chdir(arg[1])) {
                *cb.fileerror = errno;
                free(olddn);
                outbuf[0] = 0;
                if (newcwd) free(newcwd);
                return (cb_funcret){0, 1};
            }
            free(newcwd);
        }
        (void)bret;
        DIR* cwd = opendir(".");
        if (!cwd) {if (argct) {ret = chdir(olddn); free(olddn);} outbuf[0] = 0; return (cb_funcret){0, 1};}
        struct dirent* dir;
        #ifdef _WIN32
            #define FSC '\\'
            strcpy(outbuf, ".\\\n..\\");
        #else
            #define FSC '/'
            strcpy(outbuf, "./\n../");
        #endif
        struct stat pathstat;
        while ((dir = readdir(cwd))) {
            stat(dir->d_name, &pathstat);
            if (S_ISDIR(pathstat.st_mode) && strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
                strApndChar(outbuf, '\n');
                copyStrApnd(dir->d_name, outbuf);
                strApndChar(outbuf, FSC);
            }
        }
        closedir(cwd);
        cwd = opendir(".");
        while ((dir = readdir(cwd))) {
            stat(dir->d_name, &pathstat);
            if (!(S_ISDIR(pathstat.st_mode))) {
                strApndChar(outbuf, '\n');
                copyStrApnd(dir->d_name, outbuf);
            }
        }
        if (argct) {
            ret = chdir(olddn);
            free(olddn);
        }
        closedir(cwd);
        (void)ret;
        return (cb_funcret){0, 1};
    }
    if (cb.chkCmd(1, "ISFILE")) {
        *cb.fileerror = 0;
        if (argct != 1) {return (cb_funcret){3, 2};}
        if (argt[1] != 1) {return (cb_funcret){2, 2};}
        if (!arg[1][0]) {*cb.fileerror = EINVAL; return (cb_funcret){16, 2};}
        #ifndef _WIN32
        char* newcwd = realpath(arg[1], NULL);
        #else
        char* newcwd = _fullpath(NULL, arg[1], CB_BUF_SIZE);
        #endif
        struct stat pathstat;
        if (!newcwd || slashct(newcwd) < slashct(startcwd) + 1 || stat(arg[1], &pathstat)) {
            outbuf[0] = '-';
            outbuf[1] = '1';
            outbuf[2] = 0;
            *cb.fileerror = errno;
            return (cb_funcret){0, 2};
        }
        outbuf[0] = '0' + !(S_ISDIR(pathstat.st_mode));
        outbuf[1] = 0;
        return (cb_funcret){0, 2};
    }
    return (cb_funcret){127, 0};
}

