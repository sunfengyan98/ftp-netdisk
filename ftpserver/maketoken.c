#include <l8w8jwt/encode.h>
#include "maketoken.h"
#include "ds.h"

int maketoken(cmd_t *cmd,char *ptoken)
{
    PRINT_FUNCTION_LOG();
    char* jwt;
    size_t jwt_length;

    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    params.alg = L8W8JWT_ALG_HS512;

    params.sub = "Netdisk";
    params.iss = "Niqiu";
    params.aud = "Daoyou";

    params.iat = 0;
    params.exp = 0x7fffffff; /* Set to expire after 10 minutes (600 seconds). */

    char token[1024] = {0};
    strcat(token,cmd->userinfo.username);
    strcat(token,cmd->userinfo.password);
    params.secret_key = (unsigned char*)token;
    params.secret_key_length = strlen(token);

    params.out = &jwt;
    params.out_length = &jwt_length;

    int r = l8w8jwt_encode(&params);//执行加密函数

   // printf("\n l8w8jwt example HS512 token: %s \n", r == L8W8JWT_SUCCESS ? jwt : " (encoding failure) ");
    memcpy(ptoken,jwt,jwt_length);
    //printf("ptoken:%s\n",ptoken);

    /* Always free the output jwt string! */
    l8w8jwt_free(jwt);

    return 0;
}
