#include "commandAnalyze.h"
int printCmdError(const char *cmdName){
    printf("ERROR : %s argement is invalid.\n", cmdName);
    return 0;
}
int commandAnalyze(char *input, cmd_t *cmd)
{

    char *cmdNameArr[12] = {"cd", "ls", "pwd", "puts", "gets", "rm", "mkdir", "rmdir", "sign", "login", "quit", "deluser"};
    int validFlag = 0;

    char *cmdName = strtok(input, " ");

    
    for (int i = 0; i < 12; ++i)
    {

        int ret = strcmp(cmdNameArr[i], cmdName);
        if (ret == 0)
        {
            cmd->cmdType = i;
            validFlag = 1;
            char *parameter1 = strtok(NULL, " ");

            if (parameter1 != NULL)
            {
                if (cmd->cmdType == CMD_PWD || cmd->cmdType == CMD_LS 
                    || cmd->cmdType == CMD_LOGIN || cmd->cmdType == CMD_SIGN 
                    || cmd->cmdType == CMD_QUIT || cmd->cmdType == CMD_DELUSER)
                {
                    printCmdError(cmdName);
                    return 1;
                }

                memcpy(cmd->parameter1, parameter1, strlen(parameter1));

                char *parameter2 = strtok(NULL, " ");

                if (parameter2 != NULL)
                {
                    if (cmd->cmdType == CMD_CD || cmd->cmdType == CMD_RM || cmd->cmdType == CMD_MKDIR || cmd->cmdType == CMD_RMDIR)
                    {
                        printCmdError(cmdName);
                        return 1;
                    }
                    memcpy(cmd->parameter2, parameter2, strlen(parameter2));
                }
            }
            break;
        }
    }
    if (validFlag == 0)
    {
        printf("ERROR : command %s is not found!\n", cmdName);
        return 1;
    }

    char *waste = strtok(NULL, " ");
    if (waste != NULL)
    {
        printCmdError(cmdName);
        return 1;
    }
    //printf("type= %d, p1=%s, p2=%s\n",cmd->cmdType,cmd->parameter1,cmd->parameter2);
    //printf("command sending...\n");
    return 0;
}
