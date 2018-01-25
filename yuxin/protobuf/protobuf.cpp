#include <stdio.h>
#include <stdlib.h>

#include "mzim.login.h"

int main(int argc, char* argv[])
{
    string username = "liuqianguo";

    string password = "123456";

    CMZIMLogin login(username, password);

    string outbuf;

    if(login.ToBuffer(outbuf) > 0)
    {
        MZ::IM mzim;

        if(mzim.ParseFromArray(outbuf.c_str(), outbuf.size()))
        {
           CMZIMLogin login;

           if(login.ToBlock(mzim))
           {
               printf("test login successful [%d:%s:%s]\n\n",
                   login.Status(), login.Username().c_str(), login.Password().c_str());
           }
           else
           {
               printf("toblock login failed\n\n");
           }
        }
        else
        {
            printf("parse login failed\n\n");
        }
    }
    else
    {
        printf("tobuffer login failed\n\n");
    }

    getchar();

    return 0;
}